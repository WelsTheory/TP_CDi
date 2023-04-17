/*=====[Inclusions of function dependencies]=================================*/

#include <stdlib.h>

#include "FreeRTOS.h"
#include "arm_math.h"
#include "sapi.h"
#include "task.h"

static int16_t dacValue;

void app_main(void);
void console_float (float data, uint16_t data2);

static void task_DAC_Pulse(void *taskParmPtr) {
	TickType_t lastWakeTime = xTaskGetTickCount();
	for (;;) {
		dacValue ^= 0xFFFF;
		dacValue &= 0xFFFF;
		dacWrite(DAC,dacValue);
		vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(1000 / 10));
	}
}

static void task_ADC_PRINT(void *taskParmPtr) {
	static uint32_t sample = 0;
	float adc_value = 0.0;
	TickType_t lastWakeTime = xTaskGetTickCount();
	for (;;) {
		sample = adcRead(CH1);
		adc_value = sample/1023.0;
		console_float(adc_value, (uint16_t)dacValue>>15);
		//printf ("%d/%d\r\n", ((uint16_t)sample), ((uint16_t)dacValue>>15));
		vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(1000 / 1000));
	}
}

void app_main(void)
{
	xTaskCreate(task_DAC_Pulse, "task_DAC_Pulse", configMINIMAL_STACK_SIZE*2, NULL,tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(task_ADC_PRINT, "task_ADC_Print",configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY + 1, NULL);
}

void console_float (float data, uint16_t data2)
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

	sprintf(data2_str,"%d", data2);
	printf("%s/%s \r\n",data_str,data2_str);
}

int main(void) {
	// ----- Configurations --------------------------
	boardConfig();

	// ----- Initialize ADC and DAC ------------------
	adcInit( ADC_ENABLE );
	dacInit( DAC_ENABLE );
	uartConfig( UART_USB, 460800);

	app_main();

	vTaskStartScheduler();
	configASSERT( 0 );
	/* Code won't reach here. */
	while(1)
	{

	}

}
