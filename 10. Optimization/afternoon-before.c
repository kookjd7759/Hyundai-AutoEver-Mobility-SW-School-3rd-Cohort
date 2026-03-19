/**********************************************************************************************************************
 * \file Cpu0_Main.c
 * \copyright Copyright (C) Infineon Technologies AG 2019
 *********************************************************************************************************************/
#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "IfxPort.h"
#include "IfxPort_PinMap.h"

#include "IfxScuEru.h"
#include "IfxSrc.h"
#include "_PinMap/IfxScu_PinMap.h"

#include "Platform_Types.h"

#include "Driver_Stm.h"
#include "Driver_Adc.h"
#include "GTM_TOM_PWM.h"

/* ===================== PIN DEFINES ===================== */
#define SCLK IfxPort_P00_0
#define RCLK IfxPort_P00_1
#define DIO  IfxPort_P00_2

#define LED_R IfxPort_P02_7
#define LED_G IfxPort_P10_5
#define LED_B IfxPort_P10_3

#define LED_D13 IfxPort_P10_2
#define LED_D12 IfxPort_P10_1

IfxCpu_syncEvent g_cpuSyncEvent = 0;

/* ===================== PROTOTYPES ===================== */
void initGPIO(void);
void initERU(void);
void LedOn(IfxPort_Pin pin);
void LedOff(IfxPort_Pin pin);
void LedToggle(IfxPort_Pin pin);
void fnd_display_unit(uint8_t value, uint8_t position);
void send(uint8_t X);
void send_port(uint8_t X, uint8_t port);

void AppTask1ms(void);
void AppTask10ms(void);
void AppTask100ms(void);
void AppTask1000ms(void);
void AppScheduling(void);

/* ===================== COUNTERS ===================== */
typedef struct
{
    uint32 u32nuCnt1ms;
    uint32 u32nuCnt10ms;
    uint32 u32nuCnt100ms;
    uint32 u32nuCnt1000ms;
} TestCntType;

volatile TestCntType stTestCnt;

#define LIGHT_DARK_ON   (2500u)
#define LIGHT_DARK_OFF  (2700u)

/* ===================== STATE MACHINE ===================== */
typedef enum { STATE_NORMAL, STATE_CRUISE, STATE_EMERGENCY } SystemState_t;

volatile SystemState_t g_CurrentState = STATE_NORMAL;

volatile uint8 g_Sw1Event = 0;
volatile uint8 g_Sw2Event = 0;

uint16_t g_CurrentSpeed = 0;
uint16_t g_TargetSpeed  = 70;
uint32_t g_LightLevel   = 0;

/* ===================== DISPLAY BUFFER (4 digits) ===================== */
static volatile uint8 g_FndDigits[4] = {0, 0, 0, 0};

static volatile uint8 g_DisplayMode = 0;

/* ===================== DIAG EVENT LOGGER ===================== */
#define DIAG_LOG_SIZE   (16u)

typedef enum
{
    EVT_NONE = 0,
    EVT_STATE_CHANGE,
    EVT_OVERSPEED,
    EVT_LIGHT_DARK_ON,
    EVT_LIGHT_DARK_OFF,
    EVT_SW1,
    EVT_SW2
} DiagEventId_t;

typedef struct
{
    uint8  id;
    uint8  aux;
    uint16 data;
} DiagEvent_t;

static volatile DiagEvent_t g_DiagLog[DIAG_LOG_SIZE];
static volatile uint8       g_DiagWrIdx = 0;
static volatile uint8       g_DiagCrc8  = 0;

/* ===================== HELPERS ===================== */
static void  DiagLog_Push(uint8 id, uint8 aux, uint16 data);
static uint8 DiagLog_ComputeCrc8(void);
static uint8 Crc8_Step(uint8 crc, uint8 inByte);

static uint8 To7SegHex_IfElse(uint8 nibble);

static void  Display_SetSpeedDecimal(uint16 speed);
static void  Display_SetCrcState(uint8 crc, SystemState_t st);

/* ===================== INTERRUPTS ===================== */
IFX_INTERRUPT(ISR0_cruise, 0, 0x10);
void ISR0_cruise(void)
{
    g_Sw1Event = 1;
}

IFX_INTERRUPT(ISR1_emergency, 0, 0x11);
void ISR1_emergency(void)
{
    g_Sw2Event = 1;
}

/* ===================== MAIN ===================== */
int core0_main(void)
{
    IfxCpu_enableInterrupts();

    /* !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!! */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    initGPIO();
    initERU();
    Driver_Stm_Init();
    Driver_Adc_Init();
    initGtmTomPwm();

    Display_SetSpeedDecimal(g_CurrentSpeed);

    while(1)
    {
        AppScheduling();
    }

    return (1);
}

/* ===================== GPIO HELPERS ===================== */
void LedOn(IfxPort_Pin pin)
{
    IfxPort_setPinHigh(pin.port, pin.pinIndex);
}

void LedOff(IfxPort_Pin pin)
{
    IfxPort_setPinLow(pin.port, pin.pinIndex);
}

void LedToggle(IfxPort_Pin pin)
{
    IfxPort_togglePin(pin.port, pin.pinIndex);
}

/* ===================== FND OUTPUT ===================== */
void fnd_display_unit(uint8_t value, uint8_t position)
{
    uint8 v = (uint8)(value & 0x0Fu);

    uint8 content = To7SegHex_IfElse(v);

    send_port(content, position);
}

/* ===================== TASKS ===================== */
void AppTask1ms(void)
{
    static uint8 position = 0;

    switch(position)
    {
        case 0: fnd_display_unit(g_FndDigits[0], 0x1); break;
        case 1: fnd_display_unit(g_FndDigits[1], 0x2); break;
        case 2: fnd_display_unit(g_FndDigits[2], 0x4); break;
        case 3: fnd_display_unit(g_FndDigits[3], 0x8); break;
        default: break;
    }

    position = (uint8)((position + 1u) % 4u);

    stTestCnt.u32nuCnt1ms++;
}

void AppTask10ms(void)
{
    uint8 sw1 = g_Sw1Event;
    uint8 sw2 = g_Sw2Event;

    if (sw1 != 0u)
    {
        DiagLog_Push((uint8)EVT_SW1, (uint8)g_CurrentState, (uint16)g_CurrentSpeed);
    }
    if (sw2 != 0u)
    {
        DiagLog_Push((uint8)EVT_SW2, (uint8)g_CurrentState, (uint16)g_CurrentSpeed);
    }

    if ((sw2 != 0u) && (g_CurrentState != STATE_EMERGENCY))
    {
        g_CurrentState = STATE_EMERGENCY;
        DiagLog_Push((uint8)EVT_STATE_CHANGE, (uint8)g_CurrentState, (uint16)g_CurrentSpeed);
        sw2 = 0u;
    }

    switch (g_CurrentState)
    {
        case STATE_NORMAL:
            if (sw1 != 0u)
            {
                g_CurrentState = STATE_CRUISE;
                DiagLog_Push((uint8)EVT_STATE_CHANGE, (uint8)g_CurrentState, (uint16)g_CurrentSpeed);
            }
            LedOff(LED_R); LedOn(LED_G); LedOff(LED_B);
            break;

        case STATE_CRUISE:
            if (sw1 != 0u)
            {
                g_CurrentState = STATE_NORMAL;
                DiagLog_Push((uint8)EVT_STATE_CHANGE, (uint8)g_CurrentState, (uint16)g_CurrentSpeed);
            }
            g_CurrentSpeed = g_TargetSpeed;
            LedOff(LED_R); LedOff(LED_G); LedOn(LED_B);
            break;

        case STATE_EMERGENCY:
            if (sw2 != 0u)
            {
                g_CurrentState = STATE_NORMAL;
                DiagLog_Push((uint8)EVT_STATE_CHANGE, (uint8)g_CurrentState, (uint16)g_CurrentSpeed);
            }
            g_CurrentSpeed = 0u;
            LedOn(LED_R); LedOff(LED_G); LedOff(LED_B);
            break;

        default:
            break;
    }

    g_Sw1Event = 0u;
    g_Sw2Event = 0u;

    if (g_CurrentSpeed > 120u)
    {
        DiagLog_Push((uint8)EVT_OVERSPEED, (uint8)g_CurrentState, (uint16)g_CurrentSpeed);
    }

    stTestCnt.u32nuCnt10ms++;
}

void AppTask100ms(void)
{
    uint32_t potVal = Driver_Adc0_DataObtain(7);
    uint32_t ldrVal = Driver_Adc0_DataObtain(6);

    if (g_CurrentState == STATE_NORMAL)
    {
        if (g_CurrentState == STATE_NORMAL)
        {
            g_CurrentSpeed = (uint16)((potVal * 200u) / 4095u);
        }
    }

    if (IfxPort_getPinState(LED_D13.port, LED_D13.pinIndex) == FALSE)
    {
        if (ldrVal < LIGHT_DARK_ON)
        {
            LedOn(LED_D13);
            DiagLog_Push((uint8)EVT_LIGHT_DARK_ON, (uint8)g_CurrentState, (uint16)ldrVal);
        }
    }
    else
    {
        if (ldrVal > LIGHT_DARK_OFF)
        {
            LedOff(LED_D13);
            DiagLog_Push((uint8)EVT_LIGHT_DARK_OFF, (uint8)g_CurrentState, (uint16)ldrVal);
        }
    }

    if (g_CurrentState == STATE_EMERGENCY)
    {
        LedToggle(LED_D12);
        makeSound(12);
    }
    else
    {
        if (g_CurrentSpeed > 120u)
        {
            LedToggle(LED_D12);
            makeSound(7);
        }
        else
        {
            LedOff(LED_D12);
            makeSound(14);
        }
    }

    if (g_DisplayMode == 0u)
    {
        Display_SetSpeedDecimal(g_CurrentSpeed);
    }

    stTestCnt.u32nuCnt100ms++;
}

void AppTask1000ms(void)
{
    g_DiagCrc8 = DiagLog_ComputeCrc8();

    g_DisplayMode ^= 1u;

    if (g_DisplayMode != 0u)
    {
        Display_SetCrcState(g_DiagCrc8, g_CurrentState);
    }
    else
    {
        Display_SetSpeedDecimal(g_CurrentSpeed);
    }

    stTestCnt.u32nuCnt1000ms++;
}

/* ===================== SCHEDULING ===================== */
void AppScheduling(void)
{
    if (stSchedulingInfo.u8nuScheduling1msFlag == 1u)
    {
        stSchedulingInfo.u8nuScheduling1msFlag = 0u;
        AppTask1ms();

        if (stSchedulingInfo.u8nuScheduling10msFlag == 1u)
        {
            stSchedulingInfo.u8nuScheduling10msFlag = 0u;
            AppTask10ms();
        }

        if (stSchedulingInfo.u8nuScheduling100msFlag == 1u)
        {
            stSchedulingInfo.u8nuScheduling100msFlag = 0u;
            AppTask100ms();
        }

        if (stSchedulingInfo.u8nuScheduling1000msFlag == 1u)
        {
            stSchedulingInfo.u8nuScheduling1000msFlag = 0u;
            AppTask1000ms();
        }
    }
}

/* ===================== SHIFT REGISTER OUTPUT ===================== */
void send(uint8_t X)
{
    for (int i = 8; i >= 1; i--)
    {
        if ((X & 0x80u) != 0u)
        {
            IfxPort_setPinHigh(DIO.port, DIO.pinIndex);
        }
        else
        {
            IfxPort_setPinLow(DIO.port, DIO.pinIndex);
        }

        X <<= 1u;
        IfxPort_setPinLow(SCLK.port, SCLK.pinIndex);
        IfxPort_setPinHigh(SCLK.port, SCLK.pinIndex);
    }
}

void send_port(uint8_t X, uint8_t port)
{
    send(X);
    send(port);
    IfxPort_setPinLow(RCLK.port, RCLK.pinIndex);
    IfxPort_setPinHigh(RCLK.port, RCLK.pinIndex);
}

/* ===================== DISPLAY HELPERS ===================== */
static void Display_SetSpeedDecimal(uint16 speed)
{
    uint16 v = speed;
    if (v > 1999u) { v = 1999u; }

    g_FndDigits[0] = (uint8)(v % 10u);
    g_FndDigits[1] = (uint8)((v / 10u) % 10u);
    g_FndDigits[2] = (uint8)((v / 100u) % 10u);
    g_FndDigits[3] = (uint8)((v / 1000u) % 10u);
}

static void Display_SetCrcState(uint8 crc, SystemState_t st)
{
    uint8 hi = (uint8)((crc >> 4) & 0x0Fu);
    uint8 lo = (uint8)(crc & 0x0Fu);

    g_FndDigits[3] = hi;
    g_FndDigits[2] = lo;
    g_FndDigits[1] = (uint8)st;
    g_FndDigits[0] = 0u;
}

/* ===================== DIAG LOGGER ===================== */
static void DiagLog_Push(uint8 id, uint8 aux, uint16 data)
{
    uint8 idx = g_DiagWrIdx;

    g_DiagLog[idx].id   = id;
    g_DiagLog[idx].aux  = aux;
    g_DiagLog[idx].data = data;

    idx++;
    if (idx >= DIAG_LOG_SIZE)
    {
        idx = 0u;
    }
    g_DiagWrIdx = idx;
}

static uint8 DiagLog_ComputeCrc8(void)
{
    uint8 crc = 0xFFu;

    for (uint8 i = 0u; i < DIAG_LOG_SIZE; i++)
    {
        uint8 b0 = g_DiagLog[i].id;
        uint8 b1 = g_DiagLog[i].aux;
        uint8 b2 = (uint8)(g_DiagLog[i].data & 0xFFu);
        uint8 b3 = (uint8)((g_DiagLog[i].data >> 8) & 0xFFu);

        crc = Crc8_Step(crc, b0);
        crc = Crc8_Step(crc, b1);
        crc = Crc8_Step(crc, b2);
        crc = Crc8_Step(crc, b3);
    }

    return crc;
}

static uint8 Crc8_Step(uint8 crc, uint8 inByte)
{
    for (uint8 bit = 0u; bit < 8u; bit++)
    {
        uint8 mix = (uint8)((crc ^ inByte) & 0x01u);
        crc >>= 1u;

        if (mix != 0u)
        {
            crc ^= 0x8Cu; /* CRC-8 Dallas/Maxim (reflected) */
        }

        inByte >>= 1u;
    }

    return crc;
}

/* ===================== 7SEG HELPER ===================== */
static uint8 To7SegHex_IfElse(uint8 nibble)
{
    nibble &= 0x0Fu;

    if (nibble == 0u) return 0xC0;
    else if (nibble == 1u) return 0xF9;
    else if (nibble == 2u) return 0xA4;
    else if (nibble == 3u) return 0xB0;
    else if (nibble == 4u) return 0x99;
    else if (nibble == 5u) return 0x92;
    else if (nibble == 6u) return 0x82;
    else if (nibble == 7u) return 0xF8;
    else if (nibble == 8u) return 0x80;
    else if (nibble == 9u) return 0x90;
    else if (nibble == 10u) return 0x88; /* A */
    else if (nibble == 11u) return 0x83; /* b */
    else if (nibble == 12u) return 0xC6; /* C */
    else if (nibble == 13u) return 0xA1; /* d */
    else if (nibble == 14u) return 0x86; /* E */
    else return 0x8E;                    /* F */
}

/* ===================== INIT GPIO ===================== */
void initGPIO(void)
{
    IfxPort_setPinModeOutput(IfxPort_P10_2.port,IfxPort_P10_2.pinIndex,
                             IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(IfxPort_P10_1.port, IfxPort_P10_1.pinIndex,
                             IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);

    LedOff(LED_D13);
    LedOff(LED_D12);

    IfxPort_setPinModeOutput(LED_R.port, LED_R.pinIndex,
                             IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(LED_G.port, LED_G.pinIndex,
                             IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(LED_B.port, LED_B.pinIndex,
                             IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);

    LedOff(LED_R);
    LedOff(LED_G);
    LedOff(LED_B);

    IfxPort_setPinModeOutput(SCLK.port, SCLK.pinIndex, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(RCLK.port, RCLK.pinIndex, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(DIO.port,  DIO.pinIndex,  IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
}

/* ===================== INIT ERU ===================== */
void initERU(void)
{
    IfxPort_setPinModeInput(IfxPort_P02_0.port, IfxPort_P02_0.pinIndex, IfxPort_InputMode_pullUp);

    IfxScuEru_selectExternalInput(IfxScuEru_InputChannel_3, IfxScuEru_ExternalInputSelection_2);

    IfxScuEru_enableFallingEdgeDetection(IfxScuEru_InputChannel_3);
    IfxScuEru_disableRisingEdgeDetection(IfxScuEru_InputChannel_3);

    IfxScuEru_enableTriggerPulse(IfxScuEru_InputChannel_3);

    IfxScuEru_connectTrigger(IfxScuEru_InputChannel_3, IfxScuEru_InputNodePointer_0);

    IfxScuEru_setInterruptGatingPattern(IfxScuEru_OutputChannel_0,
                                        IfxScuEru_InterruptGatingPattern_alwaysActive);
    IfxScuEru_setFlagPatternDetection(IfxScuEru_OutputChannel_0, IfxScuEru_InputChannel_3, TRUE);

    IfxScuEru_clearEventFlag(IfxScuEru_InputChannel_3);

    IfxSrc_init(&SRC_SCU_SCU_ERU0, IfxSrc_Tos_cpu0, 0x10u);
    IfxSrc_enable(&SRC_SCU_SCU_ERU0);

    IfxPort_setPinModeInput(IfxPort_P02_1.port, IfxPort_P02_1.pinIndex, IfxPort_InputMode_pullUp);

    IfxScuEru_selectExternalInput(IfxScuEru_InputChannel_2, IfxScuEru_ExternalInputSelection_1);

    IfxScuEru_enableFallingEdgeDetection(IfxScuEru_InputChannel_2);
    IfxScuEru_disableRisingEdgeDetection(IfxScuEru_InputChannel_2);

    IfxScuEru_enableTriggerPulse(IfxScuEru_InputChannel_2);
    IfxScuEru_connectTrigger(IfxScuEru_InputChannel_2, IfxScuEru_InputNodePointer_1);

    IfxScuEru_setInterruptGatingPattern(IfxScuEru_OutputChannel_1,
                                        IfxScuEru_InterruptGatingPattern_alwaysActive);
    IfxScuEru_setFlagPatternDetection(IfxScuEru_OutputChannel_1, IfxScuEru_InputChannel_2, TRUE);

    IfxScuEru_clearEventFlag(IfxScuEru_InputChannel_2);

    IfxSrc_init(&SRC_SCU_SCU_ERU1, IfxSrc_Tos_cpu0, 0x11u);
    IfxSrc_enable(&SRC_SCU_SCU_ERU1);
}
