#ifndef CONFIG_H
#define CONFIG_H

// WiFi credentials for this specific location
#define WIFI_SSID     "Enter Wifi SSID"
#define WIFI_PASSWORD "Enter Wifi Password"

// InfluxDB connection (same across all devices, unless you split by location) !! DO NOT CHANGE !!
#define INFLUX_URL    "https://us-east-1-1.aws.cloud2.influxdata.comr"
#define INFLUX_ORG    "BEACON"
#define INFLUX_BUCKET "air_quality"
#define INFLUX_TOKEN "OKWRyprjj_oX3PjQimaZ-9Vdgke-axpUmChOOew1s-xTWMn6C4i--xRUHcspdPrYlS4XR84Ifxbm11CFvuvtDQ=="

// A unique name for this sensor so you can tell devices apart in the data !! YOU MUST CHANGE THIS TO A UNIQUE NAME. CHECK THE README FOR THE NAME YOU SHOULD ENTER!!
#define DEVICE_NAME   "Sensor name (default sheme is sensor_xx)"

#endif
