/* PID Controler Implementation in C
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
   
*/

#include "PID.h"

void PIDController_Init(PIDController *pid) {

	/* Clear controller variables */
	pid->integrator = 0.0f;
	pid->prevError  = 0.0f;

	pid->differentiator  = 0.0f;
	pid->prevMeasurement = 0.0f;

	pid->out = 0.0f;

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
    pid->integrator = pid->integrator + 0.5f * pid->Ki * pid->T * (error + pid->prevError);

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
    pid->differentiator = -(2.0f * pid->Kd * (measurement - pid->prevMeasurement)
                        + (2.0f * pid->tau - pid->T) * pid->differentiator)
                        / (2.0f * pid->tau + pid->T);


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