
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>
#include "config.h"
#include <Battery.h>

// Varialble Declarations
float readingValue;   // in Fahrenheit
int batteryLevel;
char charBuffer[11];
int integer;
int fraction;
unsigned long delayTime;
unsigned long sleepTime;
unsigned long startTime;
unsigned long lastStartTime;
int runTimeBeforeOverflow;
int totalRunTime;
boolean justStarted;
unsigned long UL_MAX=4294967295;

// Function Prototypes
void publishIt (float, char *);


// Initialize the libraries we are using
DHT dht(SENSOR_PIN, SENSOR_TYPE);
WiFiClient espClient;
PubSubClient mqtt(espClient);
Battery battery (3300, 4800, A0);


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

    battery.begin(1000, 4.62);
}


void loop() {
    startTime = millis ();
    
    // Turn on the LED so we know that this thing is working
    digitalWrite (PIN_LED, LOW);

    // Check to see if we are connected to the MQTT server, if not, re-connect
    if (! mqtt.connected ()) {
        // we are not connected.
        if (mqtt.connect(SENSOR_NAME, MQTT_USERNAME, MQTT_PASSWORD)) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(mqtt.state());
        }
    }

    // Read and publish the temperature
    readingValue = dht.readTemperature(true);
    if (isnan(readingValue)) {
        Serial.println("Failed to read temperature from DHT sensor!");
        return;
    }
    publishIt (readingValue, FEED_TEMPERATURE);

    
    // Read and publish the humidity
    readingValue = dht.readHumidity();
    if (isnan(readingValue)) {
        Serial.println("Failed to read humidity from DHT sensor!");
        return;
    }
    publishIt (readingValue, FEED_HUMIDITY);

    
    // Read and publish the battery level
    batteryLevel = analogRead (A0);
    // convert battery level to percent
    readingValue = map(batteryLevel, 610, 1000, 0, 100);
    if (isnan(readingValue)) {
        Serial.println("Failed to read battery level!");
        return;
    }
    publishIt (readingValue, FEED_BATTERY_LEVEL);

    
    // Read and publish the battery voltage
    readingValue = analogRead (A0);
    readingValue = batteryLevel * 4.62;
    if (isnan(readingValue)) {
        Serial.println("Failed to read battery voltage!");
        return;
    }
    publishIt (readingValue, FEED_BATTERY_VOLTAGE);

    
    // Read and publish the battery sense
    if (isnan(batteryLevel)) {
        Serial.println("Failed to read Battery Sense!");
        return;
    }
    publishIt (batteryLevel, FEED_BATTERY_SENSE);
    
    
    // Turn off the LED
    digitalWrite (PIN_LED, HIGH);

    // See if we started on time, if not, set the next start time, or, if this is this first time through, just move on
    if (!justStarted && lastStartTime + delayTime != startTime) {
        startTime = lastStartTime + delayTime;
    }

    // If this is our first pass, set justStarted to false, since we won't have just started anymore...
    if (justStarted) {
        justStarted = false;
    } 
    
    // Figure out how much time until the next reading should be taken
    if (millis () < startTime) {
        // this should only be run if the millis () has overflowed (i.e. hit UL_MAX), which is about every forty-nine days
        runTimeBeforeOverflow = UL_MAX - startTime;       // amount of time before the clock went back to zero
        totalRunTime = runTimeBeforeOverflow + millis (); // this is the total run time of the loop up to this point
        sleepTime = delayTime - (totalRunTime + millis ());   // And this is the amount of time to sleep before starting at the top of the loop 
    } else {
        sleepTime = (startTime + delayTime) - millis ();
    }
    
    lastStartTime = startTime;
    
    // take a nap
    delay (sleepTime);
}


void publishIt (float value, char * feed) {
    // Calculate the humidity to one decimal place and the move convert it to a char*
    integer = (int) value;   // compute the integer part of the float 
    fraction = (int) ((value - (float)integer) * 10); 
    snprintf (charBuffer, sizeof(charBuffer), "%d.%d", integer, fraction);

    // Print some more debug info to serial in the hopes that somebody is watching...
    Serial.print("\nPublishing value (");
    Serial.print(charBuffer);
    Serial.print(") to ");
    Serial.print(feed);
    Serial.print(": ");
    
    // Publish the humidity
    if (! mqtt.publish(feed, charBuffer)) {
        Serial.println("Failed");
    } else {
        Serial.println("OK!");
    }

    return;
}




