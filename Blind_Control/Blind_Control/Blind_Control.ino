#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <Servo.h>
#include <EEPROM.h>

const char *ssid =  "TELUS2819";     // replace with your wifi ssid and wpa2 key
const char *pass =  "5dbw28zvs4";

WiFiClient client;

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                                  // use 8883 for SSL
#define AIO_USERNAME    "Nathan_Tham"                         // Replace it with your username
#define AIO_KEY         "aio_bWOn61d3j4LJVnPevGeMZyUQNaT1"    // Replace with your Project Auth Key
#define led D7

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);


Servo myservo;      // create servo object to control a servo
int blindopen=180;  //120
int blindclosed=0;  //0

Adafruit_MQTT_Subscribe Blindcontrol = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/Blinds");

void MQTT_connect();

void setup() 
{
       Serial.begin(9600);
       delay(10);
               
       Serial.println("Connecting to ");                                                                
       Serial.println(ssid); 
 
       WiFi.begin(ssid, pass); 
       while (WiFi.status() != WL_CONNECTED) 
          {
            delay(500);
            Serial.print(".");
          }
      Serial.println("");
      Serial.println("WiFi connected"); 

      mqtt.subscribe(&Blindcontrol);
      
      myservo.attach(D2);  // attaches the servo on pin 9 to the servo object
}
 
void loop() 
{ 
  MQTT_connect(); 

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(20000))) {
    if (subscription == &Blindcontrol) { //onoff
      byte Blindcontrol_state = atoi((char *)Blindcontrol.lastread);
      Serial.println(Blindcontrol_state);

      double Variable=(double)Blindcontrol_state;
      double Variablepos = (Variable/100)*180;  //changes percentage into value
      Serial.println(Variablepos);
      
      if(Blindcontrol_state == 1)
      {
        myservo.write(blindclosed);
        Serial.println("closed");
        delay(3000);
      }            
      else if(Blindcontrol_state == 0)
      {
        myservo.write(blindopen);
        Serial.println("open");
        delay(3000);
      } 
      else 
      {
        myservo.write(Variablepos);
        delay(3000);
      }
     }
    } 
}

void MQTT_connect() 
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    //Serial.println("Connected");
    return;
  }

  digitalWrite(led,HIGH);
  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      //while (1);
      Serial.println("Wait 10 min to reconnect");
      delay(600000);
    }
  }
  Serial.println("MQTT Connected!");
  digitalWrite(led,LOW);
}
