int pir1 = 8;
int pir2 = 9;

int distance = 4; //distance in meters between PIR sensors

long timeout = 5000;
int maxspeed = 50;

void setup() 
{
  pinMode(pir1, INPUT);
  pinMode(pir2, INPUT);
  Serial.begin(115200);
}

void loop() 
{
  while (digitalRead(pir1) == LOW)
  {
    delay(10); //slow loop down
    Serial.print(".");
  }
  
  long time = millis();
  Serial.print(time);
  
  while (digitalRead(pir2) == LOW)
  {
    delay(10);
    Serial.print("!");
  }
  
  time = millis() - time;
  Serial.print(time);
  
}
