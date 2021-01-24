/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c_at24c64.h"
#include "stm32l476g_discovery_glass_lcd.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define EEPROM_ADDRESS 0xA0
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
RTC_DateTypeDef RTC_DateStructure;
RTC_TimeTypeDef RTC_TimeStructure;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN PV */
char lcd_buffer[6]; // LCD display buffer
char timestring[10] = {0}; // String to store time information
char datestring[6] = {0}; //String to store date information
uint8_t wd, dd, mt, yy, ss1, ss2, mm1, mm2, hh1, hh2; //for weekday, day, month, year, second, minute, hour
__IO uint16_t memLocation = 0x000A; //pick one location within range
__IO uint16_t memLocation_ref = 0x000A; //reference location
__IO uint16_t Cpressed;
__IO uint16_t Lpressed; //save the timestamp A to the eeprom using I2C
__IO uint16_t Rpressed; //read and display timestamp A from eeprom
__IO uint16_t Upressed; //save the timestamp B to the eeprom using I2C
__IO uint16_t Dpressed; //read and display timestamp B from eeprom
__IO uint32_t counter; //Counter
uint16_t EE_status;
int mode = 1;
int presscount = 0;
int submode = 1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
void RTC_TimeShow();
void RTC_DateShow();
void RTC_ModifyShow();
void RTC_ModifyUp();
void RTC_ModifyDown();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	Cpressed = 0;
	Lpressed = 0;
	Rpressed = 0;
	Upressed = 0;
	Dpressed = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	BSP_LCD_GLASS_Init();
	BSP_LED_Init(LED4);
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
 
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    /* USER CODE BEGIN 3 */
		if(mode == 1) //This is the time display mode
		{
			RTC_TimeShow();
		}
		counter = HAL_GetTick(); //Counter get initial value
		while(BSP_JOY_GetState()==JOY_SEL) //If the selection button is being pushed
		{
			if((HAL_GetTick() - counter) > 1000) //Check if the selection button is pushed and hold
			{
				Cpressed = 2;
			}
		}
		
		if(Cpressed == 2) // If the selection button is pushed and hold
		{
			RTC_DateShow(); //Show date
			Cpressed = 0; // Automatically turned off
		}
		if(Cpressed == 1) // If the selection button is pressed
		{
			if(presscount == 1) // First time pressed
			{
				BSP_LCD_GLASS_DisplayString((uint8_t *) "SAVED");
				HAL_Delay(200);
				HAL_RTC_GetTime(&hrtc, &RTC_TimeStructure, RTC_FORMAT_BIN);
				hh1 = RTC_TimeStructure.Hours;
				mm1 = RTC_TimeStructure.Minutes;
				ss1 = RTC_TimeStructure.Seconds;
				sprintf(timestring, "%02d%02d%02d", hh1, mm1, ss1);
				EE_status = I2C_ByteWrite(&hi2c1, EEPROM_ADDRESS, memLocation, hh1);
				if(EE_status != HAL_OK)
				{
					I2C_Error(&hi2c1);
				}
				EE_status = I2C_ByteWrite(&hi2c1, EEPROM_ADDRESS, memLocation + 1, mm1);
				if(EE_status != HAL_OK)
				{
					I2C_Error(&hi2c1);
				}
				EE_status = I2C_ByteWrite(&hi2c1, EEPROM_ADDRESS, memLocation + 2, ss1);
				if(EE_status != HAL_OK)
				{
					I2C_Error(&hi2c1);
				}
			}
				
			else if(presscount == 2) // Second time pressed
			{
				BSP_LCD_GLASS_DisplayString((uint8_t *) "SAVED");
				HAL_Delay(200);
				HAL_RTC_GetTime(&hrtc, &RTC_TimeStructure, RTC_FORMAT_BIN);
				hh2 = RTC_TimeStructure.Hours;
				mm2 = RTC_TimeStructure.Minutes;
				ss2 = RTC_TimeStructure.Seconds;
				sprintf(timestring, "%02d%02d%02d", hh2, mm2, ss2);
				EE_status = I2C_ByteWrite(&hi2c1, EEPROM_ADDRESS, memLocation + 3, hh2);
				if(EE_status != HAL_OK)
				{
					I2C_Error(&hi2c1);
				}
				EE_status = I2C_ByteWrite(&hi2c1, EEPROM_ADDRESS, memLocation + 4, mm2);
				if(EE_status != HAL_OK)
				{
					I2C_Error(&hi2c1);
				}
				EE_status = I2C_ByteWrite(&hi2c1, EEPROM_ADDRESS, memLocation + 5, ss2);
				if(EE_status != HAL_OK)
				{
					I2C_Error(&hi2c1);
				}
				presscount = 0; // Initialize presscount
			}
			Cpressed = 0;
		}
		
		if(Lpressed == 1 && mode == 1) //If in time display mode and left button is pressed
		{
			mode = 2;
			hh1 = I2C_ByteRead(&hi2c1, EEPROM_ADDRESS, memLocation_ref);
			mm1 = I2C_ByteRead(&hi2c1, EEPROM_ADDRESS, memLocation_ref + 1);
			ss1 = I2C_ByteRead(&hi2c1, EEPROM_ADDRESS, memLocation_ref + 2);
			sprintf(timestring, "%02d%02d%02d", hh1, mm1, ss1);
			BSP_LCD_GLASS_DisplayString((uint8_t*)timestring);
			HAL_Delay(1000);
			hh2 = I2C_ByteRead(&hi2c1, EEPROM_ADDRESS, memLocation_ref + 3);
			mm2 = I2C_ByteRead(&hi2c1, EEPROM_ADDRESS, memLocation_ref + 4);
			ss2 = I2C_ByteRead(&hi2c1, EEPROM_ADDRESS, memLocation_ref + 5);
			sprintf(timestring, "%02d%02d%02d", hh2, mm2, ss2);
			BSP_LCD_GLASS_DisplayString((uint8_t*)timestring);
			HAL_Delay(1000);
			Lpressed = 0;
		}
		if(Lpressed == 1 && mode == 2) // If already in EEPROM display mode and left is pressed
		{
			mode = 1; // Go back to time display mode
			Lpressed = 0;
		}
		
		if(Lpressed == 1 && mode == 3) // If current mode is modification mode
		{
			submode++; // Select parameters to be changed
			if(submode > 7)
			{
				submode = 1;
			}
			Lpressed = 0;
		}
		
		if(Upressed == 1)
		{
			RTC_ModifyUp();
			Upressed = 0;
		}
		
		if (Rpressed == 1)
		{
			mode = 3; // Set mode to modification mode
			RTC_ModifyShow();
		}
		if (Dpressed == 1)
		{
			RTC_ModifyDown();
			Dpressed = 0;
		}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 20;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00404C74;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */
  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 02;
  sTime.Minutes = 55;
  sTime.Seconds = 40;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 29;
  sDate.Year = 20;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0;
  sAlarm.AlarmTime.Minutes = 0;
  sAlarm.AlarmTime.Seconds = 0;
  sAlarm.AlarmTime.SubSeconds = 0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_ALL;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : JOY_CENTER_Pin */
  GPIO_InitStruct.Pin = JOY_CENTER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(JOY_CENTER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : JOY_LEFT_Pin JOY_RIGHT_Pin JOY_UP_Pin PA4
                           JOY_DOWN_Pin */
  GPIO_InitStruct.Pin = JOY_LEFT_Pin|JOY_RIGHT_Pin|JOY_UP_Pin|GPIO_PIN_4
                          |JOY_DOWN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin)
	{
		case GPIO_PIN_0:
			Cpressed = 1;
			presscount++; //Presscount plus one, preparing for next press

			break;
		
		case GPIO_PIN_1:
			Lpressed = 1;
				
			break;
		
		case GPIO_PIN_2:
			Rpressed = 1;
			if(mode == 3) //If the current mode is modification mode
			{
				Rpressed = 0; //Stop function modifyShow()
				mode = 1; //  Turn mode back to display time mode
			}
			break;
		
		case GPIO_PIN_3:
			Upressed = 1;
			break;
		
		case GPIO_PIN_5:
			Dpressed = 1;
			break;
		
	}
}
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef*hrtc)
{
	BSP_LED_Toggle(LED4);
}

void RTC_TimeShow()
{
	BSP_LCD_GLASS_Clear();
	HAL_RTC_GetTime(&hrtc, &RTC_TimeStructure, RTC_FORMAT_BIN); //Get current time from RTC handles and save to RTC_TimeStructure
	sprintf(timestring, "%02d%02d%02d", RTC_TimeStructure.Hours, RTC_TimeStructure.Minutes, RTC_TimeStructure.Seconds); //Formating time and display
	BSP_LCD_GLASS_DisplayString((uint8_t*)timestring);
	
	// Polish the display format
	BSP_LCD_GLASS_DisplayChar((uint8_t*)&timestring[1], POINT_OFF, DOUBLEPOINT_ON, LCD_DIGIT_POSITION_2);
	BSP_LCD_GLASS_DisplayChar((uint8_t*)&timestring[3], POINT_OFF, DOUBLEPOINT_ON, LCD_DIGIT_POSITION_4);
	
	HAL_RTC_GetDate(&hrtc, &RTC_DateStructure, RTC_FORMAT_BIN);
}

void RTC_DateShow()
{
	BSP_LCD_GLASS_Clear();
	
	HAL_RTC_GetDate(&hrtc, &RTC_DateStructure, RTC_FORMAT_BIN);
	
	sprintf(datestring, "WD  %02d", RTC_DateStructure.WeekDay);
	BSP_LCD_GLASS_DisplayString((uint8_t *) datestring);
	HAL_Delay(1500);
	
	sprintf(datestring, "MT  %02d", RTC_DateStructure.Month);
	BSP_LCD_GLASS_DisplayString((uint8_t *) datestring);
	HAL_Delay(1500);
	
	sprintf(datestring, "DD  %02d", RTC_DateStructure.Date);
	BSP_LCD_GLASS_DisplayString((uint8_t *) datestring);
	HAL_Delay(1500);
	
	sprintf(datestring, "YY%04d", RTC_DateStructure.Year + 2000);
	BSP_LCD_GLASS_DisplayString((uint8_t *) datestring);
	HAL_Delay(1500);
}

void RTC_ModifyShow()
{
	switch(submode)
	{
		case 1:
			sprintf(datestring, "SS  %02d", RTC_TimeStructure.Seconds);
			BSP_LCD_GLASS_DisplayString((uint8_t *) datestring);
			break;
		case 2:
			sprintf(datestring, "MM  %02d", RTC_TimeStructure.Minutes);
			BSP_LCD_GLASS_DisplayString((uint8_t *) datestring);
			break;
		case 3:
			sprintf(datestring, "HH  %02d", RTC_TimeStructure.Hours);
			BSP_LCD_GLASS_DisplayString((uint8_t *) datestring);
			break;
		case 4:
			sprintf(datestring, "WD  %02d", RTC_DateStructure.WeekDay);
			BSP_LCD_GLASS_DisplayString((uint8_t *) datestring);
			break;
		case 5:
			sprintf(datestring, "DD  %02d", RTC_DateStructure.Date);
			BSP_LCD_GLASS_DisplayString((uint8_t *) datestring);
			break;
		case 6:
			sprintf(datestring, "MT  %02d", RTC_DateStructure.Month);
			BSP_LCD_GLASS_DisplayString((uint8_t *) datestring);
			break;
		case 7:
			sprintf(datestring, "YY%04d", RTC_DateStructure.Year + 2000);
			BSP_LCD_GLASS_DisplayString((uint8_t *) datestring);
			break;
	}
}

void RTC_ModifyUp()
{
	switch(submode)
	{
		case 1:
			if(RTC_TimeStructure.Seconds > 58)
			{
				RTC_TimeStructure.Seconds = 0;
			}
			else
			{
				RTC_TimeStructure.Seconds++;
				HAL_RTC_SetTime(&hrtc, &RTC_TimeStructure, RTC_FORMAT_BIN);
			}
			break;
		case 2:
			if(RTC_TimeStructure.Minutes > 58)
			{
				RTC_TimeStructure.Minutes = 0;
			}
			else
			{
				RTC_TimeStructure.Minutes++;
				HAL_RTC_SetTime(&hrtc, &RTC_TimeStructure, RTC_FORMAT_BIN);
			}
			break;
			
		case 3:
			if(RTC_TimeStructure.Hours > 22)
			{
				RTC_TimeStructure.Hours = 0;
			}
			else
			{
				RTC_TimeStructure.Hours++;
				HAL_RTC_SetTime(&hrtc, &RTC_TimeStructure, RTC_FORMAT_BIN);
			}
			break;
			
		case 4:
			if(RTC_DateStructure.WeekDay > 6)
			{
				RTC_DateStructure.WeekDay = 1;
			}
			else
			{
				RTC_DateStructure.WeekDay++;
				HAL_RTC_SetDate(&hrtc, &RTC_DateStructure, RTC_FORMAT_BIN);
			}
			break;
			
		case 5:
			if(RTC_DateStructure.Date > 29)
			{
				RTC_DateStructure.Date = 1;
			}
			else
			{
				RTC_DateStructure.Date++;
				HAL_RTC_SetDate(&hrtc, &RTC_DateStructure, RTC_FORMAT_BIN);
			}
			break;
			
		case 6:
			if(RTC_DateStructure.Month > 11)
			{
				RTC_DateStructure.Month = 1;
			}
			else
			{
				RTC_DateStructure.Month++;
				HAL_RTC_SetDate(&hrtc, &RTC_DateStructure, RTC_FORMAT_BIN);
			}
			break;
			
		case 7:
			RTC_DateStructure.Year++;
			HAL_RTC_SetDate(&hrtc, &RTC_DateStructure, RTC_FORMAT_BIN);
			break;
	}
}
	
void RTC_ModifyDown()
{
	switch(submode)
	{
		case 1:
			if(RTC_TimeStructure.Seconds == 0)
			{
				RTC_TimeStructure.Seconds = 59;
			}
			else
			{
				RTC_TimeStructure.Seconds--;
				HAL_RTC_SetTime(&hrtc, &RTC_TimeStructure, RTC_FORMAT_BIN);
			}
			break;
		case 2:
			if(RTC_TimeStructure.Minutes == 0)
			{
				RTC_TimeStructure.Minutes = 59;
			}
			else
			{
				RTC_TimeStructure.Minutes--;
				HAL_RTC_SetTime(&hrtc, &RTC_TimeStructure, RTC_FORMAT_BIN);
			}
			break;
			
		case 3:
			if(RTC_TimeStructure.Hours == 0)
			{
				RTC_TimeStructure.Hours = 23;
			}
			else
			{
				RTC_TimeStructure.Hours--;
				HAL_RTC_SetTime(&hrtc, &RTC_TimeStructure, RTC_FORMAT_BIN);
			}
			break;
			
		case 4:
			if(RTC_DateStructure.WeekDay < 2)
			{
				RTC_DateStructure.WeekDay = 7;
			}
			else
			{
				RTC_DateStructure.WeekDay--;
				HAL_RTC_SetDate(&hrtc, &RTC_DateStructure, RTC_FORMAT_BIN);
			}
			break;
			
		case 5:
			if(RTC_DateStructure.Date < 2)
			{
				RTC_DateStructure.Date = 30;
			}
			else
			{
				RTC_DateStructure.Date--;
				HAL_RTC_SetDate(&hrtc, &RTC_DateStructure, RTC_FORMAT_BIN);
			}
			break;
			
		case 6:
			if(RTC_DateStructure.Month < 2)
			{
				RTC_DateStructure.Month = 12;
			}
			else
			{
				RTC_DateStructure.Month--;
				HAL_RTC_SetDate(&hrtc, &RTC_DateStructure, RTC_FORMAT_BIN);
			}
			break;
			
		case 7:
			RTC_DateStructure.Year--;
			HAL_RTC_SetDate(&hrtc, &RTC_DateStructure, RTC_FORMAT_BIN);
			break;
	}
}
	
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
