int encoder_pin = 18; // pulse output from the module
unsigned int rpm = 0; // rpm reading
volatile byte pulses = 0; // number of pulses
unsigned long timeold = 0;
// number of pulses per revolution
// based on your encoder disc
unsigned int pulsesperturn = 100;
void counter()
{
   //Update count
   pulses++;
}
void setup()
{
   Serial.begin(9600);
   pinMode(encoder_pin, INPUT);
   //Interrupt 0 is digital pin 18
   //Triggers on Falling Edge (change from HIGH to LOW)
   attachInterrupt(5, counter, FALLING);
}
void speedsense(){
   if (millis() - timeold >= 1000) {
      //Don't process interrupts during calculations
      detachInterrupt(5);
      rpm = (60 * 1000 / pulsesperturn )/ (millis() - timeold)* pulses;
      timeold = millis();
      pulses = 0;
      Serial.print("RPM = ");
      Serial.println(rpm,DEC);
      //Restart the interrupt processing
      attachInterrupt(5, counter, FALLING);
   }
}

