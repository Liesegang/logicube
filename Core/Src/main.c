/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "boards.h"
#include "digital_block.h"
#include <string.h>
#include <stdio.h>
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

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef handle_GPDMA2_Channel7;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;

IRDA_HandleTypeDef hirda1;
IRDA_HandleTypeDef hirda2;
IRDA_HandleTypeDef hirda3;
DMA_HandleTypeDef handle_GPDMA1_Channel7;

/* USER CODE BEGIN PV */
uint8_t buff_a[] = "d";
uint8_t buff_b[] = "d";

bool a = false;
bool b = false;
bool current = false;

enum Mode mode = MODE_TRUE;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_GPDMA1_Init(void);
static void MX_GPDMA2_Init(void);
static void MX_MEMORYMAP_Init(void);
static void MX_USART3_IRDA_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART1_IRDA_Init(void);
static void MX_USART2_IRDA_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_IRDA_RxCpltCallback(IRDA_HandleTypeDef *huart)
{
	if (huart->Instance == hirda1.Instance) {
		if (memcmp(buff_a, code_false, code_len) == 0) {
			 a = false;
		} else if(memcmp(buff_a, code_true, code_len) == 0) {
			 a = true;
		}
		HAL_IRDA_Receive_IT(&hirda1, buff_a, code_len);
	} else if (huart->Instance == hirda2.Instance) {
		if (memcmp(buff_b, code_false, code_len) == 0) {
			 b = false;
		} else if(memcmp(buff_a, code_true, code_len) == 0) {
			 b = true;
		}
		HAL_IRDA_Receive_IT(&hirda2, buff_b, code_len);
	}
}

void transmit(bool c) {
	if (c) {
		HAL_IRDA_Transmit(&hirda3, code_true, code_len, 100);
	} else {
		HAL_IRDA_Transmit(&hirda3, code_false, code_len, 100);
	}
}

void process(enum Mode mode, bool a, bool b) {
	bool result = false;

	switch(mode) {
		case MODE_TRUE:
			result = true;
			break;
		case MODE_FALSE:
			result = false;
			break;
		case MODE_AND:
			result = a && b;
			break;
		case MODE_OR:
			result = a || b;
			break;
		case MODE_NOTA:
			result = !a;
			break;
		case MODE_NOTB:
			result = !b;
			break;
		case MODE_NAND:
			result = !(a && b);
			break;
		case MODE_NOR:
			result = !(a || b);
			break;
		case MODE_XOR:
			result = a != b;
			break;
		case MODE_EQ:
			result = a == b;
			break;
		case MODE_A:
			result = a;
			break;
		case MODE_B:
			result = b;
			break;
		case MODE_END:
			result = false;
			break;
	}

	current = result;
	transmit(result);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == htim1.Instance) {
	  Button_Update();
  } else if(htim->Instance == htim2.Instance){
	  process(mode, a, b);
  }
}

void Button_ShortRelease(uint8_t btnCode) {
	switch(btnCode) {
		case 0:
			break;
		case 1:
			if (mode == 0) {
				mode = MODE_END - 1;
			} else {
				mode = mode - 1;
			}
			break;
		case 2:
			mode = 0;
			break;
		case 3:
			mode = (mode + 1) % MODE_END;
			break;
	}
}

void Button_LongPress(uint8_t btnCode) {
}

void Button_LongRelease(uint8_t btnCode) {
}

void write_mode(enum Mode mode) {
	switch(mode) {
		case MODE_TRUE:
			ST7735_WriteString(40, 30, "True ", Font_16x26, ST7735_WHITE, ST7735_BLACK);
			break;
		case MODE_FALSE:
			ST7735_WriteString(40, 30, "False", Font_16x26, ST7735_WHITE, ST7735_BLACK);
			break;
		case MODE_AND:
			ST7735_WriteString(40, 30, "And  ", Font_16x26, ST7735_WHITE, ST7735_BLACK);
			break;
		case MODE_OR:
			ST7735_WriteString(40, 30, "Or   ", Font_16x26, ST7735_WHITE, ST7735_BLACK);
			break;
		case MODE_NOTA:
			ST7735_WriteString(40, 30, "Not A", Font_16x26, ST7735_WHITE, ST7735_BLACK);
			break;
		case MODE_NOTB:
			ST7735_WriteString(40, 30, "Not B", Font_16x26, ST7735_WHITE, ST7735_BLACK);
			break;
		case MODE_NAND:
			ST7735_WriteString(40, 30, "Nand ", Font_16x26, ST7735_WHITE, ST7735_BLACK);
			break;
		case MODE_NOR:
			ST7735_WriteString(40, 30, "Nor  ", Font_16x26, ST7735_WHITE, ST7735_BLACK);
			break;
		case MODE_XOR:
			ST7735_WriteString(40, 30, "Xor  ", Font_16x26, ST7735_WHITE, ST7735_BLACK);
			break;
		case MODE_EQ:
			ST7735_WriteString(40, 30, "Eq   ", Font_16x26, ST7735_WHITE, ST7735_BLACK);
			break;
		case MODE_A:
			ST7735_WriteString(40, 30, "A    ", Font_16x26, ST7735_WHITE, ST7735_BLACK);
			break;
		case MODE_B:
			ST7735_WriteString(40, 30, "B    ", Font_16x26, ST7735_WHITE, ST7735_BLACK);
			break;
		case MODE_END:
			break;
	}
}

uint8_t ReadSwitch(const GPIO_TypeDef *port, const uint32_t pin) {
	return !HAL_GPIO_ReadPin(port, pin);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	setbuf(stdout, NULL);
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_GPDMA1_Init();
  MX_GPDMA2_Init();
  MX_MEMORYMAP_Init();
  MX_USART3_IRDA_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USART1_IRDA_Init();
  MX_USART2_IRDA_Init();
  /* USER CODE BEGIN 2 */
	ST7735_Init();
	ST7735_InvertColors(false);
  //
  btn_instance_t btn_inst[4] = {
      {
          {0, 0, 0},
          0,
          0,
          0,
          BUTTON_RIGHT_GPIO,
          BUTTON_RIGHT_PIN,
      },
      {
          {0, 0, 0},
          0,
          0,
          0,
          BUTTON_UP_GPIO,
          BUTTON_UP_PIN,
      },
      {
          {0, 0, 0},
          0,
          0,
          0,
          BUTTON_LEFT_GPIO,
          BUTTON_LEFT_PIN,
      },
      {
          {0, 0, 0},
          0,
          0,
          0,
          BUTTON_DOWN_GPIO,
          BUTTON_DOWN_PIN,
      },
  };

  btn_init_t btn_init = {
      0,
      0,
      10,
      30,
      0,
      (port_read_cb_t)ReadSwitch,
      Button_ShortRelease,
      Button_LongRelease,
      Button_LongPress,
  };

  Button_Init(&btn_init, btn_inst, 4);
  HAL_TIM_Base_Start_IT(&htim1);	// Timer for button
  HAL_TIM_Base_Start_IT(&htim2);	// Timer for IR transmit
  ST7735_FillScreenFast(ST7735_BLACK);
  HAL_GPIO_WritePin(LED_GPIO, LED_PIN, RESET);
  HAL_IRDA_Receive_IT(&hirda1, buff_a, code_len);
  HAL_IRDA_Receive_IT(&hirda2, buff_b, code_len);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    if (a) {
//    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, SET);
    	ST7735_WriteString(5, 5, "1", Font_16x26, ST7735_WHITE, ST7735_BLACK);
    } else {
//    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, RESET);
    	ST7735_WriteString(5, 5, "0", Font_16x26, ST7735_WHITE, ST7735_BLACK);
    }

    if (b) {
//    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, SET);
    	ST7735_WriteString(5, 49, "1", Font_16x26, ST7735_WHITE, ST7735_BLACK);
    } else {
//    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, RESET);
    	ST7735_WriteString(5, 49, "0", Font_16x26, ST7735_WHITE, ST7735_BLACK);
    }

    if (current) {
//    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, SET);
    	ST7735_WriteString(139, 30, "1", Font_16x26, ST7735_WHITE, ST7735_BLACK);
    } else {
//    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, RESET);
    	ST7735_WriteString(139, 30, "0", Font_16x26, ST7735_WHITE, ST7735_BLACK);
    }
    write_mode(mode);
    printf("%d, %d\r\n", mode, current);
    HAL_Delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_CSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV2;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.CSIState = RCC_CSI_ON;
  RCC_OscInitStruct.CSICalibrationValue = RCC_CSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_CSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 32;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPDMA1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPDMA1_Init(void)
{

  /* USER CODE BEGIN GPDMA1_Init 0 */

  /* USER CODE END GPDMA1_Init 0 */

  /* Peripheral clock enable */
  __HAL_RCC_GPDMA1_CLK_ENABLE();

  /* GPDMA1 interrupt Init */
    HAL_NVIC_SetPriority(GPDMA1_Channel7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel7_IRQn);

  /* USER CODE BEGIN GPDMA1_Init 1 */

  /* USER CODE END GPDMA1_Init 1 */
  /* USER CODE BEGIN GPDMA1_Init 2 */

  /* USER CODE END GPDMA1_Init 2 */

}

/**
  * @brief GPDMA2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPDMA2_Init(void)
{

  /* USER CODE BEGIN GPDMA2_Init 0 */

  /* USER CODE END GPDMA2_Init 0 */

  /* Peripheral clock enable */
  __HAL_RCC_GPDMA2_CLK_ENABLE();

  /* GPDMA2 interrupt Init */
    HAL_NVIC_SetPriority(GPDMA2_Channel7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA2_Channel7_IRQn);

  /* USER CODE BEGIN GPDMA2_Init 1 */

  /* USER CODE END GPDMA2_Init 1 */
  /* USER CODE BEGIN GPDMA2_Init 2 */

  /* USER CODE END GPDMA2_Init 2 */

}

/**
  * @brief MEMORYMAP Initialization Function
  * @param None
  * @retval None
  */
static void MX_MEMORYMAP_Init(void)
{

  /* USER CODE BEGIN MEMORYMAP_Init 0 */

  /* USER CODE END MEMORYMAP_Init 0 */

  /* USER CODE BEGIN MEMORYMAP_Init 1 */

  /* USER CODE END MEMORYMAP_Init 1 */
  /* USER CODE BEGIN MEMORYMAP_Init 2 */

  /* USER CODE END MEMORYMAP_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x7;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  hspi1.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
  hspi1.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 32000;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 10;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 32000;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 100;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_IRDA_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  hirda1.Instance = USART1;
  hirda1.Init.BaudRate = 2400;
  hirda1.Init.WordLength = IRDA_WORDLENGTH_8B;
  hirda1.Init.Parity = IRDA_PARITY_NONE;
  hirda1.Init.Mode = IRDA_MODE_TX_RX;
  hirda1.Init.Prescaler = 10;
  hirda1.Init.PowerMode = IRDA_POWERMODE_NORMAL;
  hirda1.Init.ClockPrescaler = IRDA_PRESCALER_DIV1;
  if (HAL_IRDA_Init(&hirda1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_IRDA_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  hirda2.Instance = USART2;
  hirda2.Init.BaudRate = 2400;
  hirda2.Init.WordLength = IRDA_WORDLENGTH_8B;
  hirda2.Init.Parity = IRDA_PARITY_NONE;
  hirda2.Init.Mode = IRDA_MODE_TX_RX;
  hirda2.Init.Prescaler = 10;
  hirda2.Init.PowerMode = IRDA_POWERMODE_NORMAL;
  hirda2.Init.ClockPrescaler = IRDA_PRESCALER_DIV1;
  if (HAL_IRDA_Init(&hirda2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_IRDA_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  hirda3.Instance = USART3;
  hirda3.Init.BaudRate = 2400;
  hirda3.Init.WordLength = IRDA_WORDLENGTH_8B;
  hirda3.Init.Parity = IRDA_PARITY_NONE;
  hirda3.Init.Mode = IRDA_MODE_TX_RX;
  hirda3.Init.Prescaler = 10;
  hirda3.Init.PowerMode = IRDA_POWERMODE_NORMAL;
  hirda3.Init.ClockPrescaler = IRDA_PRESCALER_DIV1;
  if (HAL_IRDA_Init(&hirda3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */
  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 PC3 PC10 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_3|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PC0 PC1 PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
int _write(int file, char *ptr, int len)
{
//  HAL_UART_Transmit(&huart2,(uint8_t *)ptr,len,10);
  return len;
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
  __disable_irq();
  while (1) {
  }
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
  /* User can add his own implementation to report the file name and line
     number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
