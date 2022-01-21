#include <ESP8266WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <EEPROM.h>
#include <AccelStepper.h>


const char *ssid =  "TELUS2819";     // replace with your wifi ssid and wpa2 key
const char *pass =  "5dbw28zvs4";

WiFiClient client;

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "Nathan_Tham"            // Replace it with your username
#define AIO_KEY         "aio_bWOn61d3j4LJVnPevGeMZyUQNaT1"   // Replace with your Project Auth Key
//#define led D7

Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

#define dirPin D4
#define stepPin D3
#define motorInterfaceType 1

AccelStepper stepper=AccelStepper(motorInterfaceType, stepPin, dirPin);

Adafruit_MQTT_Subscribe Rollerblindcontrol = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME"/feeds/Roller Blinds");

int speedopen = 2000;
int speedclose = -2000;
int variablespeed;

void MQTT_connect();

void setup() 
{
       pinMode(LED_BUILTIN, OUTPUT); //turn off blue led
       
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

      mqtt.subscribe(&Rollerblindcontrol);

      stepper.setMaxSpeed(2000);
      stepper.setAcceleration(2000);
}
 
void loop() 
{ 
  digitalWrite(LED_BUILTIN, HIGH); //turn off blue led
  MQTT_connect(); 

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(20000))) {
    if (subscription == &Rollerblindcontrol) { //onoff
      
      byte Rollerblindcontrol_state = atoi((char *)Rollerblindcontrol.lastread);
      Serial.println(Rollerblindcontrol_state);

      double Variable=(double)Rollerblindcontrol_state;
      double Variablepos = (Variable/100)*8000; //changes percentage into value
      Serial.println(Variablepos);
      
      //open is 0
      //closed is 1
      if(Rollerblindcontrol_state == 1)
      {
        while (stepper.currentPosition()!= 8000)
        {
//          stepper.moveTo(2072);
//          stepper.run();
          stepper.setSpeed(speedopen);
          stepper.runSpeed();
          yield(); //reset watchdog timer
          Serial.println(stepper.currentPosition());
        }
        delay(2000);
      }            
      else if(Rollerblindcontrol_state == 0)
      {
        while (stepper.currentPosition()!=0)
        {
//          stepper.moveTo(0);
//          stepper.run();
         stepper.setSpeed(speedclose);
         stepper.runSpeed(); 
         yield();
         Serial.println(stepper.currentPosition());
        }
        delay(2000);
      } 
      else 
      {
        while (stepper.currentPosition()!= Variablepos)
        {
          if(stepper.currentPosition()<Variablepos)
          {
            variablespeed = speedopen;
          }
          else
          {
            variablespeed = speedclose;
          }
           stepper.setSpeed(variablespeed);
           stepper.runSpeed();
           yield();
           Serial.println(stepper.currentPosition());
        }
        delay(2000);
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

  //digitalWrite(led,HIGH);
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
  //digitalWrite(led,LOW);
}
