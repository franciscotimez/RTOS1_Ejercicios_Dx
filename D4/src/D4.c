/* Copyright 2020, Franco Bucafusco
 * All rights reserved.
 *
 * This file is part of sAPI Library.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*=====[Inclusions of function dependencies]=================================*/
#include "FreeRTOSConfig.h"
#include "keys.h"
#include "FreeRTOS.h"
#include "task.h"

#include "sapi.h"

/*=====[Definition & macros of public constants]==============================*/

#define PERIODO_LED2 pdMS_TO_TICKS( 2000 )
#define TIEMPO_C1_DEC_LED2 pdMS_TO_TICKS( 100 )

/*=====[Definitions of extern global functions]==============================*/

// Prototipo de funcion de la tarea
void task_led( void* taskParmPtr );
void task_led2( void* taskParmPtr );
void task_tecla( void* taskParmPtr );

/*=====[Definitions of public global variables]==============================*/

/*=====[Main function, program entry point after power on or reset]==========*/

int main( void )
{
	BaseType_t res;

	// ---------- CONFIGURACIONES ------------------------------
	boardConfig();  // Inicializar y configurar la plataforma

	printf( "Ejercicio D4\n" );

	// Crear tareas en freeRTOS
	res = xTaskCreate (
			task_led,					// Funcion de la tarea a ejecutar
			( const char * )"Tarea 2",	// Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE*2,	// Cantidad de stack de la tarea
			0,							// Parametros de tarea
			tskIDLE_PRIORITY+1,			// Prioridad de la tarea
			0							// Puntero a la tarea creada en el sistema
	);

	// Gestión de errores
	configASSERT( res == pdPASS );

	res = xTaskCreate (
			task_led2,					// Funcion de la tarea a ejecutar
			( const char * )"Tarea 3",	// Nombre de la tarea como String amigable para el usuario
			configMINIMAL_STACK_SIZE*2,	// Cantidad de stack de la tarea
			0,							// Parametros de tarea
			tskIDLE_PRIORITY+1,			// Prioridad de la tarea
			0							// Puntero a la tarea creada en el sistema
	);

	// Gestión de errores
	configASSERT( res == pdPASS );

	/* inicializo driver de teclas */
	keys_Init();

	// Iniciar scheduler
	vTaskStartScheduler();					// Enciende tick | Crea idle y pone en ready | Evalua las tareas creadas | Prioridad mas alta pasa a running

	/* realizar un assert con "false" es equivalente al while(1) */
	configASSERT( 0 );
	return 0;
}

void task_led( void* taskParmPtr )
{
	TickType_t xPeriodicity;

	TickType_t xLastWakeTime = xTaskGetTickCount();

	while( 1 )
	{
		xPeriodicity = get_tiempoC1();

		if( xPeriodicity != KEYS_INVALID_TIME )

		{
			gpioWrite( LED1, ON );
			vTaskDelay( xPeriodicity >> 1 );
			gpioWrite( LED1, OFF );
		}

		// Envia la tarea al estado bloqueado durante xPeriodicity (delay periodico)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}
}

void task_led2( void* taskParmPtr )
{
	TickType_t xPeriodicity = PERIODO_LED2; // Tarea periodica cada 1000 ms

	TickType_t xLastWakeTime = xTaskGetTickCount();

	while( 1 )
	{
		TickType_t tiempoC1_l = get_tiempoC1();

		if( tiempoC1_l != KEYS_INVALID_TIME )

		{
			gpioWrite( LED2, ON );
			vTaskDelay( tiempoC1_l << 1 );
			gpioWrite( LED2, OFF );
			subs_tiempoC1( TIEMPO_C1_DEC_LED2 );
		}

		// Envia la tarea al estado bloqueado durante xPeriodicity (delay periodico)
		vTaskDelayUntil( &xLastWakeTime, xPeriodicity );
	}
}

/* hook que se ejecuta si al necesitar un objeto dinamico, no hay memoria disponible */
void vApplicationMallocFailedHook()
{
	printf( "Malloc Failed Hook!\n" );
	configASSERT( 0 );
}
