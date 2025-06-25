#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stdio.h"

#define HIGH_PRIORITY   ( tskIDLE_PRIORITY + 3 )
#define MEDIUM_PRIORITY ( tskIDLE_PRIORITY + 2 )
#define LOW_PRIORITY    ( tskIDLE_PRIORITY + 1 )

static SemaphoreHandle_t xMutex;

static void vTaskLow( void *pvParameters )
{
    printf("Low: My priority is %d.\r\n", uxTaskPriorityGet( NULL ));
    vTaskDelay( pdMS_TO_TICKS( 10 ) );
    printf("Low: Trying to TAKE mutex...\r\n");
    xSemaphoreTake( xMutex, portMAX_DELAY );
    printf("Low: got it. Doing work for 250ms...\r\n");
    vTaskDelay( pdMS_TO_TICKS( 250 ) );
    printf("Low: My priority is %d.\r\n", uxTaskPriorityGet( NULL )); 
    printf("Low: RELEASING mutex.\r\n");
    // print priority of this task
    xSemaphoreGive( xMutex );
    vTaskDelete( NULL );
}


static void vTaskMedium( void *pvParameters )
{
    int count = 0;
    vTaskDelay( pdMS_TO_TICKS( 25 ) );
    for( ;; )
    {
        printf("Medium: Running (but Low may inherit my priority)... %d and pritority %d \r\n", count++ ,uxTaskPriorityGet( NULL ));
        vTaskDelay( pdMS_TO_TICKS( 50 ) );
    }
}

static void vTaskHigh( void *pvParameters )
{
    vTaskDelay( pdMS_TO_TICKS( 20 ) );
    printf("High: Trying to TAKE mutex...\r\n");
    printf("High: My priority is %d.\r\n", uxTaskPriorityGet( NULL ));
    xSemaphoreTake( xMutex, portMAX_DELAY );
    printf("High: got it! Immediately releasing.\r\n");
    xSemaphoreGive( xMutex );
    vTaskDelete( NULL );
}

int main( void )
{
    /* Create a mutex (with priority inheritance enabled in FreeRTOSConfig.h) */
    xMutex = xSemaphoreCreateMutex();

    xTaskCreate( vTaskLow,    "Low",    configMINIMAL_STACK_SIZE, NULL, LOW_PRIORITY,    NULL );
    xTaskCreate( vTaskMedium, "Medium", configMINIMAL_STACK_SIZE, NULL, MEDIUM_PRIORITY, NULL );
    xTaskCreate( vTaskHigh,   "High",   configMINIMAL_STACK_SIZE, NULL, HIGH_PRIORITY,   NULL );

    vTaskStartScheduler();
    for( ;; );
}