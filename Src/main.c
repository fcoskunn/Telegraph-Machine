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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum kind {
    ALPHA, NUM
} Kind;

typedef struct mtree {
    char value;
    struct mtree *dot;
    struct mtree *bar;
} MTree;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

MTree *root;
uint16_t lcd_index = 1;

void make_tree(void);
void drop_tree(void);
void encode_out(const char *s);
void decode_out(const char *s);

static const char *alpha[] = {
    ".-",   //A
    "-...", //B
    "-.-.", //C
    "-..",  //D
    ".",    //E
    "..-.", //F
    "--.",  //G
    "....", //H
    "..",   //I
    ".---", //J
    "-.-",  //K
    ".-..", //L
    "--",   //M
    "-.",   //N
    "---",  //O
    ".--.", //P
    "--.-", //Q
    ".-.",  //R
    "...",  //S
    "-",    //T
    "..-",  //U
    "...-", //V
    ".--",  //W
    "-..-", //X
    "-.--", //Y
    "--..", //Z
};
static const char *num[] = {
    "-----", //0
    ".----", //1
    "..---", //2
    "...--", //3
    "....-", //4
    ".....", //5
    "-....", //6
    "--...", //7
    "---..", //8
    "----.", //9
};
static const char **table[] = { alpha, num };

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

#include "LCD.h"

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void send_num_to_uart(uint8_t num){
	uint8_t blank = ' ';
	uint8_t one = '1';
	uint8_t zero = '0';
	uint8_t sum = '0' + num;
	HAL_UART_Transmit(&huart1, &blank, sizeof(blank), 500);	
	if (num == 10) {
			HAL_UART_Transmit(&huart1, &one, sizeof(one), 500);
			HAL_UART_Transmit(&huart1, &zero, sizeof(zero), 500);
	}	else {
			HAL_UART_Transmit(&huart1, &sum, sizeof(sum), 500);
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

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
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	uint8_t number = 0;
	uint8_t uart_input= 0;
	make_tree();
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	
	lcd_init(_LCD_4BIT, _LCD_FONT_5x8, _LCD_2LINE);
  char ch[210];
  while (1)

  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);

		HAL_UART_Receive(&huart1, &uart_input, sizeof(uart_input), 500);
		if ((uart_input >= 'A' && uart_input <= 'Z') || uart_input == ' ' || (uart_input >= '0' && uart_input <= '9')  ){
			HAL_UART_Transmit(&huart1, &uart_input, sizeof(uart_input), 500);
			char inp[1];
			inp[0] = uart_input;
			encode_out(inp);
			lcd_print(1, lcd_index, " ");
			lcd_index++;	
		}
		
		if (HAL_GPIO_ReadPin(GPIOB, eoc_Pin)) {
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
				decode_out(ch);
				strcpy(ch, "");
				lcd_print(2, 1, "                ");
		}
		

		if (HAL_GPIO_ReadPin(GPIOB, blank_Pin)) {
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
				strcpy(ch, "/");
				decode_out(ch);
				lcd_print(2, 1, "                ");
				strcpy(ch, "");

		}
				
		if (HAL_GPIO_ReadPin(GPIOB, long_Pin)) {
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
				char c = '-';
				strcat(ch, &c);		
		}
		
		if (HAL_GPIO_ReadPin(clear_lcd_GPIO_Port, clear_lcd_Pin)) {
				strcpy(ch, "");
				lcd_clear();
				lcd_index = 1;
		}
		
		
		if (HAL_GPIO_ReadPin(GPIOB, short_Pin)) {
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
				char c = '.';
				strcat(ch, &c);
		}
		
		lcd_print(2, 1, ch);
		
		HAL_Delay(75);
		
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);		
		uart_input = 0;

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

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 799;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 99;
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
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LCD_RS_Pin|LCD_EN_Pin|LCD_D4_Pin|LCD_D5_Pin 
                          |LCD_D6_Pin|LCD_D7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_SET);

  /*Configure GPIO pin : PC0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_RS_Pin LCD_EN_Pin LCD_D4_Pin LCD_D5_Pin 
                           LCD_D6_Pin LCD_D7_Pin */
  GPIO_InitStruct.Pin = LCD_RS_Pin|LCD_EN_Pin|LCD_D4_Pin|LCD_D5_Pin 
                          |LCD_D6_Pin|LCD_D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : eoc_Pin blank_Pin long_Pin short_Pin 
                           clear_lcd_Pin */
  GPIO_InitStruct.Pin = eoc_Pin|blank_Pin|long_Pin|short_Pin 
                          |clear_lcd_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 PB5 PB6 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void encode_out(const char *s){
    for(;;++s){
        char ch = *s;
        if(ch == '\0')
            break;
        if(isalpha(ch)){
            ch = toupper(ch);
            //fputs(table[ALPHA][ch - 'A'], stdout);//`-'A'` depend on the sequence of character code
					  const char* output;
					  output = table[ALPHA][ch - 'A'];
						for (int i = 0; i < strlen(output); i++){
								HAL_Delay(250);	
								if (output[i] == '.'){
									HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);	
									lcd_print(1, lcd_index, ".");
									lcd_index++;									
									HAL_Delay(100);		
									HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
								} else if (output[i] == '-'){
									HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
									lcd_print(1, lcd_index, "-");
									lcd_index++;		
									HAL_Delay(300);									
									HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
								}
						}
					
        } else if(isdigit(ch)){			
						ch = toupper(ch);
					  const char* output;
					  output = table[NUM][ch - '0'];
						for (int i = 0; i < strlen(output); i++){
								HAL_Delay(200);	
								if (output[i] == '.'){
									HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
									lcd_print(1, lcd_index, ".");
									lcd_index++;		
									HAL_Delay(300);		
									HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
								} else if (output[i] == '-'){
									HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);
									HAL_Delay(800);									
									lcd_print(1, lcd_index, "-");
									lcd_index++;		
									HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET);
								}
						}
					}
        else if(ch == ' '){
					lcd_print(1, lcd_index, "/");
					lcd_index++;	
				}	
        else{
					lcd_print(1, lcd_index, "#");
					lcd_index++;	
				}
            ;//invalid character => ignore
        //fputc(' ', stdout);
    }
    //fputc('\n', stdout);
}
static void decode_out_aux(MTree *tree, const char *s){
    if(tree == NULL) return;
    if(*s == '\0'){
				lcd_print(1, lcd_index, &(tree->value));
				lcd_index++;
		}
    else if(*s == '/'){
			char ch = ' ';
			lcd_print(1, lcd_index, &ch);
			lcd_index++;
		}
    else if(*s == '.')
        decode_out_aux(tree->dot, ++s);
    else if(*s == '-')
        decode_out_aux(tree->bar, ++s);
		
		if (lcd_index >= 16 )lcd_index = 1;

}
void decode_out(const char *s){
    char *p;
    while(*s){
        p = strchr(s, ' ');
        if(p){
            if(p-s != 0){
                char code[p-s+1];
                memcpy(code, s, p-s);
                code[p-s]='\0';
                decode_out_aux(root, code);
            }
            s = p + 1;
        } else {
            decode_out_aux(root, s);
            break;
        }
    }
}
static void insert_aux(MTree **tree, char ch, const char *s){
    if(*tree == NULL)
        *tree = calloc(1, sizeof(**tree));
    if(*s == '\0')
        (*tree)->value = ch;
    else if(*s == '.')
        insert_aux(&(*tree)->dot, ch, ++s);
    else if(*s == '-')
        insert_aux(&(*tree)->bar, ch, ++s);
}

static inline void insert(char ch, const char *s){
    if(*s == '.')
        insert_aux(&root->dot, ch, ++s);
    else if(*s == '-')
        insert_aux(&root->bar, ch, ++s);
}

void make_tree(void){
    root = calloc(1, sizeof(*root));
    //root->value = '/';//anything
    int i;
    for(i = 0; i < 26; ++i)
        insert('A'+i, table[ALPHA][i]);
    for(i = 0; i < 10; ++i)
        insert('0'+i, table[NUM][i]);
}
static void drop_tree_aux(MTree *root){
    if(root){
        drop_tree_aux(root->dot);
        drop_tree_aux(root->bar);
        free(root);
    }
}
void drop_tree(void){
    drop_tree_aux(root);
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
