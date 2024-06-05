/*
 * PID_Controls.h
 *
 *  Created on: Jun 2, 2024
 *      Author: jacka
 */
#include "PID.h"

static pid_datatype integral_sum = 0;   // Riemann sum for integrating
static pid_datatype prev_err = 0;       // Previous error (for derivative)
static float delta_t = 1/SAMPLE_RATE;          // Time between samples
static float current_time = 0;          // Time passed

pid_datatype integrate_error(pid_datatype error){
    integral_sum += error;
    return integral_sum;
}

pid_datatype diff_error(pid_datatype error){
    return (pid_datatype)((error-prev_err)/delta_t);
}

pid_datatype pid_output(pid_datatype Kp, pid_datatype Ki, pid_datatype Kd, pid_in_datatype input){
    pid_datatype error = calculate_error(input);
    current_time += delta_t;
    return Kp*error + Ki*integrate_error(error) + Kd*diff_error(error);
}

pid_datatype calculate_error(pid_in_datatype input){
}
