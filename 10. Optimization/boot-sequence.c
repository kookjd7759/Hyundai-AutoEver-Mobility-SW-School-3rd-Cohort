/**********************************************************************************************************************
 * \file Cpu0_Main.c
 * \summary TC275 Memory Layout Implementation Example
 *********************************************************************************************************************/
#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include <stdlib.h>


const uint32 g_rom_const = 0x12345678;


uint32 g_initialized_data = 0xABCDEFFF;


uint32 g_uninitialized_data;


uint32 addr_code, addr_data, addr_bss, addr_stack, addr_heap;

IfxCpu_syncEvent cpuSyncEvent = 0;



void check_stack_address(uint32 depth)
{
    uint32 local_var = depth;
    addr_stack = (uint32)&local_var;

    if(depth < 1)
    {
        check_stack_address(depth + 1);
    }
}


void crash_csa(int count) {
    int temp = count + 1;
    crash_csa(temp);
}

void core0_main(void)
{
    IfxCpu_enableInterrupts();

    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    IfxCpu_emitEvent(&cpuSyncEvent);
    IfxCpu_waitEvent(&cpuSyncEvent, 1);

    //crash_csa(1);

    addr_code = (uint32)&g_rom_const;

    addr_data = (uint32)&g_initialized_data;
    addr_bss  = (uint32)&g_uninitialized_data;

    void *heap_ptr = malloc(sizeof(uint32));
    if(heap_ptr != NULL)
    {
        addr_heap = (uint32)heap_ptr;
        free(heap_ptr);
    }

    check_stack_address(0);

    while(1)
    {

    }
}
