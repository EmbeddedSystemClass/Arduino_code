//long b = 115200;
long b = 230400;
//long b = 345600;
//long b = 460800;
//long b = 2000000;

long count = 0;
long time = millis();

void setup() 
{
  Serial.begin(b);
  delay(100);
  Serial.print("Serial communication establised at ");
  Serial.print(b);
  Serial.println(" baud.");
  
}

void loop() 
{
  /*if (count==10000)
  {
    Serial.print(millis()-time);
    delay(100);
    Serial.end();
  }
  Serial.print(count);
  Serial.print('\n');
  for (int i=0; i<16; i++)
  {
    Serial.print(i);
  }
  Serial.print('\n');
  count++;*/
  long time = millis();
  byte t = B10010011;
  for (int i=0; i<10000; i++)
  {
    Serial.write((byte)t);
  }
  time = millis() - time;
  Serial.println("");
  Serial.println(time);
  delay(5000);
}
