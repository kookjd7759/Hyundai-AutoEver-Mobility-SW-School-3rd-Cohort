/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "can.h"
#include "usart.h"
#include "gpio.h"
#include "i2c.h"

/* USER CODE BEGIN Includes */
#include <string.h>
/* USER CODE END Includes */

/* USER CODE BEGIN PTD */
typedef void (*pFunction)(void);

typedef enum {
  FW_UPDATE_IDLE,
  FW_UPDATE_REQUESTED,
  FW_UPDATE_IN_PROGRESS,
  FW_UPDATE_COMPLETE
} FirmwareUpdateStat_t;

typedef struct {
  CAN_RxHeaderTypeDef rxHeader;
  uint8_t rxData[8];
} CAN_Message_t;
/* USER CODE END PTD */

/* USER CODE BEGIN PD */
#define APPLICATION_A_ADDRESS 0x08008000
#define APPLICATION_B_ADDRESS 0x08014000
#define BOOT_FLAG_ADDR BKP->DR1

#define NUMBER_OF_PAGES_IN_PARTITION 48

#define BOOT_FLAG_A 0x01
#define BOOT_FLAG_B 0x02

#define CAN_ID_FILE    0x70
#define CAN_ID_SIZE    0x71
#define CAN_ID_SEND    0x7A
#define CAN_ID_CONTROL 0x7B

#define MESSAGE_BUFFER_SIZE 512

#define LCD_ADDR (0x27 << 1)
/* USER CODE END PD */

/* USER CODE BEGIN PV */
CAN_TxHeaderTypeDef TxHeader;
uint8_t TxData[8] = {0x00,};
uint8_t command[8] = {0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00};
uint32_t TxMailbox;

FirmwareUpdateStat_t fwUpdateState = FW_UPDATE_IDLE;
volatile uint8_t fwUpdateRequested = 0;
volatile uint8_t fwUpdateComplete = 0;
uint32_t fwUpdateAddress = 0;
uint32_t fwUpdateSize = 0;
uint32_t fwUpdateReceivedBytes = 0;
uint8_t LD2Counter = 0;
uint8_t ledState = 0;

CAN_Message_t messageBuffer[MESSAGE_BUFFER_SIZE];
volatile uint16_t messageBufferHead = 0;
volatile uint16_t messageBufferTail = 0;

CAN_TxHeaderTypeDef txHeader;
uint8_t txData[8];
uint32_t txMailbox;
/* USER CODE END PV */

void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
void SetBootFlag(void);
void NVIC_Config(void);

void FirmwareUpdateStateMachine(void);
void StartFirmwareUpdate(void);
void EraseFlashMemory(void);
void SendInactivePartitionAddress(void);
void SendUpdateState(void);

uint8_t MessageBufferIsFull(void);
uint8_t MessageBufferIsEmpty(void);
void MessageBufferPut(CAN_RxHeaderTypeDef *rxHeader, uint8_t *rxData);
void MessageBufferGet(CAN_RxHeaderTypeDef *rxHeader, uint8_t *rxData);

void SendFirmwareStartedMessage(void);
void ProcessFirmwareSizeMessage(CAN_RxHeaderTypeDef *rxHeader, uint8_t *rxData);
void LD2Flip(void);
void Error_Handler(void);

/* LCD 함수 */
void LCD_SendInternal(uint8_t data, uint8_t flags);
void LCD_SendCmd(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_Init(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_Print(char *str);
void LCD_Clear(void);
void LCD_PrintStatus(char *line1, char *line2);
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

void LCD_PrintStatus(char *line1, char *line2)
{
    LCD_Clear();

    LCD_SetCursor(0, 0);
    LCD_Print("STM32F103RB OTA");

    LCD_SetCursor(1, 0);
    LCD_Print(line1);

    LCD_SetCursor(2, 0);
    LCD_Print(line2);

    LCD_SetCursor(3, 0);
    LCD_Print("CAN + I2C LCD");
}

/* USER CODE END 0 */

int main(void)
{
  HAL_Init();

  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  NVIC_Config();

  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_BKP_CLK_ENABLE();
  /* USER CODE END SysInit */

  MX_GPIO_Init();
  MX_CAN_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();

  /* USER CODE BEGIN 2 */

  LCD_Init();
  LCD_PrintStatus("System Booting", "LCD Init OK");
  HAL_Delay(2000);

  if (HAL_CAN_Start(&hcan) != HAL_OK)
  {
      LCD_PrintStatus("CAN Start Fail", "Error Handler");
      Error_Handler();
  }

  CAN_FilterTypeDef canFilterConfig;
  canFilterConfig.FilterActivation = CAN_FILTER_ENABLE;
  canFilterConfig.FilterBank = 0;
  canFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  canFilterConfig.FilterIdHigh = 0x0000;
  canFilterConfig.FilterIdLow = 0x0000;
  canFilterConfig.FilterMaskIdHigh = 0x0000;
  canFilterConfig.FilterMaskIdLow = 0x0000;
  canFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  canFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;

  if (HAL_CAN_ConfigFilter(&hcan, &canFilterConfig) != HAL_OK)
  {
      LCD_PrintStatus("CAN Filter Fail", "Error Handler");
      Error_Handler();
  }

  if (HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
  {
      LCD_PrintStatus("CAN Notify Fail", "Error Handler");
      Error_Handler();
  }

  SendFirmwareStartedMessage();

  TxHeader.DLC = 8;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.StdId = 0x80;
  LCD_PrintStatus("now sending", "CAN message!");
  /* USER CODE END 2 */

  while (1)
  {
    if (fwUpdateState == FW_UPDATE_IDLE)
    {
        if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK)
        {
            LCD_PrintStatus("CAN TX Fail", "Error Handler");
            Error_Handler();
        }

        TxData[0] += 0x11;
        TxData[1] += 0x12;
        TxData[2] += 0x13;

        HAL_Delay(100);

        LD2Counter++;
        if (LD2Counter > 100)
            LD2Flip();

    }

    else{
    	LCD_PrintStatus("now start", "Update!");
    }
    FirmwareUpdateStateMachine();
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

  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_AFIO_REMAP_SWJ_NOJTAG();
}

/* USER CODE BEGIN 4 */

void LD2Flip(void)
{
    if (ledState == 0)
    {
        ledState = 1;
        HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_SET);
    }
    else
    {
        ledState = 0;
        HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_RESET);
    }

    LD2Counter = 0;
}

void SetBootFlag(void)
{
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_BKP_CLK_ENABLE();

    uint32_t boot_flag = BOOT_FLAG_ADDR;

    if (boot_flag == BOOT_FLAG_A)
        BOOT_FLAG_ADDR = BOOT_FLAG_B;
    else
        BOOT_FLAG_ADDR = BOOT_FLAG_A;
}

void SendFirmwareStartedMessage(void)
{
    txHeader.StdId = CAN_ID_SEND;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.IDE = CAN_ID_STD;
    txHeader.DLC = 1;

    txData[0] = 0x01;

    if (HAL_CAN_AddTxMessage(&hcan, &txHeader, txData, &txMailbox) != HAL_OK)
    {
        Error_Handler();
    }

    while (HAL_CAN_IsTxMessagePending(&hcan, txMailbox))
    {
    }
}

void NVIC_Config(void)
{
    HAL_NVIC_SetPriority(USB_LP_CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
}

void ProcessFirmwareSizeMessage(CAN_RxHeaderTypeDef *rxHeader, uint8_t *rxData)
{
    if (rxHeader->DLC == 4)
    {
        fwUpdateSize = (rxData[0] << 24) |
                       (rxData[1] << 16) |
                       (rxData[2] << 8) |
                       rxData[3];

        LCD_PrintStatus("Size Received", "Update Ready");
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8];

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rxHeader, rxData) != HAL_OK)
    {
        Error_Handler();
        return;
    }

    if (rxHeader.StdId == CAN_ID_CONTROL && fwUpdateRequested == 0)
    {
        if (!memcmp(command, rxData, sizeof(command)))
        {
            fwUpdateRequested = 1;
            LCD_PrintStatus("Update Request", "Command OK");
        }
    }
    else if (fwUpdateRequested == 1 && rxHeader.StdId == CAN_ID_SIZE)
    {
        ProcessFirmwareSizeMessage(&rxHeader, rxData);
    }
    else if (fwUpdateRequested == 1 && rxHeader.StdId == CAN_ID_FILE)
    {
        MessageBufferPut(&rxHeader, rxData);
    }
}

void EraseFlashMemory(void)
{
    HAL_FLASH_Unlock();

    FLASH_EraseInitTypeDef eraseInitStruct;
    uint32_t pageError = 0;

    eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInitStruct.PageAddress = fwUpdateAddress;
    eraseInitStruct.NbPages = NUMBER_OF_PAGES_IN_PARTITION;

    if (HAL_FLASHEx_Erase(&eraseInitStruct, &pageError) != HAL_OK)
    {
        HAL_FLASH_Lock();
        fwUpdateState = FW_UPDATE_IDLE;
        LCD_PrintStatus("Flash Erase Fail", "System Reset");
        NVIC_SystemReset();
    }

    HAL_FLASH_Lock();
}

void StartFirmwareUpdate(void)
{
    if (BOOT_FLAG_ADDR == BOOT_FLAG_A)
        fwUpdateAddress = APPLICATION_B_ADDRESS;
    else
        fwUpdateAddress = APPLICATION_A_ADDRESS;

    fwUpdateReceivedBytes = 0;

    LCD_PrintStatus("Update Start", "Erase Flash");

    EraseFlashMemory();

    LCD_PrintStatus("Erase Complete", "Send Address");

    SendInactivePartitionAddress();
}

void SendInactivePartitionAddress(void)
{
    txHeader.StdId = CAN_ID_SEND;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.IDE = CAN_ID_STD;
    txHeader.DLC = 4;

    uint32_t targetAddress = fwUpdateAddress;

    txData[0] = (targetAddress >> 24) & 0xFF;
    txData[1] = (targetAddress >> 16) & 0xFF;
    txData[2] = (targetAddress >> 8) & 0xFF;
    txData[3] = targetAddress & 0xFF;

    if (HAL_CAN_AddTxMessage(&hcan, &txHeader, txData, &txMailbox) != HAL_OK)
    {
        Error_Handler();
    }

    while (HAL_CAN_IsTxMessagePending(&hcan, txMailbox))
    {
    }
}

void SendUpdateState(void)
{
    txHeader.StdId = CAN_ID_SEND;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.IDE = CAN_ID_STD;
    txHeader.DLC = 1;

    if (fwUpdateComplete == 1)
        txData[0] = 0x02;
    else
        txData[0] = 0x03;

    if (HAL_CAN_AddTxMessage(&hcan, &txHeader, txData, &txMailbox) != HAL_OK)
    {
        Error_Handler();
    }

    while (HAL_CAN_IsTxMessagePending(&hcan, txMailbox))
    {
    }
}

uint8_t MessageBufferIsFull(void)
{
    return ((messageBufferHead + 1) % MESSAGE_BUFFER_SIZE) == messageBufferTail;
}

uint8_t MessageBufferIsEmpty(void)
{
    return messageBufferHead == messageBufferTail;
}

void MessageBufferPut(CAN_RxHeaderTypeDef *rxHeader, uint8_t *rxData)
{
    if (!MessageBufferIsFull())
    {
        messageBuffer[messageBufferHead].rxHeader = *rxHeader;
        memcpy(messageBuffer[messageBufferHead].rxData, rxData, 8);
        messageBufferHead = (messageBufferHead + 1) % MESSAGE_BUFFER_SIZE;
    }
}

void MessageBufferGet(CAN_RxHeaderTypeDef *rxHeader, uint8_t *rxData)
{
    if (!MessageBufferIsEmpty())
    {
        *rxHeader = messageBuffer[messageBufferTail].rxHeader;
        memcpy(rxData, messageBuffer[messageBufferTail].rxData, 8);
        messageBufferTail = (messageBufferTail + 1) % MESSAGE_BUFFER_SIZE;
    }
}

void FirmwareUpdateStateMachine(void)
{
    switch (fwUpdateState)
    {
        case FW_UPDATE_IDLE:
            if (fwUpdateRequested)
            {
                StartFirmwareUpdate();
                fwUpdateState = FW_UPDATE_IN_PROGRESS;
                LCD_PrintStatus("Update Progress", "Receiving FW");
            }
            break;

        case FW_UPDATE_IN_PROGRESS:
        {
            CAN_RxHeaderTypeDef rxHeader;
            uint32_t currentAddress = fwUpdateAddress;
            uint8_t rxData[8];

            while (fwUpdateSize > fwUpdateReceivedBytes)
            {
                if (!MessageBufferIsEmpty())
                {
                    MessageBufferGet(&rxHeader, rxData);

                    uint8_t dataLength = rxHeader.DLC;

                    HAL_FLASH_Unlock();

                    for (uint8_t i = 0; i < dataLength; i += 2)
                    {
                        uint16_t data16 = rxData[i];

                        if (i + 1 < dataLength)
                        {
                            data16 |= rxData[i + 1] << 8;
                        }

                        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, currentAddress, data16) != HAL_OK)
                        {
                            HAL_FLASH_Lock();
                            fwUpdateRequested = 0;
                            fwUpdateState = FW_UPDATE_IDLE;
                            LCD_PrintStatus("Flash Write Fail", "Update Stop");
                            return;
                        }

                        currentAddress += 2;
                        fwUpdateReceivedBytes += (i + 1 < dataLength) ? 2 : 1;
                    }

                    HAL_FLASH_Lock();
                }
            }

            fwUpdateRequested = 0;

            if (fwUpdateReceivedBytes != fwUpdateSize)
            {
                fwUpdateComplete = 0;
                SendUpdateState();
                fwUpdateState = FW_UPDATE_IDLE;
                LCD_PrintStatus("Update Failed", "Size Mismatch");
            }
            else
            {
                fwUpdateComplete = 1;
                SendUpdateState();
                fwUpdateState = FW_UPDATE_COMPLETE;
                LCD_PrintStatus("Update Complete", "Reset Soon");
            }

            fwUpdateSize = 0;
            break;
        }

        case FW_UPDATE_COMPLETE:
            SetBootFlag();
            HAL_Delay(1000);
            NVIC_SystemReset();
            break;

        default:
            fwUpdateState = FW_UPDATE_IDLE;
            break;
    }
}

/* USER CODE END 4 */

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  __disable_irq();
  HAL_GPIO_WritePin(GPIOA, LD2_Pin, GPIO_PIN_SET);
  LCD_Clear();
  LCD_Print("Error!");
  HAL_Delay(5000);

  while (1)
  {
      SetBootFlag();
      NVIC_SystemReset();
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
