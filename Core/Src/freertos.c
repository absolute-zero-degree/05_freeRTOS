/* USER CODE BEGIN Header */
#include "driver_led.h"
#include "driver_lcd.h"
#include "driver_mpu6050.h"
#include "driver_timer.h"
#include "driver_ds18b20.h"
#include "driver_dht11.h"
#include "driver_active_buzzer.h"
#include "driver_passive_buzzer.h"
#include "driver_color_led.h"
#include "driver_ir_receiver.h"
#include "driver_ir_sender.h"
#include "driver_light_sensor.h"
#include "driver_ir_obstacle.h"
#include "driver_ultrasonic_sr04.h"
#include "driver_spiflash_w25q64.h"
#include "driver_rotary_encoder.h"
#include "driver_motor.h"
#include "driver_key.h"
#include "driver_uart.h"

/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
  TaskHandle_t xPlaymusicTaskHandle;
  BaseType_t xPlaymusicTaskReturnValue;

  static uint32_t Led_Test_Stack[128];
  static StaticTask_t Led_Test_TCB;
  TaskHandle_t xLed_TestTaskHandle;

  static uint32_t Color_Test_Stack[128];
  static StaticTask_t Color_Test_TCB;
  TaskHandle_t xColor_TestTaskHandle;

  extern void PlayMusic(void *params);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

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

  // xPlaymusicTaskReturnValue = xTaskCreate(PlayMusic, "PlayMusic", 128, NULL, osPriorityAboveNormal, &xPlaymusicTaskHandle);
  
  xLed_TestTaskHandle = xTaskCreateStatic(Led_Test, "Led_Test", 128, NULL, osPriorityNormal, Led_Test_Stack, &Led_Test_TCB);

  xColor_TestTaskHandle = xTaskCreateStatic(ColorLED_Test, "ColorLED_Test", 128, NULL, osPriorityNormal, Color_Test_Stack, &Color_Test_TCB);

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
  /* Infinite loop */
  uint8_t dev, data;
  int len;
  int bRunning = 1;

  LCD_Init();
  LCD_Clear();
  xPlaymusicTaskHandle = NULL;
  IRReceiver_Init();
  LCD_PrintString(0, 0, "created task");

  while (1)
  {
      if(0 == IRReceiver_Read(&dev, &data))   
      {
        if (data == 0xa8)
        {
          if (xPlaymusicTaskHandle == NULL)
          {
            LCD_ClearLine(0,0);
            LCD_PrintString(0, 0, "music is ok");
            xPlaymusicTaskReturnValue = xTaskCreate(PlayMusic, "PlayMusic", 256, NULL, osPriorityAboveNormal+1, &xPlaymusicTaskHandle);\
          }
          else
          {
            if(bRunning)
            {
              LCD_ClearLine(0,0);
              LCD_PrintString(0, 0, "pause music");
              vTaskSuspend(xPlaymusicTaskHandle);
              PassiveBuzzer_Control(0);
              bRunning = 0;
            }
            else
            {
              LCD_ClearLine(0,0);
              LCD_PrintString(0, 0, "resume music");
              vTaskResume(xPlaymusicTaskHandle);
              bRunning = 1;
            }
          }
        }
        else if (data == 0xa2)
        {
          if (xPlaymusicTaskHandle != NULL)
          {
            LCD_ClearLine(0,0);
            LCD_PrintString(0, 0, "delete music");
            vTaskDelete(xPlaymusicTaskHandle);
            PassiveBuzzer_Control(0); /* 停止蜂鸣器 */
            xPlaymusicTaskHandle = NULL;
          }
        }
      }
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

