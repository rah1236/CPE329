/*
 * PID_Controls.h
 *
 *  Created on: Jun 2, 2024
 *      Author: jacka
 */
#include "PID.h"
#include "tim2_timer.h"

static pid_datatype integral_sum = 0;   // Riemann sum for integrating
static pid_datatype prev_err = 0;       // Previous error (for derivative)
static float delta_t = 1/SAMPLE_RATE;          // Time between samples

struct pid_setpoint{
    int sp_time;
    pid_datatype value;
};

pid_datatype integrate_error(pid_datatype error){
    integral_sum += error;
    return integral_sum;
}

pid_datatype diff_error(pid_datatype error){
    return (pid_datatype)((error-prev_err)/delta_t);
}

pid_datatype pid_output(pid_datatype Kp, pid_datatype Ki, pid_datatype Kd){
    pid_datatype error = calculate_error();
    pid_datatype response = Kp*error + Ki*integrate_error(error) + Kd*diff_error(error);
    if (response < 0){
        return 0;
    }else if (response > 100){
        return 100;
    }else{
        return response;
    }
}

pid_datatype calculate_error(pid_in_datatype input){
    int sec_passed = get_sec_passed();
    return 50 - input; // TODO: change to track setpoint (not fixed)
}

