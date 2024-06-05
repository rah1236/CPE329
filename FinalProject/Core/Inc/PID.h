/*
 * PID.h
 *
 *  Created on: Jun 2, 2024
 *      Author: jacka
 */

#ifndef INC_PID_H_
#define INC_PID_H_

#define SAMPLE_RATE 1   // Samples/s

typedef int pid_datatype;   // Numeric data type for PID
typedef int pid_in_datatype;// Typedef for input to PID

pid_datatype integrate_error(pid_datatype error);
pid_datatype diff_error(pid_datatype error);
int pid_output(pid_datatype Kp, pid_datatype Ki, pid_datatype Kd);

#endif /* INC_PID_H_ */
