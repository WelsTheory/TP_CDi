/*=============================================================================
 * Copyright (c) 2019, Eric Pernia <ericpernia@gmail.com>
 * All rights reserved.
 * License: bsd-3-clause (see LICENSE.txt)
 * Date: 2019/10/8
 * Version: 1.1.0
 *
 * Changelog:
 * Version 1.1.0, 2019/10/8, Santiago Germino <sgermino@retro-ciaa.com>:
 *    1) Name change for SCALE_OUT(Y), SCALE_REF(R), SCALE_PID_OUT(U).
 *    2) Proper values for SCALE_Y, SCALE_R and SCALE_U.
 *    3) Proper scaling of Ki and Kd by h_s.
 *    4) Added calls to enable and initialize ADC & DAC.
 *===========================================================================*/

/*=====[Inclusion of own header]=============================================*/

#include "pidTask.h"

/*=====[Inclusions of private function dependencies]=========================*/

#include "sapi.h"
#include "pid_controller.h"

/*=====[Definition macros of private constants]==============================*/

#define SCALE_Y         0.003225806f    // 3.3 / 1023     10-bit ADC to Voltage
#define SCALE_R         0.003225806f    // 3.3 / 1023     10-bit ADC to Voltage
#define SCALE_U         310.0f          // 1023 / 3.3     Voltage to 10-bit DAC

/*=====[Private function-like macros]========================================*/
//#define COUNT_CYCLES
#define KP_2
//#define KI_49
//#define KP_54
//#define KI_4703

//#define random_coef
/*=====[Definitions of private data types]===================================*/

/*=====[Definitions of external public global variables]=====================*/
#ifdef KI_49
#define Coef_KP        0.0817f            /* Proporcional */
#define Coef_KI        49.1f        		/* Integral */
#define Coef_KD        0.0000339f        /* Derivative */
#endif
#ifdef KP_2
#define Coef_KP        2.5f            /* Proporcional */
#define Coef_KI        1.0f        		/* Integral */
#define Coef_KD        0.0f        /* Derivative */
#endif
#ifdef KP_54
#define Coef_KP        0.12259f            /* Proporcional */
#define Coef_KI        54.0345f        		/* Integral */
#define Coef_KD        0.000069531f        /* Derivative */
#endif

#ifdef KI_4703
#define Coef_KP        10.3845f            /* Proporcional */
#define Coef_KI        4703.5711f        		/* Integral */
#define Coef_KD        0.0038301f        /* Derivative */
#endif

#ifdef random_coef
#define Coef_KP        2.01f            /* Proporcional */
#define Coef_KI        1.52f        		/* Integral */
#define Coef_KD        0.043f        /* Derivative */
#endif

/*=====[Definitions of public global variables]==============================*/

/*=====[Definitions of private global variables]=============================*/
void console_float (float data,float data2);
/*=====[Prototypes (declarations) of private functions]======================*/
static int16_t dacValue;


/*=====[Implementations of public functions]=================================*/

// Para calcular el tiempo que tarda el algoritmo y establecer un h minimo
#define COUNT_CYCLES

// Task implementation
void pidControlTask( void* taskParmPtr )
{
	// Controller signals
	float r = 0.0f; // Measure of reference r[k]
	float y = 0.0f; // Measure of system output y[k]
	float u = 0.0f; // Calculated controller's output u[k]

	// h no puede ser menor ni al tiempo del algoritmo, y con va a ser un
	// multiplo del periodo de tick del RTOS
	uint32_t h_ms = 1; // Task periodicity (sample rate)
	float h_s = ((float)h_ms)/1000.0f;

	// PID Controller structure (like an object instance)
	PIDController_t PsPIDController;

	// PID Controller Initialization
	pidInit( &PsPIDController,
			Coef_KP,                  // Kp
			Coef_KI / h_s,            // Ki
			Coef_KD * h_s,            // Kd
			h_s,                   // h en [s]
			20.0f,                 // N
			1.0f,                  // b
			0.0f,                  // u_min
			3.3f                   // u_max
	);

	// Peridodic task each h_ms
	portTickType xPeriodicity =  h_ms / portTICK_RATE_MS;
	portTickType xLastWakeTime = xTaskGetTickCount();

#ifdef COUNT_CYCLES
	// Configura el contador de ciclos con el clock de la EDU-CIAA NXP
	cyclesCounterConfig(EDU_CIAA_NXP_CLOCK_SPEED);
	volatile uint32_t cyclesElapsed = 0;
#endif

	while(true) {

#ifdef COUNT_CYCLES
		// Resetea el contador de ciclos
		cyclesCounterReset();
#endif

		// Leer salida y[k] y refererencia r[k]
		y = adcRead( CH1 ) * SCALE_Y;
		r = adcRead( CH2 ) * SCALE_R;
		//r = 1.0;


		// Calculate PID controller output u[k]
		u = pidCalculateControllerOutput( &PsPIDController, y, r ) * SCALE_U;

		// Actualizar la salida u[k]
		dacWrite( DAC, u);

		// Update PID controller for next iteration
		pidUpdateController( &PsPIDController, y, r );

		console_float(y/3.3,u/1023.0);

#ifdef COUNT_CYCLES
		// Leer conteco actual de ciclos
		cyclesElapsed = DWT_CYCCNT;
		volatile float us = cyclesCounterToUs(cyclesElapsed);
#endif
		// Send the task to the locked state during xPeriodicity
		// (periodical delay)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}
}

void console_float (float data,float data2)
{
	int32_t integer, fraction;
	char data_str[10];
	char data2_str[10];

	// Casteo de float a int
	integer = (int)data;
	fraction = (int)(((data - (float)integer)) * 1000);
	if (fraction<0)
	{
		fraction = (-1)*fraction;
		if (integer==0)
		{
			sprintf(data_str,"-%d.%03d", integer, fraction);
			//printf("-%d.%03d", integer, fraction);
		}
		else
		{
			sprintf(data_str,"%d.%03d", integer, fraction);
			//printf("%d.%03d", integer, fraction);
		}
	}
	else
	{
		sprintf(data_str,"%d.%03d", integer, fraction);
		//printf("%d.%03d", integer, fraction);
	}

	integer = (int)data2;
	fraction = (int)(((data2 - (float)integer)) * 1000);
	if (fraction<0)
	{
		fraction = (-1)*fraction;
		if (integer==0)
		{
			sprintf(data2_str,"-%d.%03d", integer, fraction);
			//printf("-%d.%03d", integer, fraction);
		}
		else
		{
			sprintf(data2_str,"%d.%03d", integer, fraction);
			//printf("%d.%03d", integer, fraction);
		}
	}
	else
	{
		sprintf(data2_str,"%d.%03d", integer, fraction);
		//printf("%d.%03d", integer, fraction);
	}
	printf("%s/%s\r\n",data_str,data2_str);
}

/*=====[Implementations of interrupt functions]==============================*/

/*=====[Implementations of private functions]================================*/

