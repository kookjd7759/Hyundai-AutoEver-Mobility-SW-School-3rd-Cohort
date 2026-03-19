/**********************************************************************************************************************
 * Cpu0_Main.c
 *********************************************************************************************************************/

#include "IfxCpu.h"
#include "IfxPort.h"
#include "IfxPort_PinMap.h"
#include "IfxScuWdt.h"
#include "Ifx_Types.h"

#include "IfxScuEru.h"
#include "IfxSrc.h"
#include "_PinMap/IfxScu_PinMap.h"

#include "Platform_Types.h"

#include "Driver_Adc.h"
#include "Driver_Stm.h"
#include "GTM_TOM_PWM.h"

/* ============================================================
 * Pin Defines
 * ============================================================ */
#define SCLK IfxPort_P00_0
#define RCLK IfxPort_P00_1
#define DIO IfxPort_P00_2

#define LED_R IfxPort_P02_7
#define LED_G IfxPort_P10_5
#define LED_B IfxPort_P10_3

#define LED_D12 IfxPort_P10_1

#define SW1_PIN IfxPort_P02_0
#define SW2_PIN IfxPort_P02_1
#define SW_ACTIVE_LEVEL (0u) // NOTE: pull-up circuit

/* ============================================================
 * Light / PWM Mapping
 * ============================================================ */
#define PWM_PERIOD_LIGHT (50000u)
#define LDR_DARK_LEVEL (2400u)
#define LDR_BRIGHT_LEVEL (3000u)

/* Smooth control */
#define LIGHT_SMOOTH_SHIFT (3u) // NOTE: 3-bit shift (div. by 8)

/* ============================================================
 * Speed Ramp Criterion
 * ============================================================ */
#define SPEED_RAMP_STEP_PER_10MS (1u)

/* ============================================================
 * Switch Processing
 * ============================================================ */
#define SW_DEBOUNCE_TICKS (3u)    // 30ms
#define SW_LONGPRESS_TICKS (100u) // 1000ms = 1s

/* ============================================================
 * Types
 * ============================================================ */
typedef struct
{
    uint32 cnt_1ms;
    uint32 cnt_10ms;
    uint32 cnt_100ms;
    uint32 cnt_1000ms;
} test_cnt_t;

typedef enum
{
    STATE_NORMAL,
    STATE_CRUISE,
    STATE_EMERGENCY,
    STATE_CRUISE_SET
} system_state_t;

typedef struct
{
    uint8  stable_level;
    uint8  debounce_cnt;
    uint16 hold_cnt;
    uint8  short_press;
    uint8  long_press;
    uint8  long_fired;
} sw_proc_t;

/* ============================================================
 * Globals
 * ============================================================ */
IfxCpu_syncEvent cpuSyncEvent = 0;

static volatile test_cnt_t     g_test_cnt;
static volatile system_state_t g_current_state = STATE_NORMAL;

static uint16 g_current_speed = 0u;
static uint16 g_target_speed = 70u;
static uint16 g_pot_speed = 0u;
static uint16 g_display_value = 0u;

/* 7-seg */
static const uint8 g_led_0f[10] = {0xC0u, 0xF9u, 0xA4u, 0xB0u, 0x99u,
                                   0x92u, 0x82u, 0xF8u, 0x80u, 0x90u};

/* Switch states */
static sw_proc_t g_sw1 = {1u, 0u, 0u, 0u, 0u, 0u};
static sw_proc_t g_sw2 = {1u, 0u, 0u, 0u, 0u, 0u};

/* Light control */
static uint32 g_light_duty_target_ticks = 0u;
static uint32 g_light_duty_filt_ticks = 0u;

/* ============================================================
 * Prototypes
 * ============================================================ */
static void init_gpio(void);

/* IO helpers */
static void led_on(IfxPort_Pin pin);
static void led_off(IfxPort_Pin pin);
static void led_toggle(IfxPort_Pin pin);

/* FND */
static void send(uint8 data);
static void send_port(uint8 data, uint8 digit_sel);
static void fnd_display_unit(uint8 value, uint8 position);

/* Switch */
static void sw_proc_update_10ms(sw_proc_t * sw, uint8 raw_level);
static void sw_events_update_10ms(void);

/* Control */
static uint16 map_pot_to_speed(uint32 pot_val);
static void   control_update_10ms(void);

/* Tasks */
static void app_task_1ms(void);
static void app_task_10ms(void);
static void app_task_100ms(void);
static void app_task_1000ms(void);
static void app_scheduling(void);

/* ============================================================
 * main
 * ============================================================ */
int core0_main(void)
{
    IfxCpu_enableInterrupts();

    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    IfxCpu_emitEvent(&cpuSyncEvent);
    IfxCpu_waitEvent(&cpuSyncEvent, 1);

    init_gpio();

    Driver_Stm_Init();
    Driver_Adc_Init();

    initGtmTomPwm();
    initPwm_Light();

    while (1)
    {
        app_scheduling();
    }

    return 1;
}

/* ============================================================
 * Scheduling
 * ============================================================ */
static void app_scheduling(void)
{
    if (1u == stSchedulingInfo.u8nuScheduling1msFlag)
    {
        stSchedulingInfo.u8nuScheduling1msFlag = 0u;
        app_task_1ms();

        if (1u == stSchedulingInfo.u8nuScheduling10msFlag)
        {
            stSchedulingInfo.u8nuScheduling10msFlag = 0u;
            app_task_10ms();
        }

        if (1u == stSchedulingInfo.u8nuScheduling100msFlag)
        {
            stSchedulingInfo.u8nuScheduling100msFlag = 0u;
            app_task_100ms();
        }

        if (1u == stSchedulingInfo.u8nuScheduling1000msFlag)
        {
            stSchedulingInfo.u8nuScheduling1000msFlag = 0u;
            app_task_1000ms();
        }
    }
}

/* ============================================================
 * Tasks
 * ============================================================ */
static void app_task_1ms(void)
{
    static uint8 digit_pos = 0u;
    uint32       disp_val;

    // determine the contents for FND according to state
    if (STATE_CRUISE_SET == g_current_state)
    {
        g_display_value = g_target_speed;
    }
    else
    {
        g_display_value = g_current_speed;
    }

    disp_val = (uint32)g_display_value;

    // display with one digit during 1ms
    switch (digit_pos)
    {
    case 0u:
    {
        fnd_display_unit((uint8)(disp_val % 10u), 0x1u);
        break;
    }
    case 1u:
    {
        fnd_display_unit((uint8)((disp_val / 10u) % 10u), 0x2u);
        break;
    }
    case 2u:
    {
        fnd_display_unit((uint8)((disp_val / 100u) % 10u), 0x4u);
        break;
    }
    case 3u:
    {
        fnd_display_unit((uint8)((disp_val / 1000u) % 10u), 0x8u);
        break;
    }
    default:
    {
        break;
    }
    }

    digit_pos = (uint8)((digit_pos + 1u) % 4u);
    g_test_cnt.cnt_1ms++;
}

static void app_task_10ms(void)
{
    sw_events_update_10ms();

    // State transitions based on events
    if ((0u != g_sw2.short_press) || (0u != g_sw2.long_press))
    {
        g_sw2.short_press = 0u;
        g_sw2.long_press = 0u;

        if (STATE_EMERGENCY != g_current_state)
        {
            g_current_state = STATE_EMERGENCY;
            g_current_speed = 0u; // NOTE: emergent stop
        }
        else
        {
            g_current_state = STATE_NORMAL;
        }
    }

    if (0u != g_sw1.long_press)
    {
        g_sw1.long_press = 0u;

        if (STATE_CRUISE_SET != g_current_state)
        {
            g_current_state = STATE_CRUISE_SET;
        }
        else
        {
            g_current_state = STATE_CRUISE;
        }
    }

    if (0u != g_sw1.short_press)
    {
        g_sw1.short_press = 0u;

        if (STATE_NORMAL == g_current_state)
        {
            g_current_state = STATE_CRUISE;
        }
        else if (STATE_CRUISE == g_current_state)
        {
            g_current_state = STATE_NORMAL;
        }
        else
        {
            // do nothing in other states (STATE_EMERGENCY and STATE_CRUISE_SET)
        }
    }

    // operation according to the state
    switch (g_current_state)
    {
    case STATE_NORMAL:
    {
        led_off(LED_R);
        led_on(LED_G);
        led_off(LED_B);
        break;
    }
    case STATE_CRUISE:
    {
        led_off(LED_R);
        led_off(LED_G);
        led_on(LED_B);
        break;
    }
    case STATE_EMERGENCY:
    {
        led_on(LED_R);
        led_off(LED_G);
        led_off(LED_B);
        break;
    }
    case STATE_CRUISE_SET:
    {
        led_off(LED_R);
        led_on(LED_G);
        led_on(LED_B);
        break;
    }
    default:
    {
        break;
    }
    }

    control_update_10ms();

    g_test_cnt.cnt_10ms++;
}

static void app_task_100ms(void)
{
    uint32 pot_val;
    uint32 ldr_val;
    uint32 duty_ticks;
    uint32 span_ticks;
    uint32 ldr_offset;

    pot_val = Driver_Adc0_DataObtain(7);
    ldr_val = Driver_Adc0_DataObtain(6);

    g_pot_speed = map_pot_to_speed(pot_val);

    // In CRUISE_SET, potentiometer determines the target speed
    if (STATE_CRUISE_SET == g_current_state)
    {
        g_target_speed = g_pot_speed;
    }

    // duty cycle determination according to light level
    if (ldr_val <= LDR_DARK_LEVEL)
    {
        duty_ticks = PWM_PERIOD_LIGHT;
    }
    else if (ldr_val >= LDR_BRIGHT_LEVEL)
    {
        duty_ticks = 0u;
    }
    else
    {
        ldr_offset = (uint32)(ldr_val - LDR_DARK_LEVEL);
        span_ticks = (uint32)(LDR_BRIGHT_LEVEL - LDR_DARK_LEVEL);

        duty_ticks =
            PWM_PERIOD_LIGHT - ((ldr_offset * PWM_PERIOD_LIGHT) / span_ticks);
    }

    if (duty_ticks > PWM_PERIOD_LIGHT)
    {
        duty_ticks = PWM_PERIOD_LIGHT;
    }

    g_light_duty_target_ticks = duty_ticks;

    // Buzzer and LED policy in warning case
    if (STATE_EMERGENCY == g_current_state)
    {
        led_toggle(LED_D12);
        makeSound(12u);
    }
    else if (g_current_speed > 120u)
    {
        led_toggle(LED_D12);
        makeSound(7u);
    }
    else
    {
        led_off(LED_D12);
        makeSound(14u);
    }

    g_test_cnt.cnt_100ms++;
}

static void app_task_1000ms(void) { g_test_cnt.cnt_1000ms++; }

/* ============================================================
 * Unified Control
 * - Speed control smoothing
 * - Light smoothing
 * ============================================================ */
static void control_update_10ms(void)
{
    uint16 desired_speed;

    // Decide desired speed by state
    if (STATE_EMERGENCY == g_current_state)
    {
        desired_speed = 0u;
    }
    else if (STATE_CRUISE == g_current_state)
    {
        desired_speed = g_target_speed;
    }
    else if (STATE_CRUISE_SET == g_current_state)
    {
        desired_speed = g_current_speed;
    }
    else
    {
        desired_speed = g_pot_speed;
    }

    // Apply smoothing in all states
    if (STATE_CRUISE_SET != g_current_state)
    {
        if (g_current_speed < desired_speed)
        {
            uint16 diff;
            uint16 step;

            diff = (uint16)(desired_speed - g_current_speed);
            step = ((uint16)SPEED_RAMP_STEP_PER_10MS < diff)
                       ? (uint16)SPEED_RAMP_STEP_PER_10MS
                       : diff;

            g_current_speed = (uint16)(g_current_speed + step);
        }
        else if (g_current_speed > desired_speed)
        {
            uint16 diff;
            uint16 step;

            diff = (uint16)(g_current_speed - desired_speed);
            step = ((uint16)SPEED_RAMP_STEP_PER_10MS < diff)
                       ? (uint16)SPEED_RAMP_STEP_PER_10MS
                       : diff;

            g_current_speed = (uint16)(g_current_speed - step);
        }
        else
        {
            // when current speed == desired speed, do nothing
        }
    }

    // Light smoothing and generate PWM
    {
        sint32 delta;

        delta =
            (sint32)g_light_duty_target_ticks - (sint32)g_light_duty_filt_ticks;
        g_light_duty_filt_ticks = (uint32)((sint32)g_light_duty_filt_ticks +
                                           (delta >> LIGHT_SMOOTH_SHIFT));

        if (g_light_duty_filt_ticks > PWM_PERIOD_LIGHT)
        {
            g_light_duty_filt_ticks = PWM_PERIOD_LIGHT;
        }

        LightPwm_SetDutyTicks(g_light_duty_filt_ticks);
    }
}

/* ============================================================
 * Switch level update
 * ============================================================ */
static void sw_events_update_10ms(void)
{
    uint8 sw1_level;
    uint8 sw2_level;

    sw1_level = (uint8)IfxPort_getPinState(SW1_PIN.port, SW1_PIN.pinIndex);
    sw2_level = (uint8)IfxPort_getPinState(SW2_PIN.port, SW2_PIN.pinIndex);

    sw_proc_update_10ms(&g_sw1, sw1_level);
    sw_proc_update_10ms(&g_sw2, sw2_level);
}

/* ============================================================
 * SW Polling with debounce (short press / long press)
 * ============================================================ */
static void sw_proc_update_10ms(sw_proc_t * sw, uint8 raw_level)
{
    uint8 level_changed;
    uint8 pressed;
    uint8 long_check;

    level_changed = (uint8)(raw_level != sw->stable_level);

    // Checking debounce -> determine short press
    if ((0u != level_changed) && (sw->debounce_cnt < SW_DEBOUNCE_TICKS))
    {
        sw->debounce_cnt++;
    }
    else if (0u != level_changed)
    {
        sw->stable_level = raw_level;
        sw->debounce_cnt = 0u;

        pressed = (uint8)(SW_ACTIVE_LEVEL == sw->stable_level);

        if (0u != pressed)
        {
            sw->hold_cnt = 0u;
            sw->long_fired = 0u;
        }

        if ((0u == pressed) && (0u == sw->long_fired))
        {
            sw->short_press = 1u;
        }
    }
    else
    {
        sw->debounce_cnt = 0u;
    }

    // Long press detection
    if ((0u == level_changed) && (SW_ACTIVE_LEVEL == sw->stable_level))
    {
        if (sw->hold_cnt < 0xFFFFu)
        {
            sw->hold_cnt++;
        }

        long_check = (uint8)((SW_LONGPRESS_TICKS <= sw->hold_cnt) &&
                             (0u == sw->long_fired));
        if (0u != long_check)
        {
            sw->long_press = 1u;
            sw->long_fired = 1u;
        }
    }

    return;
}

/* ============================================================
 * Misc. helpers (Potentiometer value, LED control)
 * ============================================================ */
static uint16 map_pot_to_speed(uint32 pot_val)
{
    return (uint16)((pot_val * 200u) / 4095u);
}

static void led_on(IfxPort_Pin pin)
{
    IfxPort_setPinHigh(pin.port, pin.pinIndex);
}

static void led_off(IfxPort_Pin pin)
{
    IfxPort_setPinLow(pin.port, pin.pinIndex);
}

static void led_toggle(IfxPort_Pin pin)
{
    IfxPort_togglePin(pin.port, pin.pinIndex);
}

/* ============================================================
 * Control FND display
 * ============================================================ */
static void fnd_display_unit(uint8 value, uint8 position)
{
    uint8 content;

    content = g_led_0f[value];
    send_port(content, position);
}

/* ============================================================
 * Send data step by step (SCLK)
 * ============================================================ */
static void send(uint8 data)
{
    sint32 bit_idx;

    for (bit_idx = 8; bit_idx >= 1; bit_idx--)
    {
        if (0u != (data & 0x80u))
        {
            IfxPort_setPinHigh(DIO.port, DIO.pinIndex);
        }
        else
        {
            IfxPort_setPinLow(DIO.port, DIO.pinIndex);
        }

        data <<= 1;

        IfxPort_setPinLow(SCLK.port, SCLK.pinIndex);
        IfxPort_setPinHigh(SCLK.port, SCLK.pinIndex);
    }
}

/* ============================================================
 * Send data and location then latch (RCLK)
 * ============================================================ */
static void send_port(uint8 data, uint8 digit_sel)
{
    send(data);
    send(digit_sel);

    IfxPort_setPinLow(RCLK.port, RCLK.pinIndex);
    IfxPort_setPinHigh(RCLK.port, RCLK.pinIndex);
}

/* ============================================================
 * init General-Purpose Input/Output
 * ============================================================ */
static void init_gpio(void)
{
    // Pin setting for LED_D12
    IfxPort_setPinModeOutput(LED_D12.port, LED_D12.pinIndex,
                             IfxPort_OutputMode_pushPull,
                             IfxPort_OutputIdx_general);
    led_off(LED_D12);

    // Pin setting for RGB LEDs
    IfxPort_setPinModeOutput(LED_R.port, LED_R.pinIndex,
                             IfxPort_OutputMode_pushPull,
                             IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(LED_G.port, LED_G.pinIndex,
                             IfxPort_OutputMode_pushPull,
                             IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(LED_B.port, LED_B.pinIndex,
                             IfxPort_OutputMode_pushPull,
                             IfxPort_OutputIdx_general);

    led_off(LED_R);
    led_off(LED_G);
    led_off(LED_B);

    // Pin setting for FND
    IfxPort_setPinModeOutput(SCLK.port, SCLK.pinIndex,
                             IfxPort_OutputMode_pushPull,
                             IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(RCLK.port, RCLK.pinIndex,
                             IfxPort_OutputMode_pushPull,
                             IfxPort_OutputIdx_general);
    IfxPort_setPinModeOutput(DIO.port, DIO.pinIndex,
                             IfxPort_OutputMode_pushPull,
                             IfxPort_OutputIdx_general);

    // Pin setting for switch
    IfxPort_setPinModeInput(SW1_PIN.port, SW1_PIN.pinIndex,
                            IfxPort_InputMode_pullUp);
    IfxPort_setPinModeInput(SW2_PIN.port, SW2_PIN.pinIndex,
                            IfxPort_InputMode_pullUp);
}
