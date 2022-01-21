#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <Servo.h>
#include <EEPROM.h>

const char *ssid =  "TELUS2819";     // replace with your wifi ssid and wpa2 key
const char *pass =  "5dbw28zvs4";

WiFiClient client;

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "Nathan_Tham"            // Replace it with your username
#define AIO_KEY         "aio_bWOn61d3j4LJVnPevGeMZyUQNaT1"   // Replace with your Project Auth Key

#define led D7

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Servo myservo;  // create servo object to control a servo

Adafruit_MQTT_Subscribe Servocontrol = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/Servotest");
Adafruit_MQTT_Subscribe DCmotorcontrol = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/DCmotortest");
Adafruit_MQTT_Subscribe JJ = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/JJ");

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

      mqtt.subscribe(&Servocontrol);
      mqtt.subscribe(&DCmotorcontrol);
      mqtt.subscribe(&JJ);
      
      myservo.attach(D2);  // attaches the servo on pin 9 to the servo object
      pinMode(D1,OUTPUT);  //DC motor
      pinMode(D7,OUTPUT);  //LED
}
 
void loop() 
{ 
  MQTT_connect(); 

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(20000))) {
    if (subscription == &Servocontrol) {
      byte Servocontrol_state = atoi((char *)Servocontrol.lastread);
      //Serial.print(Servocontrol_state);
      if(Servocontrol_state == 1)
      {
        myservo.write(15);
      }            
      if(Servocontrol_state == 0)
      {
        myservo.write(165);
      } 
    }
    if (subscription == &DCmotorcontrol) {
      byte DCmotorcontrol_state = atoi((char *)DCmotorcontrol.lastread);
      //Serial.print(DCmotorcontrol_state);
      if(DCmotorcontrol_state == 1)
      {
        digitalWrite(D1,HIGH);
      }            
      if(DCmotorcontrol_state == 0)
      {
        digitalWrite(D1,LOW);
      }
    }
    if (subscription == &JJ) {
      byte JJ_state = atoi((char *)JJ.lastread);
      Serial.print(JJ_state);
      if(JJ_state == 1)
      {
        digitalWrite(D7,HIGH);
        delay(2000);
        digitalWrite(D7,LOW);
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
