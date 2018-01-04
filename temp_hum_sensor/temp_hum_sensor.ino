
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>
#include "config.h"

float humidity;
float temperature;   // in Fahrenheit
int ret;
char temp_buffer[7];
char humidity_buffer[7];
int val_int;
int val_fra;
String tmp;
int delay_time;

// Initialize DHT sensor.
DHT dht(SENSOR_PIN, SENSOR_TYPE);
WiFiClient espClient;
PubSubClient mqtt(espClient);

void setup() {

    pinMode (0, OUTPUT);

    delay_time = SLEEP_TIME * 1000;
    
    Serial.begin(115200);
    delay(100);

    Serial.println("Publishing data to: ");
    Serial.println(FEED_TEMPERATURE);
    Serial.println(FEED_HUMIDITY);

    // We start by connecting to a WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.print(WIRELESS_NETWORK);
  
    WiFi.begin(WIRELESS_NETWORK, WIRELESS_PASSWORD);
  
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println(".connected");  
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Netmask: ");
    Serial.println(WiFi.subnetMask());
    Serial.print("Gateway: ");
    Serial.println(WiFi.gatewayIP());

    mqtt.setServer (MQTT_SERVER, MQTT_PORT);

    dht.begin();
}


void loop() {
    digitalWrite (0, LOW);
  
    humidity = dht.readHumidity();
    temperature = dht.readTemperature(true);  // Read temperature as Fahrenheit (isFahrenheit = true)

    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    // print to the serial port for debugging
    Serial.println ();
    Serial.print ("Time: ");
    Serial.println ("");
    Serial.print ("Temperature: ");
    Serial.println (temperature);
    Serial.print ("Hummidity: ");
    Serial.println (humidity);

    // Attempt to connect
    if (! mqtt.connected ()) {
        if (mqtt.connect(SENSOR_NAME, MQTT_USERNAME, MQTT_PASSWORD)) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqtt.state());
        }
    }

    val_int = (int) temperature;   // compute the integer part of the float 
    val_fra = (int) ((temperature - (float)val_int) * 10);   // compute 1 decimal places (and convert it to int)
    snprintf (temp_buffer, sizeof(temp_buffer), "%d.%d", val_int, val_fra); 

    // Publish the temperature
    Serial.print("\nSending temperature (");
    Serial.print(temp_buffer);
    Serial.print(") to ");
    Serial.print(FEED_TEMPERATURE);
    Serial.print(": ");
    
    if (! mqtt.publish(FEED_TEMPERATURE, temp_buffer)) {
        Serial.println("Failed");
    } else {
        Serial.println("OK!");
    }

    // Split off stuff to the left of the decimal in one variable, and the float into another
    val_int = (int) humidity;   // compute the integer part of the float 
    val_fra = (int) ((humidity - (float)val_int) * 10); 
    snprintf (humidity_buffer, sizeof(humidity_buffer), "%d.%d", val_int, val_fra);
    
    // Publish the humidity
    Serial.print("\nSending humidity (");
    Serial.print(humidity_buffer);
    Serial.print(") to ");
    Serial.print(FEED_HUMIDITY);
    Serial.print(": ");
    if (! mqtt.publish(FEED_HUMIDITY, humidity_buffer)) {
        Serial.println("Failed");
    } else {
        Serial.println("OK!");
    }

    digitalWrite (0, HIGH);

    delay (delay_time);
}


