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
#include "semphr.h"
#include "queue.h"
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

QueueHandle_t key_queuehandle;
QueueHandle_t semphore_handle;
QueueSetHandle_t queueset_handle;

char buff[100] = {"hello world liulichuchang"};

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
#define TASK2_PRIO         osPriorityNormal+1
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
     BaseType_t err1;
     BaseType_t err2;
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
	 queueset_handle = xQueueCreateSet(2);
	 if(queueset_handle != NULL)
	 {
		printf("队列集创建成功！！！\r\n");
	 }
	 key_queuehandle = xQueueCreate(1,sizeof(uint8_t));
	 if(key_queuehandle != NULL)
	 {
		printf("队列创建成功！！！！\r\n");
	 }
     semphore_handle = xSemaphoreCreateBinary();
	 if(semphore_handle != NULL)
	 {
		printf("二值信号量创建成功！！！！\r\n");
	 }
	 err1 = xQueueAddToSet(key_queuehandle,queueset_handle);
	 if(err1 == pdPASS)
	 {
	  printf("key_queuehandle加入队列集");
	 }
	 err2 = xQueueAddToSet(semphore_handle,queueset_handle);
	 if(err2 == pdPASS)
	 {
	  printf("semphore_handle加入队列集");
	 }
	 
	
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
	Key_Init();
	uint8_t key_num = 0;
	BaseType_t err = 0;
	BaseType_t err1 = 0; 
    while(1)
	{
	  key_num  = Key_Read();
	  if(key_num == 1)
	  {
		if(semphore_handle != NULL)
		{
			err = xSemaphoreGive(semphore_handle);
			if(err == pdPASS)
			{
				printf("信号量释放成功！！！\r\n");
			}
			else
			{
			    printf("信号量释放失败！！！\r\n");
			}
		}
	  }
	  else if(key_num == 2)
	  {
		if(key_queuehandle != NULL )
		{
		  err1 = xQueueSend(key_queuehandle,&key_num,portMAX_DELAY);
		  if(err == pdPASS)
		  {
			 printf("队列发送入队成功！！！\r\n");
		  }
		  else
		  {
			printf("队列发送入队失败！！！\r\n");
		  }
		}
	  }
	  vTaskDelay(10);
	}
	
 }

void task2 (void * pvParameters )
 {
	QueueSetMemberHandle_t member_handle;
    uint8_t key = 0;
	BaseType_t err = 0;
	BaseType_t err1 = 0;
	Led_Init();
	while(1)
	{
	  member_handle = xQueueSelectFromSet(queueset_handle,portMAX_DELAY);
	  if(member_handle == key_queuehandle)
	  {
	    err1 = xQueueReceive(member_handle,&key,portMAX_DELAY);
		if(err1 == pdPASS)
		{
		   printf("获取到队列数据为：%d\r\n",key);
		}
		else
		{
		   printf("获取失败！！！\r\n");
		}
		
	  }
	  else if(member_handle == semphore_handle)
	  {
	    err = xSemaphoreTake(semphore_handle,portMAX_DELAY);
	    if(err == pdPASS)
	    {
		   Led_Control(LED_GREEN, 1);
		   vTaskDelay(500);

		   Led_Control(LED_GREEN, 0);
		   vTaskDelay(500);
	    }
	  }
      
	}
 }
 
/* USER CODE END Application */

