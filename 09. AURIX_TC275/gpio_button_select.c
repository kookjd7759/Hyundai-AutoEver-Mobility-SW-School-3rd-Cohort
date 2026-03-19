#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"

#define PCn_2_IDX 19
#define P2_IDX 2
#define PCn_1_IDX 11
#define P1_IDX 1

IfxCpu_syncEvent cpuSyncEvent = 0;

void initGPIO(void);

int core0_main(void)
{
    IfxCpu_enableInterrupts();
    
    /* !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
     * Enable the watchdogs and service them periodically if it is required
     */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());
    
    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&cpuSyncEvent);
    IfxCpu_waitEvent(&cpuSyncEvent, 1);

    initGPIO();
        
    while(1)
    {
        if((P02_IN.U & (0x1 << P1_IDX)) == 0){
            P10_OUT.U = 0x1 << P1_IDX;
        }
        else{
            P10_OUT.U = 0x1 << P2_IDX;
        }
    }
    return (1);
}

void initGPIO(void){
    P02_IOCR0.U &= ~(0x1F << PCn_1_IDX);
    P02_IOCR0.U |= 0x02 << PCn_1_IDX;

    P10_IOCR0.U &= ~(0x1F << PCn_2_IDX);
    P10_IOCR0.U |= 0x10 << PCn_2_IDX;

    P10_IOCR0.U &= ~(0x1F << PCn_1_IDX);
    P10_IOCR0.U |= 0x10 << PCn_1_IDX;
}
