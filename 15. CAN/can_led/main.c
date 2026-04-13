#include "main.h"

unsigned char CanData[8] = {0, };
void main (void)
{
    SYSTEM_Init();

    while (1);
}

IFX_INTERRUPT(Can_RxIsrHandler, 0, ISR_PRIORITY_CAN_RX);
void Can_RxIsrHandler (void)
{
   unsigned int rxID;
   unsigned char rxData[8] = {0,};
   int rxLen;
   Can_RecvMsg(&rxID, rxData, &rxLen);

   if (rxID == 0x130)
   {
       if (rxData[0] == 1)
       {
           GPIO_SetLed(1, 1);
       }
       else if (rxData[0] == 0)
       {
           GPIO_SetLed(1, 0);
       }
   }
   else if (rxID == 0x131)
   {
       if (rxData[0] == 1)
       {
           GPIO_SetLed(2, 1);
       }
       else if (rxData[0] == 0)
       {
           GPIO_SetLed(2, 0);
       }
   }
}

IFX_INTERRUPT(CanFd_RxIsrHandler, 0, ISR_PRIORITY_CANFD_RX);
void CanFd_RxIsrHandler (void)
{
    unsigned int rxID = 0;
    char rxData[64] = {0, };
    int rxDLC = 0;
    CanFd_RecvMsg(&rxID, rxData, &rxDLC);
}
