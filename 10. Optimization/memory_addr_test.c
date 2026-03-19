#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include <stdlib.h>

const uint32 g_rom_const = 0x12345678;

uint32 g_initialized_data = 0xABCDEFFF;

uint32 g_uninitialized_data;

uint32 addr_code;
uint32 addr_data;
uint32 addr_bss;
uint32 addr_stack[3];
uint32 addr_heap[3];

IfxCpu_syncEvent cpuSyncEvent = 0;

void check_stack_growth(uint32 depth)
{
    uint32 local_var = depth;
    addr_stack[depth] = (uint32)&local_var;

    if(depth < 3){
        check_stack_growth(depth + 1);
    }
}

void core0_main(void)
{
    IfxCpu_enableInterrupts();
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());
    IfxCpu_emitEvent(&cpuSyncEvent);
    IfxCpu_waitEvent(&cpuSyncEvent, 1);

    addr_code = (uint32)&g_rom_const;
    addr_data = (uint32)&g_initialized_data;
    addr_bss  = (uint32)&g_uninitialized_data;

    for(int i = 0; i < 3; i++)
    {
        void *ptr = malloc(10);
        addr_heap[i] = (uint32)ptr;
    }

    check_stack_growth(0);

    while(1)
    {

    }
}
