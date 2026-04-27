#include "tpl_os.h"
#include "Arduino.h"
#include "bsw.h"

#define LEN_BUF 64

extern "C" {

void mdelay(unsigned long delay_ms)
{
    unsigned long prev_ms = millis(), current_ms = millis();
    unsigned long period_ms = 20, cnt = 0;
    while (cnt < (delay_ms / period_ms)) {
        current_ms = millis();
        if (current_ms - prev_ms >= period_ms) {
            cnt++;
            prev_ms = millis();
        }
    }
}

void printfSerial(const char *fmt, ...)
{
    char buf[LEN_BUF];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, LEN_BUF, fmt, args);
    va_end(args);
    Serial.print(buf);
}

} // extern "C"

/**
 * configure TIMER2 to generate an interrupt
 * each 1 ms
 */
void timerInit()
{
  TCCR2A = 0;
  TCNT2 = 0;
  // clkio = 16MHz => clk = 250KHz
  TCCR2B = (1 << WGM22) | (1 << CS22); // CTC and clk = Clkio/64
                                       // interrupt each 1ms
  OCR2A = 250 - 1;
  TIMSK2 |= (1 << TOIE2);
  interrupts();
}

void setup(void)
{
    pinMode(13, OUTPUT);
    Serial.begin(115200); //115200 bps, 8N1
    printfSerial("\n...............\n");
    printfSerial("...OS Starts...\n");
    printfSerial("...............\n");
    timerInit();
}

/* This task will be activated every second */
TASK(TickPrint)
{
    static long c = -4;
    //printfSerial("\n%4ld: ", c++);
    TerminateTask();
}
