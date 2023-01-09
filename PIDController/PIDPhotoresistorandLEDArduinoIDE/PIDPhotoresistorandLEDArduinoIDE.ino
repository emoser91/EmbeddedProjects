/* PID Controller
 * 
 * This Project consists of a PID Controller that I simply downloaded from the Arduino Manage Libraries feature.
 * The Project will set out to control a LED Brightness using a Photoresistor
 * This ia a closed loop control
 * 
 * You can use Tool/Serial Plotter to see the graph plotting of the Photoresistor Reading to see the PID working
 * Make sure to aim the Photoresistor at the LED
 * Place a card between the Photoresistor and LED then remove it to see how the PID recovers back to the setpoint
 * 
 * You can also use the Serial Monitor to see the values to help select a setpoint for your system to target
 * 
 * Try to retune the PID and see how the process is done. Use the Serial Plotter to see whats going on. 
 * 
 * Wiring:
 * Output to the LED is PD5:
 * PD5/Digital5 to LED to a 220ohm resistor to GND
 * 
 * Input for Photoresistor is PC0:
 * 5V to photoresistor to BOTH PC0 and to a 1kohm resistor to GND (Voltage Devider Setup)
 * 
 * 
 * I used the following Youtube Video for this project: 
 * https://www.youtube.com/watch?v=H-ENMrhOviA&list=PLujzZZC66w_pKogpC4tPced23JakRdmF6&index=20&t=358s
 * I followed the Youtube video for this project and it is not an example code from the library.
 * Github of the Library used: https://github.com/br3ttb/Arduino-PID-Library
 */
#include <PID_v1.h> //PID Library: PID by Brett Beauregard

#define photocellPin 0
#define ledPin 5

double Setpoint; //PID Setpoint
double photocellReading; //Input from photocell
double ledBrightness; //Output to LED

//defining PID tuning varriables
/*How to Tune a PID:
 * 1. Set all gains to zero
 * 2. Increase the P gain until the responseto a disturbance is steady oscillation
 * 3. Increase the D gain until the oscillations go away (ie. its critically damped)
 * 4. Repeat steps 2 and 3 until increasing the D gain does not stop the oscillations
 * 5. Set P and D to the last stable values
 * 6. Increase the I gain until it brings you to the setpoint with the number of oscillations you desire
 * (normally 0 but a quicker response can be had if you dont mind a couple of osciallations of overshoot)
 */
double kp = .1; //proportional gain
double ki = 10; //derivative gain
double kd = .01; //integral gain

//Inital setup required by the included PID library
PID myPID(&photocellReading, &ledBrightness, &Setpoint, kp, ki, kd, DIRECT);

void setup()
{
  Serial.begin(9600);

  Setpoint = 200; //Must be higher then regular room lighting with LED off and lower then LED at max value of 255 (analog read max is 1023)

  //Turn on PID
  myPID.SetMode(AUTOMATIC);
  myPID.SetTunings(kp,ki,kd);
  
  delay(1000); //Just a delay to give time to open a serial plotter and see it working

}

void loop()
{
  photocellReading = analogRead(photocellPin); //Analog Read of PC0

  myPID.Compute(); //Compute output of PID for its response

  analogWrite(ledPin, ledBrightness); //Analog output to set LED Brightness

  Serial.println(photocellReading); //Print photcell reading values to serial output so we can watch the PID working

//  Serial.println(ledBrightness); //Can be used to check the brightness being commanded (255 is max)
  
}
