
/* ==========================================================
Project : Smart house
Author: Ko Fukushima and Oscar Morales
Created: 8th 11 2016
Description: Collecting the sensor's information
==============================================================
*/

#include <SoftwareSerial.h>
SoftwareSerial BTSerial(11, 10); // RX | TX

// for photocell
int photoRPin = 0; 
int minLight;          //Used to calibrate the readings
int maxLight;          //Used to calibrate the readings
int lightLevel;
int adjustedLightLevel=0;

// for temp
int sensorPin = 1;

// for motion
int inputPin = 2;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status

// for magnetic sensor
const int sensor = 4;
int state; // 0 close - 1 open wwitch
int state_light = LOW;

// for buzzer
const int buzzer = 3; 

// for rain sensor
int nRainIn = A2;
int nRainDigitalIn = 5;
int nRainVal;
boolean bIsRaining = false;
String strRaining;

String sampleRate;

//for doorbell
int buttonPin = 7;  // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status

//led for room
int ledPin = 9;   

int buttonPin1 = 8;     // the number of the pushbutton pin
// variables will change:
int buttonState1 = 0;         // variable for reading the pushbutton status
boolean buttonpushed = false;
int previous = LOW;    // the previous reading from the input pi
long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers

bool setAlarm = false;
void setup() 
{
   Serial.begin(9600);
   BTSerial.begin(9600);
   
   // set the sample rate
   sampleRate = 1000;
   
   //for photocell
   lightLevel=analogRead(photoRPin);
   minLight=lightLevel-20;
   maxLight=lightLevel;

   // for photocell
   pinMode(inputPin, INPUT);     // declare sensor as input

   // for magnetic sensor
   pinMode(sensor, INPUT_PULLUP);

   // for rain sensor
   pinMode(nRainDigitalIn, INPUT);     // declare sensor as input

   //LED
   pinMode(LED_BUILTIN, OUTPUT);

   // initialize the pushbutton pin as an input:
   pinMode(buttonPin, INPUT);

   //led for room
   pinMode(ledPin, OUTPUT);        // sets the digital pin as output

   // initialize the pushbutton pin as an input:
   pinMode(buttonPin1, INPUT);

}

int getPhotocell() 
{
   //auto-adjust the minimum and maximum limits in real time
   lightLevel=analogRead(photoRPin);
   if(minLight>lightLevel)
   {
     minLight=lightLevel;
   }
   if(maxLight<lightLevel)
   {
     maxLight=lightLevel;
   }
   
   //Adjust the light level to produce a result between 0 and 100.
   adjustedLightLevel = map(lightLevel, minLight, maxLight, 0, 100); 
  
   return adjustedLightLevel;
}

void printPhoto(int photodata) 
{
   String message = "";
   String photodataBT = String(photodata);
   message = "Brightness is " + photodataBT + "\n" + "^";
   BTSerial.print(message);
}

float getTemp()
{
  //getting the voltage reading from the temperature sensor
   int reading = analogRead(sensorPin);  
   
   // converting that reading to voltage, for 3.3v arduino use 3.3
   float voltage = reading * 5.0;
   voltage /= 1024.0; 
   
   // now print out the temperature
   float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                                 //to degrees ((voltage - 500mV) times 100)                                         
                                                 
   return temperatureC;
}

void printTemp(float temp) 
{
  String message = "";
  float temperatureF = (temp * 9.0 / 5.0) + 32.0;   
  String tempBT = String(temp);
  message = tempBT + " degrees C \n" + temperatureF + " degrees F \n" + "^";
  BTSerial.print(message);
}

String detectMotion() 
{
    // for motion 
   val = digitalRead(inputPin);  // read input value
   String motion = "No motion detected\n";
   if (val == HIGH) 
   {            
       motion = "Motion detected!\n";
   } 
   
   return motion;
}

void printdetectMotion(String motionBT)
{
   String message = "";
   message = motionBT + "^";
   BTSerial.print(message);
}

String detectDoorMotion()
{
  state = digitalRead(sensor);
  String doorState = "";
  if (state == HIGH)
  {
    doorState = "Door is open\n";
  }
  else
  {
    doorState = "Door is closed\n";
  }
  return doorState;
}

void printdetectDoorMotion(String doorBT)
{
   String message = "";
   message = doorBT + "^";
   BTSerial.print(message);
}

void detectRain() 
{
   nRainVal = analogRead(nRainIn);
   bIsRaining = !(digitalRead(nRainDigitalIn));
  
   if(bIsRaining)
   {
     strRaining = "YES";
   }
   else
   {
     strRaining = "NO";
   }
   String rainingBT = String(strRaining);
   String nRainValBT = String(nRainVal);
   String message = "";
   message = "Raining?: " + rainingBT + "\t Moisture Level: " + nRainValBT + "\n" + "^";
   BTSerial.print(message);
}

void loop()
{ 
   if (BTSerial.available())
   {  
        char c = BTSerial.read();
        if (c == 'L')
        {
           //for light status
           BTSerial.write("ACK Slave Recieved: L CMD\n#"); 
           String message = "";
           if(digitalRead(LED_BUILTIN) == HIGH)
           {
              message = "Downstairs lights are ON\n";
           }
           else
           {
              message = "Downstairs lights are OFF\n";
           }
           if(state_light == HIGH)
           {
              message += "Upstairs lights are ON\n^";
           }
           else
           {
              message += "Upstairs lights are OFF\n^";
           }
           BTSerial.print(message); 
        }
        else if (c == 'B')
        {
           //for photocell (brightness)
           BTSerial.write("ACK Slave Recieved: B CMD\n#"); 
           int photodata = getPhotocell();
           printPhoto(photodata);
        }
        else if (c == 'T')
        {
           //for temp sensor
           BTSerial.write("ACK Slave Recieved: T CMD\n#"); 
           float temp = getTemp();
           printTemp(temp);
        }
        else if (c == 'R')
        { 
           //for rain sensor
           BTSerial.write("ACK Slave Recieved: R CMD\n#");
           detectRain();
        }
        else if (c == 'D')
        {
           //for door sensor
           BTSerial.write("ACK Slave Recieved: D CMD\n#");
           String doorstate=detectDoorMotion();
           printdetectDoorMotion(doorstate);
        }
        else if (c == 'M')
        {
           //for motion sensor
           BTSerial.write("ACK Slave Recieved: M CMD\n#");
           String motion = detectMotion();
           printdetectMotion(motion);
        }
        else if (c == 'A')
        {
          String message = "";
          //to set alarm
          BTSerial.write("ACK Slave Recieved: A CMD\n#");
          setAlarm=!setAlarm;
          String setAlarmST = String(setAlarm);
          message = "ALARM SET = " + setAlarmST + "\n" + "^";
          BTSerial.print(message);
        }
   }

   // leave led on as long as motion was detected 
   val = digitalRead(inputPin); 
   if(val==HIGH)
      digitalWrite(LED_BUILTIN, HIGH);
   else
      digitalWrite(LED_BUILTIN, LOW);

   //ring door bell
   buttonState = digitalRead(buttonPin);
   if (buttonState == HIGH) {
     tone(buzzer, 400);
   } else {
     noTone(buzzer);
  }

  //button light switch 
   buttonState1 = digitalRead(buttonPin1);
 
   
   if (buttonState1 == HIGH && previous == LOW && millis() - time > debounce) {
        if (state_light == HIGH)
           state_light = LOW;        
        else
           state_light = HIGH;
        time = millis();    
   }
   if(state_light == HIGH)
   {
      analogWrite(ledPin, 100-adjustedLightLevel);
   }
   else
   {
      digitalWrite(ledPin, LOW);
   }
   //digitalWrite(ledPin, state_light);
   previous = buttonState1;

   //for alarm 
   String curdoorstate=detectDoorMotion();
   if (curdoorstate=="Door is open\n" && setAlarm){
      tone(buzzer, 400);
   }

}
