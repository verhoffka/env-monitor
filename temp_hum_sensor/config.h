// Sensor configuration
#define SENSOR_PIN  5        // what digital pin we're connected to
#define SENSOR_TYPE DHT11   // DHT 11
#define SENSOR_NAME "Indoor Temp/Humidity Sensor"

// Wireless network configuration
#define WIRELESS_NETWORK  "XXXXXXXX"
#define WIRELESS_PASSWORD "XXXXXXXX"

// MQTT broker information
#define MQTT_SERVER   "io.adafruit.com"
#define MQTT_USERNAME "XXXXXXXX"
#define MQTT_PASSWORD "XXXXXXXX"
#define MQTT_PORT     1883

// MQTT topic information
#define FEED_TEMPERATURE MQTT_USERNAME "/feeds/inside-temp"
#define FEED_HUMIDITY MQTT_USERNAME "/feeds/inside-humidity"

// How often to check (in seconds)
#define SLEEP_TIME 59
