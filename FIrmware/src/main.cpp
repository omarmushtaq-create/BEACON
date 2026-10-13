#include <Arduino.h>
#include <Wire.h>
#include <zph02.h>
#include <WiFi.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include "config.h"
#include <cmath>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <deque>
#include <algorithm>

#define SHT3X_ADDR 0x44
#define Dust_pin 34

zph02 aqi_sensor(10);

InfluxDBClient client(INFLUX_URL, INFLUX_ORG, INFLUX_BUCKET, INFLUX_TOKEN);
Point sensorData("environment");

// ---------------------------------------------------------------------------
// Timing
// ---------------------------------------------------------------------------
const unsigned long normalLoopDelay      = 1000;   // sample every 1 s when values are changing
const unsigned long normalUploadInterval = 5000;   // upload every 5 s when values are changing
const unsigned long restLoopDelay        = 60000;  // sample every 60 s in rest mode
const unsigned long restUploadInterval   = 60000;  // upload every 60 s in rest mode
const unsigned long wifiConnectTimeout   = 20000;  // give up waiting for Wi-Fi in setup() after 20 s

// ---------------------------------------------------------------------------
// Rest mode detection
// Rest mode is entered when the last maxSize readings of ALL three values stay
// inside a small band (tolerance), and left as soon as one of them moves out.
// ---------------------------------------------------------------------------
const size_t maxSize       = 5;
const float  tempTolerance = 0.2f;  // degrees C
const float  humTolerance  = 1.0f;  // % relative humidity
const float  pmTolerance   = 1.0f;  // ug/m3

// Global, so the history survives between loop() calls
std::deque<float> pcon, phum, ptemp;

bool isInRest = false;
unsigned long lastUploadTime = 0;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// CRC-8 used by the SHT3x (polynomial 0x31, init 0xFF)
uint8_t crc8(const uint8_t* data, int len) {
    uint8_t crc = 0xFF;
    for (int i = 0; i < len; i++) {
        crc ^= data[i];
        for (int bit = 0; bit < 8; bit++) {
            if (crc & 0x80) {
                crc = (uint8_t)((crc << 1) ^ 0x31);
            } else {
                crc = (uint8_t)(crc << 1);
            }
        }
    }
    return crc;
}

// Adds a sample and keeps only the newest maxSize samples
void pushSample(std::deque<float>& d, float value) {
    d.push_back(value);
    while (d.size() > maxSize) {
        d.pop_front();
    }
}

// True when we have a full history and max - min is within the tolerance
bool isStable(const std::deque<float>& d, float tolerance) {
    if (d.size() < maxSize) {
        return false;
    }
    std::pair<std::deque<float>::const_iterator, std::deque<float>::const_iterator> mm =
        std::minmax_element(d.begin(), d.end());
    return (*mm.second - *mm.first) <= tolerance;
}

// Converts AQI (0-500) to PM2.5 in ug/m3 using the piecewise AQI breakpoints
// (pre-2024 EPA table, whose first band 0-12.0 matches your original "* 12 / 50").
float aqiToPm25(float aqi) {
    static const float aqiLo[] = {0.0f, 51.0f, 101.0f, 151.0f, 201.0f, 301.0f, 401.0f};
    static const float aqiHi[] = {50.0f, 100.0f, 150.0f, 200.0f, 300.0f, 400.0f, 500.0f};
    static const float pmLo[]  = {0.0f, 12.1f, 35.5f, 55.5f, 150.5f, 250.5f, 350.5f};
    static const float pmHi[]  = {12.0f, 35.4f, 55.4f, 150.4f, 250.4f, 350.4f, 500.4f};

    if (std::isnan(aqi) || aqi < 0.0f) {
        aqi = 0.0f;
    }
    if (aqi > 500.0f) {
        aqi = 500.0f;
    }
    for (int i = 0; i < 7; i++) {
        if (aqi <= aqiHi[i]) {
            float a = (aqi < aqiLo[i]) ? aqiLo[i] : aqi;  // value falls in the gap between two bands
            return pmLo[i] + (a - aqiLo[i]) * (pmHi[i] - pmLo[i]) / (aqiHi[i] - aqiLo[i]);
        }
    }
    return pmHi[6];
}

// Reads the SHT3x. Returns {NaN, NaN} if the read fails or the CRC is wrong.
std::pair<float, float> SHTX() {
    const std::pair<float, float> failed(NAN, NAN);

    Wire.beginTransmission(SHT3X_ADDR);
    Wire.write(0x24);
    Wire.write(0x00);
    if (Wire.endTransmission() != 0) {
        return failed;  // sensor did not acknowledge
    }
    delay(20);  // high-repeatability measurement takes up to 15.5 ms
    Wire.requestFrom(SHT3X_ADDR, 6);
    if (Wire.available() != 6) {
        while (Wire.available()) {
            Wire.read();  // throw away any partial data
        }
        return failed;
    }

    uint8_t buf[6];
    for (int i = 0; i < 6; i++) {
        buf[i] = (uint8_t)Wire.read();
    }

    // buf[0..1] = temperature, buf[2] = its CRC, buf[3..4] = humidity, buf[5] = its CRC
    if (crc8(buf, 2) != buf[2] || crc8(buf + 3, 2) != buf[5]) {
        return failed;
    }

    uint16_t rawTemp = (uint16_t)((buf[0] << 8) | buf[1]);
    uint16_t rawHum  = (uint16_t)((buf[3] << 8) | buf[4]);

    float temp = -45.0f + (175.0f * (rawTemp / 65535.0f));
    float hum  = 100.0f * (rawHum / 65535.0f);

    return std::pair<float, float>(temp, hum);
}

float DustSensor() {
    // Current AQI moving average from the sensor library
    float aqi = aqi_sensor.getAqi(false);
    return aqiToPm25(aqi);
}

void uploadData(float temp, float hum, float con) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Wi-Fi not connected, trying to reconnect (skipping this upload)");
        WiFi.reconnect();
        return;
    }

    // Clear any previous fields from the data point
    sensorData.clearFields();

    sensorData.addField("temperature", temp);
    sensorData.addField("humidity", hum);
    sensorData.addField("pm25", con);

    // Print line protocol representation to Serial
    Serial.print("Writing: ");
    Serial.println(client.pointToLineProtocol(sensorData));

    // Write point to server
    if (!client.writePoint(sensorData)) {
        Serial.print("Write failed: ");
        Serial.println(client.getLastErrorMessage());
    }
}

// ---------------------------------------------------------------------------
// Arduino entry points
// ---------------------------------------------------------------------------
void setup() {
    Serial.begin(9600);
    while (!Serial && millis() < 3000);  // wait up to 3 s for a serial monitor, then carry on

    aqi_sensor.reset();

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    unsigned long wifiStart = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - wifiStart < wifiConnectTimeout) {
        Serial.print(".");
        delay(500);
    }
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWi-Fi connected.");
    } else {
        Serial.println("\nWi-Fi not connected yet, will keep retrying in the background.");
    }

    sensorData.addTag("device", DEVICE_NAME);
    client.setInsecure();

    // Verify server connection (only possible when Wi-Fi is up)
    if (WiFi.status() == WL_CONNECTED) {
        if (client.validateConnection()) {
            Serial.print("Connected to InfluxDB: ");
            Serial.println(client.getServerUrl());
        } else {
            Serial.print("InfluxDB connection failed: ");
            Serial.println(client.getLastErrorMessage());
        }
    }

    Wire.begin();
    aqi_sensor.begin(Dust_pin);
    Serial.println("Starting data collection...");
}

void loop() {
    float con = DustSensor();
    std::pair<float, float> th = SHTX();
    float temp = th.first;
    float hum  = th.second;

    // Bad sensor read: do not store it or upload it, just try again soon
    if (std::isnan(temp) || std::isnan(hum)) {
        Serial.println("SHT3x read failed, skipping this cycle");
        delay(normalLoopDelay);
        return;
    }

    // Keep the last maxSize readings
    pushSample(pcon, con);
    pushSample(phum, hum);
    pushSample(ptemp, temp);

    // Rest mode when temperature, humidity and dust have all been steady
    isInRest = isStable(ptemp, tempTolerance) &&
               isStable(phum, humTolerance) &&
               isStable(pcon, pmTolerance);

    unsigned long uploadInterval = isInRest ? restUploadInterval : normalUploadInterval;

    if (millis() - lastUploadTime >= uploadInterval) {
        // Reset the timestamp first so the interval does not drift
        lastUploadTime = millis();

        Serial.print("pm25: ");
        Serial.print(con);
        Serial.print("  temp: ");
        Serial.print(temp);
        Serial.print("  hum: ");
        Serial.println(hum);

        uploadData(temp, hum, con);
    }

    delay(isInRest ? restLoopDelay : normalLoopDelay);
}
