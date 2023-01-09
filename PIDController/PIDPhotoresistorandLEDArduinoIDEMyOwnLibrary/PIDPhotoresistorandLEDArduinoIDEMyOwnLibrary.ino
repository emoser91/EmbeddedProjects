/* PID Controller
  
   PID Controler Implementation in C
   This code is based off of a Youtube Tutorial:
   https://www.youtube.com/watch?v=zOByx3Izf5U&list=PLujzZZC66w_pKogpC4tPced23JakRdmF6&index=2
   Authored by Phil's Lab. It goes into great detail of the implemenation of a PID Controller
   in C and some ways to improve on the basic design. 
   
   PID explaination:
   You have a setpoint that you are trying to reach and you have measurement data that you are taking
   to check to see how close to the setpoint you are. You will calculate the error between the setpoint
   and meaurement and then feed that to the PID and it will output a control signal. The PID controller
   consists of three separate branches that each does thier own calculation and then the three are summed
   for your control signal value. The branches consist of position, integral and derivative. Each branch
   has a gain value which can be changed to "Tune" the PID for better operation in the real world.
   Watch the above mentioned youtube video for a further explaination of the interworkings of the PID
   Controller.
   
   The PID is implemented in C by converting the S-domain function into the z-domain
   with the help of the Tustin transform (best frequency domain match). The formulas
   are shown below. 
   
   S-domain/Continous Domain PID formula:
   G_pid = u(s) / e(s) = K_p + (K_I * (1/s)) + (K_D * (s/(s * TAU +1)))
   
   Z-domain/Descrete Domain PID terms after simplification:
   p[n] = k_p * e[n]
   
   i[n] = ((k_I * T) / 2 ) * (e[n] + e[n-1]) + i[n-1]
   
   d[n] = ((2k_D) / ((2 * TAU) + T) ) * (e[n] - e[n-1]) + ((2 * TAU - T) / ((2 * TAU) + T) ) * d[n-1]
   
   controller output = u[n] = p[n] + i[n] + d[n]
   
   Pratical Considerations:
   1. Derivative amplifies High Frequency Noise
   Solution: Add Derivative Low Pass Filter
    Problem: Adding a Low Pass Filter will deminish the power of the derivative control.
    Becuase derivative control is hard to setup, it is normally not used in most applications.
    Most will just use a PI controller. 
   2. Derivative "Kick" during setpoint change
        The derivative of a step change in the setpoint is going to be a impluse/spike
        Solution: Derivative-on-measurement: instead of taking derivative of error, use the feed back measurement
    The end result will be the same but without the change for a kick
   3. Integrator can saturate output
      Solution: Integrator anti-windup: Basically clamping
   4. Limits on system input amplitude
        Solution: Clamp controller output: The controller output needs to be constrained 
    The system has maximum and minumum outputs possible that the system needs to be set to
   5. Choosing a sample time T. How many times do we update the controller output?
        Solution: T_controller < ((T_Bw, system) / 10 )
    Controller have a 10 times larger bandwidth then that of the system it is controlling

  
   Program Code Structure:
   1. One header file for the pid (PID.h)
   2. PID Controller Stuct to store all the PID (contains gains, storage variables, ...)
   3. Initialisaton Function (Set gains, sample time, ...)
   4. Update function(provide setpoint and measurement, returns controller output)
  
  You can use Tool/Serial Plotter to see the graph plotting of the Photoresistor Reading to see the PID working
  Make sure to aim the Photoresistor at the LED
  Place a card between the Photoresistor and LED then remove it to see how the PID recovers back to the setpoint
  
  You can also use the Serial Monitor to see the values to help select a setpoint for your system to target
  
  Try to retune the PID and see how the process is done. Use the Serial Plotter to see whats going on. 
  
  Wiring:
  Output to the LED is PD5:
  PD5/Digital5 to LED to a 220ohm resistor to GND
  
  Input for Photoresistor is PC0:
  5V to photoresistor to BOTH PC0 and to a 1kohm resistor to GND (Voltage Devider Setup)
  
  NOTE: I had to modify how I would normally write this program in order to handle the Arduino Sketch System
  Arduino IDE had issues with having "f" after all float values. Normally you would want to have f at the end
  of every float value like 0.1f per example. Also I had to declare the struct object outside of the "setup" function
  so that I could use it in the "loop" function. Normally you would just have main and while(1) and the object 
  could be declared in main. Also breaking down the PID code into a PID.h and PID.c file would be a good idea. 
  
  I did not do any tuning on this yet and just copied over the gains from the other Arduino Example I have done. 
 */

/*How to Tune a PID:
 * 1. Set all gains to zero
 * 2. Increase the P gain until the responseto a disturbance is steady oscillation
 * 3. Increase the D gain until the oscillations go away (ie. its critically damped)
 * 4. Repeat steps 2 and 3 until increasing the D gain does not stop the oscillations
 * 5. Set P and D to the last stable values
 * 6. Increase the I gain until it brings you to the setpoint with the number of oscillations you desire
 * (normally 0 but a quicker response can be had if you dont mind a couple of osciallations of overshoot)
 */
 
/* Controller parameters */
#define PID_KP  0.1
#define PID_KI  10
#define PID_KD  0.01

#define PID_TAU 0.02

#define PID_LIM_MIN  0
#define PID_LIM_MAX  255

#define PID_LIM_MIN_INT  0
#define PID_LIM_MAX_INT  255

#define SAMPLE_TIME_S 0.01

/* What would be in the PID.h file*/
typedef struct {

  /* Controller gains */
  float Kp;
  float Ki;
  float Kd;

  /* Derivative low-pass filter time constant */
  float tau;

  /* Output limits */
  float limMin;
  float limMax;
  
  /* Integrator limits for anti-winding clamping*/
  float limMinInt;
  float limMaxInt;

  /* Sample time (in seconds) */
  float T;

  /* Controller "memory" */
  float integrator;
  float prevError;      /* Required for integrator */
  float differentiator;
  float prevMeasurement;    /* Required for differentiator */

  /* Controller output */
  float out;

} PIDController;

void  PIDController_Init(PIDController *pid);
float PIDController_Update(PIDController *pid, float setpoint, float measurement);

/* What would be in a PID.c file*/
//#include "PID.h"

void PIDController_Init(PIDController *pid) {

  /* Clear controller variables */
  pid->integrator = 0.0;
  pid->prevError  = 0.0;

  pid->differentiator  = 0.0;
  pid->prevMeasurement = 0.0;

  pid->out = 0.0;

}

float PIDController_Update(PIDController *pid, float setpoint, float measurement) {

  /*
  * Error signal
  */
    float error = setpoint - measurement;


  /*
  * Proportional
  */
    float proportional = pid->Kp * error;


  /*
  * Integral
  */
    pid->integrator = pid->integrator + 0.5 * pid->Ki * pid->T * (error + pid->prevError);

  /* Anti-wind-up via integrator clamping*/
    if (pid->integrator > pid->limMaxInt)
    {
        pid->integrator = pid->limMaxInt;
    }
    else if (pid->integrator < pid->limMinInt)
    {
        pid->integrator = pid->limMinInt;
    }


  /*
  * Derivative (band-limited differentiator)
  */
  /* Note: derivative on measurement, therefore minus sign in front of equation! */ 
    pid->differentiator = -(2.0 * pid->Kd * (measurement - pid->prevMeasurement)
                        + (2.0 * pid->tau - pid->T) * pid->differentiator)
                        / (2.0 * pid->tau + pid->T);


  /*
  * Compute output and apply limits
  */
    pid->out = proportional + pid->integrator + pid->differentiator;

  /* Limit the output based on the limits we want */
    if (pid->out > pid->limMax)
    {
        pid->out = pid->limMax;
    }
    else if (pid->out < pid->limMin)
    {
        pid->out = pid->limMin;
    }


  /* Store error and measurement for later use */
    pid->prevError       = error;
    pid->prevMeasurement = measurement;

  /* Return controller output */
    return pid->out;

}




/* IO Setup*/
#define photocellPin 0
#define ledPin 5

double setpoint; //PID Setpoint
double measurement; //Input from photocell
double ledBrightness; //Output to LED

/* Initialise PID controller */
  PIDController pid = { PID_KP, PID_KI, PID_KD,
                        PID_TAU,
                        PID_LIM_MIN, PID_LIM_MAX,
                        PID_LIM_MIN_INT, PID_LIM_MAX_INT,
                        SAMPLE_TIME_S };
                        
void setup()
{
  Serial.begin(9600);

  PIDController_Init(&pid);
    
  setpoint = 200; //Must be higher then regular room lighting with LED off and lower then LED at max value of 255 (analog read max is 1023)
  
  delay(1000); //Just a delay to give time to open a serial plotter and see it working

}

void loop()
{
  measurement = analogRead(photocellPin); //Analog Read of PC0

  ledBrightness = 255;
  /* Compute new control signal */
  PIDController_Update(&pid, setpoint, measurement);
  ledBrightness = pid.out;

  analogWrite(ledPin, ledBrightness); //Analog output to set LED Brightness

  Serial.println(measurement); //Print photcell reading values to serial output so we can watch the PID working

//  Serial.println(ledBrightness); //Can be used to check the brightness being commanded (255 is max)
  
}
