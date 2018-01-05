
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>
#include "config.h"

float humidity;
float temperature;   // in Fahrenheit
char temp_buffer[7];
char humidity_buffer[7];
int val_int;
int val_fra;
int delayTime;
unsigned long sleepTime;
unsigned long startTime;
int runTimeBeforeOverflow;
int totalRunTime;
char time_buffer[7];

// Initialize DHT sensor.
DHT dht(SENSOR_PIN, SENSOR_TYPE);
WiFiClient espClient;
PubSubClient mqtt(espClient);

void setup() {
    pinMode (0, OUTPUT);

    delayTime = SLEEP_TIME * 1000;
    
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
    // Turn on the LED so we know that this thing is working
    digitalWrite (0, LOW);
    
    
    startTime = millis ();
  
    // Read temperature as Fahrenheit (isFahrenheit = true)
    humidity = dht.readHumidity();
    temperature = dht.readTemperature(true);
    
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

    // Check to see if we are connected to the MQTT server, if not,re-connect
    if (! mqtt.connected ()) {
        // we are not connected.
        if (mqtt.connect(SENSOR_NAME, MQTT_USERNAME, MQTT_PASSWORD)) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqtt.state());
        }
    }

    // Calculate the temperature to one decimal place and the move convert it to a char*
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

    // Calculate the humidity to one decimal place and the move convert it to a char*
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

    // Turn off the LED
    digitalWrite (0, HIGH);
    
    // Figure out how much time until the next reading should be taken
    if (millis () < startTime) {
        // this should only be run if the millis () has overflowed (i.e. hit UL_MAX), which is about every forty-nine days
        runTimeBeforeOverflow = UL_MAX - startTime;       // amount of time before the clock went back to zero
        totalRunTime = runTimeBeforeOverflow + millis (); // this is the total run time of the loop up to this point
        sleepTime = delayTime - (totalRunTime + millis ());   // And this is the amount of time to sleep before starting at the top of the loop 
    } else {
        sleepTime = (startTime + delayTime) - millis ();
    }
    
    Serial.print ("Start Time: ");
    Serial.println (startTime);
    
    Serial.print ("Sleep Time: ");
    Serial.println (sleepTime);
    
    // take a nap
    delay (sleepTime);
}
