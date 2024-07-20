/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include "driver_timer.h"
#include "driver_led.h"
#include "driver_passive_buzzer.h"
#include "driver_active_buzzer.h"
#include "driver_key.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

uint32_t count = 0;
TaskHandle_t  xLedTaskHandle;

/*TASK1任务配,堆栈内存大小，任务优先级*/
 void START_TASK( void * pvParameters );
#define START_TASK_PRIO         osPriorityNormal
#define START_TASK_STACK_SIZE   128
TaskHandle_t  START_TASK_handler;

 void task1( void * pvParameters );
#define TASK1_PRIO         osPriorityNormal
#define TASK1_STACK_SIZE   128
TaskHandle_t  TASK1_handler;

void task2(void * pvParameters );
#define TASK2_PRIO         osPriorityNormal
#define TASK2_STACK_SIZE   70
TaskHandle_t  TASK2_handler;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */




/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
    /* USER CODE BEGIN Init */


    /* USER CODE END Init */

    /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
    /* USER CODE END RTOS_MUTEX */

    /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
    /* USER CODE END RTOS_SEMAPHORES */

    /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
    /* USER CODE END RTOS_TIMERS */

    /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
    /* USER CODE END RTOS_QUEUES */

    /* Create the thread(s) */
    /* creation of defaultTask */
    defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

    /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
    xTaskCreate((TaskFunction_t)START_TASK,"START_TASK",START_TASK_STACK_SIZE,NULL,START_TASK_PRIO,&START_TASK_handler);
    
    /* USER CODE END RTOS_THREADS */

    /* USER CODE BEGIN RTOS_EVENTS */
    /* add events, ... */
    /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
    /* USER CODE BEGIN StartDefaultTask */
    OLED_Clear();
	uint32_t StartDefault_num = 0;
    OLED_ShowString(1,1,"hello world");
    /* Infinite loop */
    for(;;)	
    { 
		taskENTER_CRITICAL();
		printf("StartDefault运行次数:%d！！！!\r\n",++StartDefault_num);
        taskEXIT_CRITICAL();
		OLED_ShowNum(2,1,count++,3);
        vTaskDelay(1000);

    }
    /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
 void START_TASK( void * pvParameters )
 {
	xTaskCreate((TaskFunction_t)task1,"Task1",TASK1_STACK_SIZE,NULL,TASK1_PRIO,&TASK1_handler);
	xTaskCreate((TaskFunction_t)task2,"Task2",TASK2_STACK_SIZE,NULL,TASK2_PRIO,&TASK2_handler);
	vTaskDelete(NULL);
 }

 void task1( void * pvParameters )
 {
	Led_Init();
	uint32_t task1_num = 0;
    while(1)
	{
		taskENTER_CRITICAL();
		printf("task1_num:%d！！！!\r\n",++task1_num);
		taskEXIT_CRITICAL();
		
        Led_Control(LED_GREEN, 1);
        vTaskDelay(500);

        Led_Control(LED_GREEN, 0);
        vTaskDelay(500);
	}
	
 }

void task2 (void * pvParameters )
 {
	UBaseType_t priortiy0 = 0;
	UBaseType_t priortiy1 = 0;
	UBaseType_t priortiy2 = 0;
	UBaseType_t task_num = 0;   
	UBaseType_t task_num2 = 0;
	UBaseType_t task_stack_min = 0;
    TaskStatus_t *status_array = 0;
	TaskStatus_t *status_array2 = 0; 
	TaskHandle_t task_handle = 0;
//	char task_buff[300];
	 
	 
	vTaskPrioritySet(TASK1_handler,4);
	priortiy0 =	uxTaskPriorityGet(TASK1_handler); 
	priortiy1 = uxTaskPriorityGet(NULL);
    priortiy2 = uxTaskPriorityGet(defaultTaskHandle);
	task_num = uxTaskGetNumberOfTasks();
	status_array = pvPortMalloc(sizeof(TaskStatus_t)*task_num);
	status_array2 = pvPortMalloc(sizeof(TaskStatus_t));
	vTaskGetInfo(TASK1_handler,status_array2,pdTRUE,eInvalid);
	task_num2 = uxTaskGetSystemState(status_array,task_num,NULL);
//	vTaskList(task_buff);
//	task_handle = xTaskGetHandle("Task2");
	 
	taskENTER_CRITICAL();
	printf("task1任务优先级%1d\r\n",(int)priortiy0);
    printf("task2任务优先级%1d\r\n",(int)priortiy1);
	printf("StartDefault任务优先级%1d\r\n",(int)priortiy2);
	printf("任务数量：%1d\r\n",(int)task_num);
	printf("任务名\t\t\t任务优先级\t\t\t任务编号\r\n");
	for(int8_t i =0; i<task_num2;i++)
	{
		printf("%s\t\t\t%1d\t\t\t%1d\r\n",
				status_array[i].pcTaskName,
				(int)status_array[i].uxBasePriority,
				(int)status_array[i].xTaskNumber);
	}
	printf("任务名：%s\t\t\r\n",status_array2->pcTaskName);
	printf("任务优先级：%1d\t\t\r\n",(int)status_array2->uxCurrentPriority);
	printf("任务编号：%1d\t\t\r\n",(int)status_array2->xTaskNumber);
	printf("任务状态：%d\t\t\r\n",status_array2->eCurrentState);
	
//	printf("任务句柄：%#x\r\n",(int)task_handle);
	printf("task1的任务句柄:%#x\r\n",(int)TASK1_handler);
//	printf("%s\r\n",task_buff); 
	 //uxTaskGetSystemState
	taskEXIT_CRITICAL(); 
    
	while(1)
	{
	  task_stack_min = uxTaskGetStackHighWaterMark(TASK2_handler);
	  printf("task2历史剩余最小堆栈位%1d\r\n",(int)task_stack_min);	
	  vTaskDelay(1000);
	}
	
 }
 
/* USER CODE END Application */

