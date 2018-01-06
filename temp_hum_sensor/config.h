// Sensor configuration
#define SENSOR_PIN  5        // what digital pin we're connected to
#define SENSOR_TYPE DHT11   // DHT 11
#define SENSOR_NAME "Indoor Temp/Humidity Sensor"

// Wireless network configuration
#define WIRELESS_NETWORK  "Valinor"
#define WIRELESS_PASSWORD "Middle-Earth"

// MQTT broker information
#define MQTT_SERVER   "io.adafruit.com"
#define MQTT_USERNAME "verhoffka"
#define MQTT_PASSWORD "6a962ba8963428f0c7af0c79300e51d993b4788b"
#define MQTT_PORT     1883

// MQTT topic information
#define FEED_TEMPERATURE MQTT_USERNAME "/feeds/inside-temp"
#define FEED_HUMIDITY MQTT_USERNAME "/feeds/inside-humidity"
#define FEED_TIME MQTT_USERNAME "/feeds/total-time"

// How often to check (in seconds)
#define SLEEP_TIME 60

// Pin that the LED is connected to that we want to blink
#define PIN_LED 0
