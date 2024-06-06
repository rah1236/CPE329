/*
 * PID.h
 *
 *  Created on: Jun 2, 2024
 *      Author: jacka
 */
#include "stm32l4xx_hal.h"

#ifndef INC_PID_H_
#define INC_PID_H_

#define SAMPLE_RATE 1   // Samples/s

typedef int pid_datatype;   // Numeric data type for PID
typedef uint32_t pid_in_datatype;// Typedef for input to PID

pid_datatype integrate_error(pid_datatype error);
pid_datatype diff_error(pid_datatype error);
pid_datatype pid_output(pid_datatype Kp, pid_datatype Ki, pid_datatype Kd);
pid_datatype calculate_error(pid_in_datatype input);
void set_input(pid_in_datatype input);

#endif /* INC_PID_H_ */
