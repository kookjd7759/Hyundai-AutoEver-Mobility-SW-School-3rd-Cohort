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

/* CONSTANT FND VALUES */
static const uint8_t LED_0F[10] =
{
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90
};

static uint16 g_sw1Hold_ms = 0u;   /* double -> uint16 */

typedef struct
{
    uint32_t tick_ms;
    uint16_t hold_ms;
    uint8_t  state;
    uint8_t  type;
} Sw1Log;


/* MACROS FOR FIXED VALUE */
#define CRUISE_MIN_SPEED   (70u)
#define CRUISE_MAX_SPEED   (120u)
#define SPEED_STEP         (5u)
#define SW1_LONGPRESS_MS   (600u)
#define SW1_HOLD_MAX_MS    (5000u)

/* 1ms TASK OPTIMIZATION */
static uint8 g_fndDigits[4] = {0u, 0u, 0u, 0u};

static uint32_t LogHash(const Sw1Log* log)
{
    return (uint32_t)(log->type + log->tick_ms + log->hold_ms + log->state);
}

/* ===================== INTERRUPTS ===================== */
IFX_INTERRUPT(ISR0_cruise, 0, 0x10);
void ISR0_cruise(void)
{
    g_Sw1Event = 1u;
}

IFX_INTERRUPT(ISR1_emergency, 0, 0x11);
void ISR1_emergency(void)
{
    g_Sw2Event = 1u;
}

/* ===================== MAIN ===================== */
int core0_main(void)
{
    IfxCpu_enableInterrupts();

    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    initGPIO();
    initERU();
    Driver_Stm_Init();
    Driver_Adc_Init();
    initGtmTomPwm();

    while (1)
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
    uint8_t content = LED_0F[value];
    send_port(content, position);
}


/* ===================== TASKS ===================== */
void AppTask1ms(void)
{
    static uint8 position = 0;

    switch (position)
    {
        case 0: fnd_display_unit(g_fndDigits[0], 0x1u); break;
        case 1: fnd_display_unit(g_fndDigits[1], 0x2u); break;
        case 2: fnd_display_unit(g_fndDigits[2], 0x4u); break;
        case 3: fnd_display_unit(g_fndDigits[3], 0x8u); break;
        default: break;
    }

    position = (uint8)((position + 1u) & 0x3u);
    stTestCnt.u32nuCnt1ms++;
}

void AppTask10ms(void)
{
    if (g_Sw2Event)
    {
        g_Sw2Event = 0u;

        if (g_CurrentState != STATE_EMERGENCY)
            g_CurrentState = STATE_EMERGENCY;
        else
            g_CurrentState = STATE_NORMAL;
    }

    if (g_CurrentState == STATE_EMERGENCY)
    {
        g_sw1Hold_ms = 0u;
    }
    else
    {
        if (g_Sw1Event)
        {
            g_Sw1Event = 0u;
            g_sw1Hold_ms = 0u;
        }

        const boolean sw1_pressed =
            (IfxPort_getPinState(IfxPort_P02_0.port, IfxPort_P02_0.pinIndex) == FALSE);

        if (sw1_pressed)
        {
            if (g_sw1Hold_ms < SW1_HOLD_MAX_MS)
            {
                uint16 next = (uint16)(g_sw1Hold_ms + 10u);
                g_sw1Hold_ms = (next > SW1_HOLD_MAX_MS) ? SW1_HOLD_MAX_MS : next;
            }
        }
        else
        {
            if (g_sw1Hold_ms != 0u)
            {
                if (g_sw1Hold_ms >= SW1_LONGPRESS_MS)
                {
                    if (g_CurrentState == STATE_NORMAL)
                        g_CurrentState = STATE_CRUISE;
                    else
                        g_CurrentState = STATE_NORMAL;
                }
                else
                {
                    if (g_CurrentState == STATE_CRUISE)
                    {
                        uint16_t newTarget = (uint16_t)(g_TargetSpeed + SPEED_STEP);

                        if (newTarget > CRUISE_MAX_SPEED)
                        {
                            g_TargetSpeed = CRUISE_MIN_SPEED;
                        }                            
                        else
                        {
                            g_TargetSpeed = newTarget;
                        }                            
                    }
                }

                Sw1Log log;
                log.type = 1u;
                log.tick_ms = stTestCnt.u32nuCnt10ms * 10u;
                log.hold_ms = g_sw1Hold_ms;
                log.state = (uint8_t)g_CurrentState;
                (void)LogHash(&log);

                g_sw1Hold_ms = 0u;
            }
        }
    }

    uint32 v = g_CurrentSpeed;
    if (v > 9999u) v = 9999u;

    g_fndDigits[0] = (uint8)(v % 10u);
    g_fndDigits[1] = (uint8)((v / 10u) % 10u);
    g_fndDigits[2] = (uint8)((v / 100u) % 10u);
    g_fndDigits[3] = (uint8)((v / 1000u) % 10u);

    switch (g_CurrentState)
    {
        case STATE_NORMAL:
            LedOff(LED_R); LedOn(LED_G); LedOff(LED_B);
            break;

        case STATE_CRUISE:
            g_CurrentSpeed = g_TargetSpeed;
            LedOff(LED_R); LedOff(LED_G); LedOn(LED_B);
            break;

        case STATE_EMERGENCY:
            g_CurrentSpeed = 0;
            LedOn(LED_R); LedOff(LED_G); LedOff(LED_B);
            break;

        default:
            g_CurrentState = STATE_NORMAL;
            break;
    }

    stTestCnt.u32nuCnt10ms++;
}

void AppTask100ms(void)
{
    uint32_t potVal = Driver_Adc0_DataObtain(7);
    uint32_t ldrVal = Driver_Adc0_DataObtain(6);

    if (g_CurrentState == STATE_NORMAL)
    {
        g_CurrentSpeed = (uint16)((potVal * 200u) / 4095u);
    }


    if (IfxPort_getPinState(LED_D13.port, LED_D13.pinIndex) == FALSE)
    {
        if (ldrVal < LIGHT_DARK_ON)
        {
            LedOn(LED_D13);
        }
    }
    else
    {
        if (ldrVal > LIGHT_DARK_OFF)
        {
            LedOff(LED_D13);
        }
    }

    if (g_CurrentState == STATE_EMERGENCY)
    {
        LedToggle(LED_D12);
        makeSound(12);
    }
    else if (g_CurrentSpeed > 120u)
    {
        LedToggle(LED_D12);
        makeSound(7);
    }
    else
    {
        LedOff(LED_D12);
        makeSound(14);
    }

    stTestCnt.u32nuCnt100ms++;
}

void AppTask1000ms(void)
{
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
        if (X & 0x80u)
        {
            IfxPort_setPinHigh(DIO.port, DIO.pinIndex);
        }
        else
        {
            IfxPort_setPinLow(DIO.port, DIO.pinIndex);
        }

        X <<= 1;
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
