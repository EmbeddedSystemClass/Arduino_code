/* PWM charge controller Arduino code

Modified: William Barbour
          University of Illinois at Urbana-Champaign
          06/07/2015

Original inspiration code: Dabasish Dutta
                            08/04/2014

This code is public domain.
 */

float solar_volt = 0;    // variable for solar panel voltage
float bat_volt = 0;    // variable for battery voltage
float solar_read = 0;     //solar voltage reading (analog A0)
float bat_read = 0;     //battery voltage reading (analog A1)
int pwm = 6;        // PWM output pin to solar disconnect transisotr
int load = 9;        //MOSFET load driver pin
int RED = 7;           // To indicate discharged condition of battery
int GREEN = 8;         // for charging and battery fully charged
int warning = 4;    //pin for connection to primary microcontroller, low battery warning
                    //set up for active HIGH

int v_num = 30;    //# of analog readings to make and average for voltage
float solar_R_ratio = (15.0+4.7)/4.7;  //resistor ratio on voltage divider for solar voltage
float bat_R_ratio = (4.7+4.7)/4.7;  //resistor ratio on voltage divider for battery voltage
float analog_scaler = (5.0/1024.0);  //volts per division on analogRead()

//battery charge curve values
//separated into 3 charge stages for PWM control
float bat_discharged = 6.200; //absolute cutoff to avoid damage to battery, load disconnected at this point
float bat_warning = 6.400; //optional implementation to warn connected controllers of low battery voltage

float bat_stage1 = 6.500; //bulk charge cuts off after stage 1 limit
float pwm_stage1 = 191.250; //75%

float bat_stage2 = 6.600; //PWM charge rate decreases to trickle for stage 2 until full charge achieved
float pwm_stage2 = 127.500;  //50%

float pwm_final = 25.5;  //10%
float bat_full = 6.8; //absolute maximum cutoff to avoid damage to battery, solar panels disconnected
boolean full_flag = false; //flag sets to true when full charge is reached, pwm = 0 until 6.8V

void setup()
{
  TCCR0B = TCCR0B & 0b11111000 | 0x05; // setting prescaler for 61.03Hz pwm
  Serial.begin(9600);
  Serial.println("Serial communication established");
  pinMode(pwm,OUTPUT);
  pinMode(load,OUTPUT);
  pinMode(RED,OUTPUT);
  pinMode(GREEN,OUTPUT);
  digitalWrite(pwm,LOW);
  digitalWrite(load,LOW);
  digitalWrite(RED,LOW);
  digitalWrite(RED,LOW);
}
void loop()
{
  ///////////////////////////  VOLTAGE SENSING ////////////////////////////////////////////
  for(int i=0;i<v_num;i++)
  {
    solar_read += analogRead(A0);  //read the input voltage from solar panel
    //Serial.println(solar_read);
    bat_read += analogRead(A1);  //read the battery voltage
    //Serial.println(bat_read);
    Serial.print(i);
    delay(20);
  }
  Serial.println("");
  
  solar_read = solar_read/v_num; 
  //Serial.println(solar_read);
  bat_read = bat_read/v_num; 
  //Serial.println(bat_read);
   
  //Serial.println(analog_scaler,4);
  //Serial.println(solar_R_ratio,4);
  
  solar_read = (solar_read * analog_scaler * solar_R_ratio); //convert analog reading to actual solar voltage
  bat_read = (bat_read * analog_scaler * bat_R_ratio);  //convert analog reading to actual battery voltage
  solar_volt = solar_read;
  bat_volt = bat_read;
  Serial.print("Solar input voltage: ");
  Serial.println(solar_volt);
  Serial.print("Battery voltage: ");
  Serial.println(bat_volt);
  
   
  // ///////////////////////////PWM BASED CHARGING ////////////////////////////////////////////////
  // As battery is gradually charged the charge rate (pwm duty) is decreased
  // 6.8v = fully charged(100%) 
  // 6.2v =fully discharged(0%)
  // when battery voltage is less than 6.4v, give you alart by glowing RED LED and displaying "DISCHARGED..."
  
  digitalWrite(warning, LOW); //reset auzilary low battery warning pin
  Serial.println(less_than_3dec(bat_volt, bat_discharged));
  if(less_than_3dec(bat_volt, bat_discharged))
  {
    digitalWrite(load, HIGH);
    Serial.println("Battery critically low. Load disconnected.");
  }
  else
  {
    digitalWrite(load, LOW);
  }
  
  if(less_than_3dec(solar_volt, bat_volt))
  {
    analogWrite(pwm, 0);
    Serial.println("Low solar voltage. PWM = 0%");
  }
  else
  {
    if(less_than_3dec(bat_volt, bat_stage1))
    {
      analogWrite(pwm, pwm_stage1); // set PWM value for stage 1 charging
      Serial.print("PWM duty cycle is :");
      Serial.print(pwm_stage1/2.55);
      Serial.println("%");
      
      if(less_than_3dec(bat_volt, bat_warning))
      {
        digitalWrite(warning, HIGH);
        Serial.println("WARNING: Battery voltage approaching full discharge");
        Serial.print("Battery voltage: ");
        Serial.print(bat_volt);
        Serial.println(" volts");
        Serial.print("Load disconnection will occur at ");
        Serial.print(bat_discharged);
        Serial.println(" volts");
      }
      
    }
  
    else if(less_than_3dec(bat_volt, bat_stage2))
    {
      analogWrite(pwm, pwm_stage2);  //set PWM value for stage 2 charging 
      Serial.print("PWM duty cycle is :");
      Serial.print(pwm_stage2/2.55);
      Serial.println("%");
      
      if(full_flag == true)
      {
        full_flag = false;
        Serial.println("Battery discharge cycling complete. Recommence charging.");
      }
    }
    
    else if(less_than_3dec(bat_volt, bat_full))
    {
      
      if (full_flag == true)
      {
        analogWrite(pwm, 0);
        Serial.println("Discharging from full for battery cycling");
      }
      else
      {
        analogWrite(pwm, pwm_final); 
        Serial.print("PWM duty cycle is :");
        Serial.print(pwm_stage1/2.55);
        Serial.println("%");
      }
    }
    
    else if(!less_than_3dec(bat_volt, bat_full))
    {
      analogWrite(pwm, 0);
      Serial.println("Battery full. Charging disconnected.");
      full_flag = true;
    }
  }
  
  //////////////////////////////LED INDICATION DURING CHARGING/////////////////////////////
  
  if (solar_volt > bat_volt && bat_volt < bat_full)
  {
    ///Green LED will blink continiously indicating charging is going on
    digitalWrite(GREEN, HIGH);
    delay(1);
    digitalWrite(GREEN, LOW);
    delay(1);
  }
  // Red LED will glow when battery is discharged
  if (bat_volt < bat_warning)
  {
    digitalWrite(RED, HIGH); // indicating battery is discharged
  }
   // Red LED will OFF when battery is not discharged
  else
  {
    digitalWrite(RED, LOW);
  }
  //Green LED will glow when battery is fully charged
  if(bat_volt >= bat_stage2 && full_flag == true) 
  {
    digitalWrite(GREEN, HIGH);
  }
}

boolean less_than_3dec(float a, float b)
{
  long a_t = a * 1000;
  long b_t = b * 1000;
  return a_t < b_t;
}
