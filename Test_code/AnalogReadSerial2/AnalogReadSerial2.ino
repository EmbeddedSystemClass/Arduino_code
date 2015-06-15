/*
  AnalogReadSerial
  Reads an analog input on pin 0, prints the result to the serial monitor.
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.
 
 This example code is in the public domain.
 */
double sensorValue;
// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  sensorValue = 0;
}

// the loop routine runs over and over again forever:
void loop() {
  // read the input on analog pin 0:
  sensorValue = analogRead(A0);
  //convert 0-1023 scale to m/s
  //anemometer outputs 0.4V = 0m/s, 2V = 32.4m/s
  sensorValue = sensorValue / 205;
  //sensorValue = (sensorValue - 82) / 10.123;
  // print out the value you read:
  Serial.println(sensorValue);
  delay(5);        // delay in between reads for stability
}
