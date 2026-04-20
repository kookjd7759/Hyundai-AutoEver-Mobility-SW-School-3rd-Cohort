#include "main.h"

void Loop_1();
void Loop_2();

void main (void)
{
    SYSTEM_Init();

    while (1)
    {
    } /* End of while */
}

IFX_INTERRUPT(Can_RxIsrHandler, 0, ISR_PRIORITY_CAN_RX);
void Can_RxIsrHandler (void)
{
    unsigned int rxID;
    unsigned char rxData[8] = {0, };
    int rxLen;
    Can_RecvMsg(&rxID, rxData, &rxLen);
}

void Loop_1()
{
    unsigned char c;
    while (1)
    {
        c = Asclin0_InUart();
        Asclin0_OutUart(c);
        if (c == '\r')
        {
            Asclin0_OutUart('\n');
        }
    }
}

void Loop_2()
{
    unsigned char c;
    while (1)
    {
        c = Asclin0_InUart();
        if (c == '0')
        {
            my_printf("Input : 0\n");
        }
        else if (c == '0')
        {
            my_printf("Input : 1\n");
        }
        else
        {
            my_printf("Other Input\n");
        }
    }
}

void Loop_3()
{
    while (1)
    {
        Can_SendMsg(0x10, (unsigned char *)"Hello!!!", 8);
        delay_ms(100);
    }
}

void Loop_4()
{
    eth_addr_t ethAddr = {
            .addr[0] = 0x11,
            .addr[1] = 0x22,
            .addr[2] = 0x33,
            .addr[3] = 0x44,
            .addr[4] = 0x55,
            .addr[5] = 0x66
    };
    initLwip(ethAddr);

    uint8 dstAddr[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    while (1)
    {

    }
}
