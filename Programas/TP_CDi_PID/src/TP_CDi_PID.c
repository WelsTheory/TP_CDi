/*=============================================================================
 * Author: Wels <hola@weltheory.com>
 * Date: 2023/04/16
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "arm_math.h"
#include "sapi.h"
#include "pidTask.h"
#include "app_pid.h"

/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/
//#define TP_APP_PID
#define TP_PID_TASK
/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/

int main( void )
{
	boardConfig();
	adcConfig(ADC_ENABLE);
	dacConfig(DAC_ENABLE);
	uartConfig( UART_USB, 460800);
	// Create a task in freeRTOS with dynamic memory
	//xTaskCreate(task_DAC_Pulse, "task_DAC_Pulse", configMINIMAL_STACK_SIZE*2, NULL,tskIDLE_PRIORITY + 1, NULL);
#ifdef TP_PID_TASK
	xTaskCreate(
			pidControlTask,                 // Function that implements the task.
			(const char *)"pidControlTask", // Text name for the task.
			configMINIMAL_STACK_SIZE*2,     // Stack size in words, not bytes.
			0,                              // Parameter passed into the task.
			tskIDLE_PRIORITY+1,             // Priority at which the task is created.
			0                               // Pointer to the task created in the system
	);
#else
	app_pid();
#endif
	vTaskStartScheduler(); // Initialize scheduler

	while( true ); // If reach heare it means that the scheduler could not start

	// YOU NEVER REACH HERE, because this program runs directly or on a
	// microcontroller and is not called by any Operating System, as in the
	// case of a PC program.
	return 0;
}
