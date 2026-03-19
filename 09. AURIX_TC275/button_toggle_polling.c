#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"

#define PCn_1_IDX 11
#define P1_IDX 1

IfxCpu_syncEvent cpuSyncEvent = 0;

void initGPIO(void);
uint8 S3_check(void);

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
        uint8 S3_currentStatus = S3_check();

        if(S3_currentStatus == 0) {

            if ((P10_OUT.U & (0x1 << P1_IDX)) != 0)
            {
                P10_OMR.U = (1 << (P1_IDX + 16));
            } 
            else 
            {
                P10_OMR.U = (1 << P1_IDX);
            }
            do { 
                S3_currentStatus = S3_check(); 
            } while (S3_currentStatus == 0);
        }
    }

    return (1);
}

void initGPIO(void){
    P02_IOCR0.U &= ~(0x1F << PCn_1_IDX);
    P02_IOCR0.U |= 0x02 << PCn_1_IDX;

    P10_IOCR0.U &= ~(0x1F << PCn_1_IDX);
    P10_IOCR0.U |= 0x10 << PCn_1_IDX;
}

uint8 S3_check(void){
    return (uint8)((P02_IN.U >> P1_IDX) & 0x1);
}