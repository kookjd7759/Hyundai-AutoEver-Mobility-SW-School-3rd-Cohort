#ifndef BSW_H_
#define BSW_H_

#include "Arduino.h"
#undef ISR //remove the avr-gcc ISR version
#include "tpl_app_config.h"
#include "tpl_os.h" //set the OSEK ISR version.

#ifdef __cplusplus
extern "C" {
#endif

void mdelay(unsigned long delay_ms);

void printfSerial(const char *fmt, ... );

#ifdef __cplusplus
}
#endif

#endif /* BSW_H_ */
