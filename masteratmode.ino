//  Pins
//  BT VCC to Arduino 5V out. 
//  BT GND to GND
//  BT RX to Arduino pin 11 (through a voltage divider)
//  BT TX to Arduino pin 10 (no need voltage divider)
#include <SoftwareSerial.h>
SoftwareSerial BTserial(11, 10); // RX | TX
 
char c = ' ';

//counts only valid commands
int terminalToMasterCount = 0;
int masterToTerminalCount = 0; // this is for the setup
int masterToSlave = 0;
int slaveToMaster = 0;

void printCommandList()
{
    masterToTerminalCount += 8;
    Serial.println("Enter the CMD for the sensor or status you would like to see");
    Serial.println("L: Lights status");
    Serial.println("B: Brightness inside of house (0-100)");
    Serial.println("T: Temparature inside house");
    Serial.println("R: Raining outside?");
    Serial.println("D: Door status");
    Serial.println("M: Motion inside house?");
    Serial.println("A: Alarm Set. (ON=1/OFF=0)\n");
}

void printTotalNumbers()
{
    masterToTerminalCount += 10;
    Serial.println();
    Serial.print("Total number of messages sent from terminal to master: "); Serial.println(terminalToMasterCount);
    Serial.print("Total number of messages sent from master to terminal: "); Serial.println(masterToTerminalCount);
    Serial.print("Total number of messages sent from master to slave: "); Serial.println(masterToSlave);
    Serial.print("Total number of messages sent from slave to master: "); Serial.println(slaveToMaster);
    Serial.println();
}

void setup() 
{
    // start the serial communication with the host computer
    Serial.begin(9600);
    // start communication with the HC-05 using 9600
    BTserial.begin(9600);  
    Serial.println("Welcome to our smarthome");
    masterToTerminalCount += 1;
    printCommandList();
}
 
void loop()
{
     // Keep reading from HC-05 and send to Arduino Serial Monitor
    if (BTserial.available())
    {   
        String ack = BTserial.readStringUntil('#');
        String message = BTserial.readStringUntil('^');
        slaveToMaster += 2;
        masterToTerminalCount += 3;
        Serial.print(ack);
        Serial.print(message); 
        Serial.println("ACK Master Recieved DATA\n");  
        printTotalNumbers();
        printCommandList();
    }
 
    // Read from Arduino Serial Monitor and send to HC-06
    if (Serial.available())
    {
        c =  Serial.read();  
        if (c == 'L' || c == 'B'|| c == 'T' || c == 'R' || c == 'D' || c == 'M' || c == 'A')
        { 
           masterToTerminalCount += 3;
           terminalToMasterCount++;
           Serial.print("ACK Master recieved :"); Serial.print(c); Serial.println(" CMD. Sending to slave..");
           masterToSlave++;
           BTserial.write(c);  
        }
    }

 
}
