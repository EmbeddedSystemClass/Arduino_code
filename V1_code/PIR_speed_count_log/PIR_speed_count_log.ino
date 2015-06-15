/*
PIR average speed and vehicle counter
***for vehicle counter function ONLY, change speed_func boolean to 'false'

Full code by: William Barbour
Portions taken as inspiration from example code provided by Arduino IDE (public domain)
Modified: 06/09/2015

V1 (06/05/2015): 
  solely PIR implentation, edge triggers marked time between interval ends
V2 (06/09/2015): 
  implented data logging to SD functionality, ability to switch between counting and speed/count

*/

// BOARD: ARDUINO UNO

// Connections:
// 5V -> 5V
// GND -> GND
// D13 -> CLK
// D12 -> DO
// D11 -> DI
// D10 -> CS

#include <SPI.h>
#include <SD.h>

const int cs = 10; //chip select pin (D10 on Arduino UNO)
boolean speed_func = true;
File dataFile; //global File instance since loop will execute multiple times while File is open

const int pir1 = 8;
const int pir2 = 9;
float distance = 4.0; //distance in meters between PIR sensors
long timeout = 3000; //timeout in milliseconds for the second PIR sensor to wait for motion trigger
float maxspeed = 50; //maximum expected speed for rejection of extraneous values
unsigned long motionCount = 0;
boolean timeout_flag = false;
unsigned int timeoutCount = 0;

unsigned long logstart = millis();
int filecounter = 1;
unsigned long loginterval = 300000;

void setup() 
{
  //initialize serial communication
  Serial.begin(115200);
  
  //setup pins for PIR communication
  Serial.print("Initialize PIR sensors...");
  pinMode(pir1, INPUT);
  pinMode(pir2, INPUT);
  Serial.println("COMPLETE");
  
  //initialize communication with SD card on pin 'cs'
  Serial.print("Initialize SD card...");
  pinMode(cs, OUTPUT);
  if (!SD.begin(cs))
  {
    Serial.println("FAILED");
    return;
  }
  Serial.println("COMPLETE");
  
  //create first file to write
  //incrementing of file depends on running millisecond counter,
  //which has not run long enough to create file in loop()
  String file_name1 = "PIR";
  
  for (int i=1; i<1000; i++)
  {
    String teststring = file_name1;
    teststring += i;
    if (!SD.exists(teststring))
    {
      file_name1 += i;
      filecounter = i;
      break;
    }
    Serial.print("File ");
    Serial.print(teststring);
    Serial.println(" exists");
  }
  
  file_name1 += ".txt";
  //filenames for SD library must comply with 8.3 filename convention
  dataFile = SD.open(file_name3, FILE_WRITE);
  Serial.print("File created for writing: ");
  Serial.println(dataFile.name());
  filecounter++;
}





void loop() 
{
  if((millis() - logstart) > loginterval)
  {
    if (dataFile)
    {
      dataFile.println("Log summary...");
      dataFile.print("Running vehicle count: ");
      dataFile.println(motionCount);
      dataFile.print("Running timeout count: ");
      dataFile.println(timeoutCount);
      Serial.print("Closing file: ");
      Serial.prinln(dataFile.name());
      dataFile.close();
    }
    else
    {
      Serial.println("Error: no data file open");
    }
    
    String file_name1 = "PIR";
    file_name1 += filecounter;
    file_name1 += ".txt";
    //file names for SD library must comply with 8.3 filename convention
    dataFile = SD.open(file_name1, FILE_WRITE);
    Serial.print("File created for writing: ");
    Serial.println(dataFile.name());
    filecounter++;
    logstart = millis();
  }
  
  while (digitalRead(pir1) == LOW)
  {
    delay(5);
    Serial.print(".");
  }
  Serial.println("");
  
  //capture time when PIR 1 triggered and print to serial monitor
  unsigned long time = millis();
  Serial.println(time);
  //increment vehicle count
  motionCount++;
  
  //compose motion trigger data string and vehicle count number
  String trigdata = "Motion detected on PIR1: ";
  trigdata += time;
  trigdata += " Vehicle #";
  trigdata += motionCount;
  //write motion trigger data string to file
  if (dataFile)
  {
    dataFile.println(trigdata);
  }
  else
  {
    Serial.println("No data file open.");
  }
  
  //executes if speed logging enabled via 'speed_func' global boolean
  if (speed_func)
  {
    //wait for trigger on PIR 2
    while (digitalRead(pir2) == LOW)
    {
      if (millis() > time + timeout)
      {
        timeout_flag = true;
        break;
      }
      delay(5);
      Serial.print(",");
    }
    Serial.println("");
    
    if (!timeout_flag);
    {
      unsigned long time2 = millis();
      unsigned long diff = time2 - time;
      Serial.println(time2);
      Serial.print("Time differential: ");
      Serial.println(diff);
      
      //compose motion trigger data string
      trigdata = "Motion detected on PIR2: ";
      trigdata += time2;
      //compose speed data string
      String speeddata = "Speed clocked at: ";
      unsigned float m_s = (diff/1000)/distance;
      unsigned float mph = m_s * 2.23694;
      speeddata += m_s;
      speeddata += " m/s; ";
      speeddata += mph;
      speeddata += " mph";
      Serial.println(speeddata);
      if (dataFile)
      {
        dataFile.println(trigdata);
        dataFile.println(speeddata);
      }
      else
      {
        Serial.println("No data file open");
      }
    }
    else
    {
      timeoutCount++;
      Serial.println("Timeout on PIR 2");
      Serial.print(timeout);
      Serial.println(" milliseconds exceeded");
      if (dataFile)
      {
        dataFile.println("Timeout on PIR 2");
      }
      else
      {
        Serial.println("No data file open");
      }
    }
  }
  
}
