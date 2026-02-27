#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"

IfxCpu_syncEvent cpuSyncEvent = 0;

volatile uint32 g_resultA = 0;
volatile uint32 g_resultB = 0;
volatile uint32 g_resultC = 0;

static inline void ret_test(volatile uint32 *dst, uint32 v)
{
    *dst = v;
}


#pragma noinline
static uint32 local_opt(uint32 x, uint32 y, uint32 z, uint32 w)
{
    uint32 a1 = x + y + z;
    uint32 a2 = x + y + w;
    uint32 a3 = (x + y) * 3;

    uint32 a4 = a3;

    return a1 + a2;
}

#pragma noinline
static uint32 global_opt(void)
{
    int p = 1;
    int q = 2;
    int t = p * q;

    if(t == 2)
        return 100;
    else
        return 200;
}

#pragma noinline
static uint32 loop_opt(uint32 n, uint32 p, uint32 q)
{
    static volatile uint32 a[256];
    uint32 sum = 0;

    for(uint32 k = 0; k < 256; k++)
    {
        a[k] = 2 * (p - q) * (n - k + 1);
        sum += a[k];
    }
    return sum;
}

void core0_main(void)
{
    IfxCpu_enableInterrupts();

    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    IfxCpu_emitEvent(&cpuSyncEvent);
    IfxCpu_waitEvent(&cpuSyncEvent, 1);

    {
        uint32 rA = local_opt(10, 20, 30, 40);
        uint32 rB = global_opt();
        uint32 rC = loop_opt(100, 64, 10);

        ret_test(&g_resultA, rA);
        ret_test(&g_resultB, rB);
        ret_test(&g_resultC, rC);
    }

    while(1)
    {

    }
}
