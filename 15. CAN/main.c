#include "main.h"

unsigned char CanData[8] = {0, };
void main (void)
{
    SYSTEM_Init();

    while (1);
}

IFX_INTERRUPT(isr_rx_can, 0, ISR_PRIORITY_CAN_RX);
void isr_rx_can(void){
    unsigned int rxID;
    unsigned char rxData[8] = {0, };
    int rxLen;
    Can_RecvMsg(&rxID, rxData, &rxLen);
    if (rxID == 0x100)
        Can_SendMsg(0x200, "RX_OK!!!", 8);
}

IFX_INTERRUPT(Can_RxIsrHandler1, 0, ISR_PRIORITY_CAN_RX);
void Can_RxIsrHandler1 (void)
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

IFX_INTERRUPT(Can_RxIsrHandler2, 0, ISR_PRIORITY_CAN_RX);
void Can_RxIsrHandler2 (void)
{
   unsigned int rxID;
   unsigned char rxData[8] = {0,};
   int rxLen;
   Can_RecvMsg(&rxID, rxData, &rxLen);

   unsigned int senVal;
   unsigned char SID = rxData[1];
   unsigned short DID = rxData[2] << 8 | rxData[3];
   unsigned char res = SID + 0x40;
   unsigned char negCanData[8] = {0x3, 0x7F, SID, 0x11, 0, 0, 0, 0 };
   unsigned char posCanData[8] = {0x5, res, rxData[2], rxData[3], 0, 0, 0, 0 };

   if (rxID >= 0x700)
   {
       if (SID == 0x22)
       {
           if (DID == 1)
           {
               senVal = Evadc_readVR();
           }
           else if (DID == 2)
           {
               senVal = Evadc_readPR();
           }
       }
       else
       {
           res = 0x7f;
       }
   }
   else
   {
       res = 0x7f;
   }

   if (res == 0x7f)
   {
       Can_SendMsg(0x7e8, negCanData, 8);
   }
   else
   {
       posCanData[4] = (senVal & 0xFF00) >> 8;
   }

   posCanData[5] = (senVal & 0xFF);
   Can_SendMsg(0x7e8, posCanData, 8);
}

IFX_INTERRUPT(Can_RxIsrHandler3, 0, ISR_PRIORITY_CAN_RX);
void Can_RxIsrHandler3 (void)
{
    unsigned int rxID;
    unsigned char rxData[8] = {0,};
    int rxLen;
    Can_RecvMsg(&rxID, rxData, &rxLen);

    unsigned int tofValue = rxData[2] << 16 | rxData[1] << 8 | rxData[0];
    unsigned char dis_status = rxData[3];
    unsigned short signal_strength = rxData[5] << 8 | rxData[4];
    if (signal_strength != 0)
    {
        if (tofValue >= 1500)
        {
            GPIO_SetLed(1, 1);
        }
        else
        {
            GPIO_SetLed(1, 0);
        }
        if (tofValue <= 300)
        {
            GPIO_SetLed(2, 1);
        }
        else
        {
            GPIO_SetLed(2, 0);
        }
    }
    else
    {
        my_printf("out of range\n");
    }
}

IFX_INTERRUPT(Can_RxIsrHandler4, 0, ISR_PRIORITY_CAN_RX);
void Can_RxIsrHandler4 (void)
{
    unsigned int rxID;
    unsigned char rxData[8] = {0,};
    int rxLen;
    Can_RecvMsg(&rxID, rxData, &rxLen);

    switch (rxID){
        case 0x150:
            GPIO_ToggleLed(1);
            break;
        case 0x7FF:
            GPIO_ToggleLed(2);
            break;
    }
}

IFX_INTERRUPT(CanFd_RxIsrHandler, 0, ISR_PRIORITY_CAN_RX);
void CanFd_RxIsrHandler (void)
{
    unsigned int rxID = 0;
    char rxData[64] = {0, };
    int rxDLC = 0;
    CanFd_RecvMsg(&rxID, rxData, &rxDLC);
}
