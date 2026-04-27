#include "bsw.h"
#include "mutex.h"

#define LEN_BUF 40

MutexType s1;

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

void timerInit2()
{
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  TCCR1B |= (1 << CS12);
  TIMSK1 |= (1 << TOIE1);
}

void setup(void)
{
    pinMode(13, OUTPUT);
    Serial.begin(115200); //115200 bps, 8N1
    printfSerial("\n...............\n");
    printfSerial("...OS Starts...\n");
    printfSerial("...............\n");
    timerInit2();
    timerInit();
}

/* This task will be activated every second */

ISR(ISRPin2)
{
  digitalWrite(13, HIGH);
}

ISR(ISRPin3)
{
  digitalWrite(13, LOW);
}

TASK(TickPrint)
{
    static long c = -4;
    printfSerial("\n%4ld: ", c++);

    if (c == -2)
      InitMutex(&s1, Event1);
    else if (c == 0)
      ActivateTask(TaskL);
    else if (c == 5)
      ActivateTask(TaskH);

    TerminateTask();
}

volatile unsigned long shared = 0;

TASK(Task1)
{
  unsigned long i;
  printfSerial("Task1 begins...\n");
  for (i = 0; i < 2000000; ++i) {
    shared++;
  }
  printfSerial("Added 2000000 to shared\n");
  printfSerial("counter = %lu\n", shared);
  printfSerial("Task1 finishes...\n");
  TerminateTask();
}

TASK(Task2)
{
  static unsigned long i;
  if (i < 1000) {
    shared++;
  }
  else if (i == 1000)
    printfSerial("Added 1000 to shared\n");
  i++;
  TerminateTask();
}

TASK(TaskH)
{
  printfSerial("<TaskH begins.> \n");
  mdelay(1000);
  printfSerial("TaskH : Try Lock(S1). \n");
  GetMutex(&s1);
  printfSerial("TaskH : Get Lock(S1). \n");
  mdelay(3000);
  printfSerial("TaskH : Release Lock(S1). \n");
  ReleaseMutex(&s1);
  mdelay(1000);
  printfSerial("<TaskH ends.> \n");
  TerminateTask();
}

TASK(TaskL)
{
  printfSerial("<TaskL begins.> \n");
  mdelay(2000);
  printfSerial("TaskL : Try Lock(S1). \n");
  GetMutex(&s1);
  printfSerial("TaskL : Get Lock(S1). \n");
  mdelay(5000);
  printfSerial("TaskL : Release Lock(S1). \n");
  ReleaseMutex(&s1);
  mdelay(1000);
  printfSerial("<TaskL ends.> \n");
  TerminateTask();
}

static const char* error_to_string(StatusType error)
{
  switch (error) {
  case E_OK:
    return "E_OK";
  case E_OS_ACCESS:
    return "E_OS_ACCESS";
  case E_OS_CALLEVEL:
    return "E_OS_CALLEVEL";
  case E_OS_ID:
    return "E_OS_ID";
  case E_OS_LIMIT:
    return "E_OS_LIMIT";
  case E_OS_NOFUNC:
    return "E_OS_NOFUNC";
  case E_OS_RESOURCE:
    return "E_OS_RESOURCE";
  case E_OS_STATE:
    return "E_OS_STATE";
  case E_OS_VALUE:
    return "E_OS_VALUE";
  default:
    return "UNKNOWN_ERROR";
  }
}

extern "C" void printOut(const char* str)
{
  Serial.print(str);
}

extern "C" void ErrorHook(StatusType error)
{
  printOut("[ErrorHook:  ");
  printOut(error_to_string(error));
  printOut("]");
}
