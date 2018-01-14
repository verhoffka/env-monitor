
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>
#include "config.h"

float humidity;
float temperature;   // in Fahrenheit
char charBuffer[11];
int val_int;
int val_fra;
unsigned long delayTime;
unsigned long sleepTime;
unsigned long startTime;
unsigned long lastStartTime;
int runTimeBeforeOverflow;
int totalRunTime;
boolean justStarted;

unsigned long UL_MAX=4294967295;

// Initialize DHT sensor.
DHT dht(SENSOR_PIN, SENSOR_TYPE);
WiFiClient espClient;
PubSubClient mqtt(espClient);

void setup() {
    pinMode (PIN_LED, OUTPUT);

    delayTime = SLEEP_TIME * 1000;
    justStarted = true;
    
    Serial.begin(115200);
    while (!Serial);

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
    startTime = millis ();
    
    // Turn on the LED so we know that this thing is working
    digitalWrite (PIN_LED, LOW);
    

    snprintf (charBuffer, sizeof(charBuffer), "%d", startTime);
    Serial.print ("Start Time (beginning): ");
    Serial.println (charBuffer);
  
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
    snprintf (charBuffer, sizeof(charBuffer), "%d.%d", val_int, val_fra); 

    // Publish the temperature
    Serial.print("\nSending temperature (");
    Serial.print(charBuffer);
    Serial.print(") to ");
    Serial.print(FEED_TEMPERATURE);
    Serial.print(": ");
    
    if (! mqtt.publish(FEED_TEMPERATURE, charBuffer)) {
        Serial.println("Failed");
    } else {
        Serial.println("OK!");
    }

    // Calculate the humidity to one decimal place and the move convert it to a char*
    val_int = (int) humidity;   // compute the integer part of the float 
    val_fra = (int) ((humidity - (float)val_int) * 10); 
    snprintf (charBuffer, sizeof(charBuffer), "%d.%d", val_int, val_fra);
    
    // Publish the humidity
    Serial.print("\nSending humidity (");
    Serial.print(charBuffer);
    Serial.print(") to ");
    Serial.print(FEED_HUMIDITY);
    Serial.print(": ");
    
    if (! mqtt.publish(FEED_HUMIDITY, charBuffer)) {
        Serial.println("Failed");
    } else {
        Serial.println("OK!");
    }

    // Turn off the LED
    digitalWrite (PIN_LED, HIGH);

    if (!justStarted && lastStartTime + delayTime != startTime) {
        Serial.println ("justStarted is FALSE and lastStartTime + delayTime != startTime");
        startTime = lastStartTime + delayTime;
    }

    if (justStarted) {
        Serial.println ("justStarted is TRUE");
        justStarted = false;
    } else {
        Serial.println ("justStarted is FALSE");
    }

    snprintf (charBuffer, sizeof(charBuffer), "%d", millis ());
    Serial.print ("Current Time: ");
    Serial.println (charBuffer);

    snprintf (charBuffer, sizeof(charBuffer), "%d", startTime);
    Serial.print ("Start Time (end): ");
    Serial.println (charBuffer);

    snprintf (charBuffer, sizeof(charBuffer), "%d", lastStartTime);
    Serial.print ("Last Start Time: ");
    Serial.println (charBuffer);
    
    // Figure out how much time until the next reading should be taken
    if (millis () < startTime) {
        // this should only be run if the millis () has overflowed (i.e. hit UL_MAX), which is about every forty-nine days
        runTimeBeforeOverflow = UL_MAX - startTime;       // amount of time before the clock went back to zero
        totalRunTime = runTimeBeforeOverflow + millis (); // this is the total run time of the loop up to this point
        sleepTime = delayTime - (totalRunTime + millis ());   // And this is the amount of time to sleep before starting at the top of the loop 
    } else {
        sleepTime = (startTime + delayTime) - millis ();
    }

    snprintf (charBuffer, sizeof(charBuffer), "%d", sleepTime);
    Serial.print ("Sleep Time: ");
    Serial.println (charBuffer);
    
    lastStartTime = startTime;
    
    // take a nap
    delay (sleepTime);
}
