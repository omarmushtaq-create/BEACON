#ifndef CONFIG_H
#define CONFIG_H

// WiFi credentials for this specific location
#define WIFI_SSID     "Enter Wifi SSID"
#define WIFI_PASSWORD "Enter Wifi Password"

// InfluxDB connection (same across all devices, unless you split by location)
#define INFLUX_URL    "Enter INFLUX URL"
#define INFLUX_ORG    "Enter INFLUX Org"
#define INFLUX_BUCKET "Enter INFLUX Bucket"
//This is the public token for public facing users
#define INFLUX_TOKEN "OKWRyprjj_oX3PjQimaZ-9Vdgke-axpUmChOOew1s-xTWMn6C4i--xRUHcspdPrYlS4XR84Ifxbm11CFvuvtDQ=="
// A unique name for this sensor so you can tell devices apart in the data
#define DEVICE_NAME   "Sensor name (default sheme is sensor_xx)"

#endif
