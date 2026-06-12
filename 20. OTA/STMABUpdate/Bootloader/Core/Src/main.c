/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Bootloader with I2C 2004 LCD
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "i2c.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN PTD */
typedef void (*pFunction)(void);
/* USER CODE END PTD */

/* USER CODE BEGIN PD */
#define APPLICATION_A_ADDRESS 0x08008000
#define APPLICATION_B_ADDRESS 0x08014000
#define BOOT_FLAG_ADDR BKP->DR1

#define BOOT_FLAG_A 0x01
#define BOOT_FLAG_B 0x02

#define LCD_ADDR (0x27 << 1)
/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void JumpToApplication(uint32_t appAddress);
void ConfigureBackupDomain(void);

/* USER CODE BEGIN PFP */
void LCD_SendInternal(uint8_t data, uint8_t flags);
void LCD_SendCmd(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_Init(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_Print(char *str);
void LCD_Clear(void);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

void LCD_SendInternal(uint8_t data, uint8_t flags)
{
    uint8_t up = data & 0xF0;
    uint8_t lo = (data << 4) & 0xF0;

    uint8_t data_arr[4];

    data_arr[0] = up | flags | 0x0C;
    data_arr[1] = up | flags | 0x08;
    data_arr[2] = lo | flags | 0x0C;
    data_arr[3] = lo | flags | 0x08;

    HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, data_arr, 4, HAL_MAX_DELAY);
}

void LCD_SendCmd(uint8_t cmd)
{
    LCD_SendInternal(cmd, 0x00);
}

void LCD_SendData(uint8_t data)
{
    LCD_SendInternal(data, 0x01);
}

void LCD_Init(void)
{
    HAL_Delay(50);

    LCD_SendCmd(0x30);
    HAL_Delay(5);
    LCD_SendCmd(0x30);
    HAL_Delay(1);
    LCD_SendCmd(0x30);
    HAL_Delay(10);

    LCD_SendCmd(0x20);
    HAL_Delay(10);

    LCD_SendCmd(0x28);
    HAL_Delay(1);
    LCD_SendCmd(0x08);
    HAL_Delay(1);
    LCD_SendCmd(0x01);
    HAL_Delay(2);
    LCD_SendCmd(0x06);
    HAL_Delay(1);
    LCD_SendCmd(0x0C);
}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t address;

    switch (row)
    {
        case 0:
            address = 0x80 + col;
            break;
        case 1:
            address = 0xC0 + col;
            break;
        case 2:
            address = 0x94 + col;
            break;
        case 3:
            address = 0xD4 + col;
            break;
        default:
            address = 0x80 + col;
            break;
    }

    LCD_SendCmd(address);
}

void LCD_Print(char *str)
{
    while (*str)
    {
        LCD_SendData((uint8_t)(*str++));
    }
}

void LCD_Clear(void)
{
    LCD_SendCmd(0x01);
    HAL_Delay(2);
}

/* USER CODE END 0 */

int main(void)
{
  HAL_Init();

  SystemClock_Config();
  ConfigureBackupDomain();

  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();

  /* USER CODE BEGIN 2 */

  LCD_Init();
  LCD_Clear();

  LCD_SetCursor(0, 0);
  LCD_Print("Bootloader Init");

  LCD_SetCursor(1, 0);
  LCD_Print("LCD Init OK");

  HAL_Delay(1000);

  uint32_t boot_flag = BOOT_FLAG_ADDR;
  uint32_t appAddress = APPLICATION_A_ADDRESS;

  LCD_Clear();
  LCD_SetCursor(0, 0);
  LCD_Print("Check Boot Flag");

  if (boot_flag != BOOT_FLAG_A && boot_flag != BOOT_FLAG_B)
  {
      boot_flag = BOOT_FLAG_A;
      BOOT_FLAG_ADDR = boot_flag;

      LCD_SetCursor(1, 0);
      LCD_Print("Flag Invalid");

      LCD_SetCursor(2, 0);
      LCD_Print("Set Flag: A");
  }
  else if (boot_flag == BOOT_FLAG_A)
  {
      LCD_SetCursor(1, 0);
      LCD_Print("Flag State: A");
  }
  else if (boot_flag == BOOT_FLAG_B)
  {
      LCD_SetCursor(1, 0);
      LCD_Print("Flag State: B");
  }

  if (boot_flag == BOOT_FLAG_A)
  {
      appAddress = APPLICATION_A_ADDRESS;

      LCD_SetCursor(2, 0);
      LCD_Print("Boot Partition A");

      LCD_SetCursor(3, 0);
      LCD_Print("Jump: 0x08008000");

      for (int i = 0; i < 1; i++)
      {
          HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_SET);
          HAL_Delay(500);
          HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_RESET);
          HAL_Delay(500);
      }
  }
  else if (boot_flag == BOOT_FLAG_B)
  {
      appAddress = APPLICATION_B_ADDRESS;

      LCD_SetCursor(2, 0);
      LCD_Print("Boot Partition B");

      LCD_SetCursor(3, 0);
      LCD_Print("Jump: 0x08014000");

      for (int i = 0; i < 2; i++)
      {
          HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_SET);
          HAL_Delay(500);
          HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_RESET);
          HAL_Delay(500);
      }
  }

  HAL_Delay(2000);
  JumpToApplication(appAddress);

  /* USER CODE END 2 */

  while (1)
  {
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL8;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK |
                                RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 |
                                RCC_CLOCKTYPE_PCLK2;

  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV8;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void JumpToApplication(uint32_t appAddress)
{
  uint32_t appStack = *(__IO uint32_t*)appAddress;
  uint32_t appEntry = *(__IO uint32_t*)(appAddress + 4);
  pFunction appResetHandler = (pFunction)appEntry;

  __disable_irq();

  HAL_DeInit();

  __set_MSP(appStack);

  SCB->VTOR = appAddress;

  __enable_irq();

  appResetHandler();
}

void ConfigureBackupDomain(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    __HAL_RCC_LSE_CONFIG(RCC_LSE_ON);
    while (__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET)
    {
    }

    __HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSE);
    __HAL_RCC_RTC_ENABLE();

    __HAL_RCC_BKP_CLK_ENABLE();
}

/* USER CODE END 4 */

void Error_Handler(void)
{
  __disable_irq();

  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
