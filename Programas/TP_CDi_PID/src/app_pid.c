/*
 * app_pid.c
 *
 *  Created on: Apr 16, 2023
 *      Author: wels
 */
#include "app_pid.h"

/*  Kp = 1.64, Ki = 561, Kd = 0.000672 */

#define PID_PARAM_KP        1.64            /* Proporcional */
#define PID_PARAM_KI        561        		/* Integral */
#define PID_PARAM_KD        0.000672        /* Derivative */

//#define COUNT_CYCLES

float32_t lim_max = 1;
float32_t lim_min = 0;
uint32_t lim_size = 1;
// Declaración de variables globales
float32_t error, output;
float32_t set_point;
arm_pid_instance_f32 PID;
static uint16_t dacValue = 0;
static uint32_t dac_convert = 0;
static uint32_t sample_2 = 0;

//void console_float (float data,float data2);

float32_t read_sensor(){
	float32_t value = 0;
	sample_2 = adcRead(CH1);

	value = (float)sample_2 / 1023.0;
	return value;
}

void write_actuator(float32_t output){
	dacValue = output * 0xFFFF;
	dac_convert = output*1023;
	dacWrite(DAC, dacValue);;
}

static void task_PID(void *taskParmPtr)
{
	/* ARM PID Instance, float_32 format */
	arm_pid_instance_f32 PID;
	/* Set PID parameters */
	PID.Kp = PID_PARAM_KP;        /* Proporcional */
	PID.Ki = PID_PARAM_KI;        /* Integral */
	PID.Kd = PID_PARAM_KD;        /* Derivative */
	TickType_t lastWakeTime = xTaskGetTickCount();
	/* Initialize PID system, float32_t format */
	arm_pid_init_f32(&PID, 1);
	set_point = 1.0;
#ifdef COUNT_CYCLES
	// Configura el contador de ciclos con el clock de la EDU-CIAA NXP
	cyclesCounterConfig(EDU_CIAA_NXP_CLOCK_SPEED);
	volatile uint32_t cyclesElapsed = 0;
#endif
	// Bucle de control
	while (1)
	{

#ifdef COUNT_CYCLES
		// Resetea el contador de ciclos
		cyclesCounterReset();
#endif
		// Leer el valor del sensor de la variable "input"
		float32_t input = read_sensor();

		// Calcular el error de seguimiento
		error = set_point - input;

		// Actualizar la salida del controlador PID
		output = arm_pid_f32(&PID, error);

		if (output > lim_max) {
			output = lim_max;
		} else if (output < lim_min) {
			output = lim_min;
		}

		// Escribir la salida del controlador en el actuador
		write_actuator(output);
		//printf ("%.2f / %.2f \r\n", (input), (output));
		//		console_float(input,output);

#ifdef COUNT_CYCLES
		// Leer conteco actual de ciclos
		cyclesElapsed = DWT_CYCCNT;
		volatile float us = cyclesCounterToUs(cyclesElapsed);
#endif


		vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(1000 / 1000));
	}
}

//void console_float (float data,float data2)
//{
//	int32_t integer, fraction;
//	char data_str[10];
//	char data2_str[10];
//
//	// Casteo de float a int
//	integer = (int)data;
//	fraction = (int)(((data - (float)integer)) * 1000);
//	if (fraction<0)
//	{
//		fraction = (-1)*fraction;
//		if (integer==0)
//		{
//			sprintf(data_str,"-%d.%03d", integer, fraction);
//			//printf("-%d.%03d", integer, fraction);
//		}
//		else
//		{
//			sprintf(data_str,"%d.%03d", integer, fraction);
//			//printf("%d.%03d", integer, fraction);
//		}
//	}
//	else
//	{
//		sprintf(data_str,"%d.%03d", integer, fraction);
//		//printf("%d.%03d", integer, fraction);
//	}
//
//	integer = (int)data2;
//	fraction = (int)(((data2 - (float)integer)) * 1000);
//	if (fraction<0)
//	{
//		fraction = (-1)*fraction;
//		if (integer==0)
//		{
//			sprintf(data2_str,"-%d.%03d", integer, fraction);
//			//printf("-%d.%03d", integer, fraction);
//		}
//		else
//		{
//			sprintf(data2_str,"%d.%03d", integer, fraction);
//			//printf("%d.%03d", integer, fraction);
//		}
//	}
//	else
//	{
//		sprintf(data2_str,"%d.%03d", integer, fraction);
//		//printf("%d.%03d", integer, fraction);
//	}
//	printf("%s/%s\r\n",data_str,data2_str);
//}


void app_pid(void)
{
	xTaskCreate(task_PID, "task_PID", configMINIMAL_STACK_SIZE*4, NULL,tskIDLE_PRIORITY + 1, NULL);
}
