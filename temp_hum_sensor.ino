
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <PubSubClient.h>

#define DHTPIN 5        // what digital pin we're connected to
#define DHTTYPE DHT11   // DHT 11

const char* ssid     = "XXXXXXXX";
const char* password = "XXXXXXXX";
const char* mqtt_server = "io.adafruit.com";
String mqtt_username = "XXXXXXXX";
String mqtt_password = "XXXXXXXX";
int mqtt_port = 1883;
String url_temp = "/feeds/inside-temp";
String url_humidity = "/feeds/inside-humidity";
const char* temp_feed;
const char* humidity_feed;


float humidity;
float temperature;   // in Fahrenheit
int ret;
char temp_buffer[7];
char humidity_buffer[7];
int val_int;
int val_fra;
String tmp;

// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient mqtt(espClient);

void setup() {

    pinMode (0, OUTPUT);
    
    Serial.begin(115200);
    delay(100);

    tmp = "/" + mqtt_username + url_temp;
    temp_feed = tmp.c_str();

    tmp = "/" + mqtt_username + url_humidity;
    humidity_feed = tmp.c_str();

    Serial.println("Publishing data to: ");
    Serial.println(temp_feed);
    Serial.println(humidity_feed);

    // We start by connecting to a WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.print(ssid);
  
    WiFi.begin(ssid, password);
  
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

    mqtt.setServer (mqtt_server, mqtt_port);

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
    if (mqtt.connect("Indoor Temp/Humidity Sensor", mqtt_username.c_str(), mqtt_password.c_str())) {
        Serial.println("connected");

        val_int = (int) temperature;   // compute the integer part of the float 
        val_fra = (int) ((temperature - (float)val_int) * 10);   // compute 3 decimal places (and convert it to int)
        snprintf (temp_buffer, sizeof(temp_buffer), "%d.%d", val_int, val_fra); //
        //Serial.println(temp_buffer);

        tmp = mqtt_username + url_temp;
        temp_feed = tmp.c_str();
    
        // Publish the temperature
        Serial.print("\nSending temperature (");
        Serial.print(temp_buffer);
        Serial.print(") to ");
        Serial.print(temp_feed);
        Serial.print(": ");
        
        if (! mqtt.publish(temp_feed, temp_buffer)) {
            Serial.println("Failed");
        } else {
            Serial.println("OK!");
        }

        // Split off stuff to the left of the decimal in one variable, and the float into another
        val_int = (int) humidity;   // compute the integer part of the float 
        val_fra = (int) ((humidity - (float)val_int) * 10); 
        snprintf (humidity_buffer, sizeof(humidity_buffer), "%d.%d", val_int, val_fra);
        //Serial.println(humidity_buffer);

        tmp = mqtt_username + url_humidity;
        humidity_feed = tmp.c_str();
        
        // Publish the humidity
        Serial.print("\nSending humidity (");
        Serial.print(humidity_buffer);
        Serial.print(") to ");
        Serial.print(humidity_feed);
        Serial.print(": ");
        if (! mqtt.publish(humidity_feed, humidity_buffer)) {
            Serial.println("Failed");
        } else {
            Serial.println("OK!");
        }
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
    }

    //ret = snprintf(buffer, sizeof buffer, "%0.1f", temperature);

    digitalWrite (0, HIGH);

    delay (59000);
}


