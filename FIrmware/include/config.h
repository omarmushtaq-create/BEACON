#ifndef CONFIG_H
#define CONFIG_H

// WiFi credentials for this specific location
#define WIFI_SSID     "hope"
#define WIFI_PASSWORD "Fatima786"

// InfluxDB connection (same across all devices, unless you split by location)
#define INFLUX_URL    "https://us-east-1-1.aws.cloud2.influxdata.com"
#define INFLUX_ORG    "BEACONteam"
#define INFLUX_BUCKET "air_quality"
#define INFLUX_TOKEN "-tpUVFdGFzWCe6DF70Xqi1fTmKtnO76niGQXLoaQoq3tKhWqtCq9-Unc8oDTQTBm7tPGJlMuZt8BCJE8wJ8irA=="

// A unique name for this sensor so you can tell devices apart in the data
#define DEVICE_NAME   "sensor_01"

#endif