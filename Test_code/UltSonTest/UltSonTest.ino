bool flag=0;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() 
{
  while (Serial.available())
  {
    flag=1;
    Serial.println(Serial.read());
    delay(10);
  }
  if (flag==1)
  {
    flag=0;
    Serial.println("");
  }
  else
  {
    Serial.print("");
  }
}
