#ifndef CONFIG_H
#define CONFIG_H

// WiFi credentials for this specific location
#define WIFI_SSID     "Enter Wifi SSID"
#define WIFI_PASSWORD "Enter Wifi Password"

// InfluxDB connection (same across all devices, unless you split by location)
#define INFLUX_URL    "Enter INFLUX URL"
#define INFLUX_ORG    "Enter INFLUX Org"
#define INFLUX_BUCKET "Enter INFLUX Bucket"
#define INFLUX_TOKEN "Enter INFLUX Token"
// A unique name for this sensor so you can tell devices apart in the data
#define DEVICE_NAME   "Sensor name (default sheme is sensor_xx)"

#endif
