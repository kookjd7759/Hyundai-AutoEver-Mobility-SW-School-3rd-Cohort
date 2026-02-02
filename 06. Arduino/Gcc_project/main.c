#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

#define sbi(REG, n) (REG |=  (1<<n))
#define cbi(REG, n) (REG &= ~(1<<n))

#define LED_DIR_PORT  (DDRB)
#define LED_OUT_PORT  (PORTB)

#define LED_1_PIN (PINB5)

#define KEY_DIR_PORT	(DDRB)
#define KEY_IN_PORT		(PINB)

#define KEY_PRESSED		(0)
#define KEY_RELEASED	(1)

#define KEY_1_PIN		(PINB7)


int main(void) {
	cbi(KEY_DIR_PORT, KEY_1_PIN); // KEY는 PB7 입력 설정
	sbi(LED_DIR_PORT, LED_1_PIN); // LED는 PB5 출력 설정
	cbi(LED_OUT_PORT, LED_1_PIN); // LED off
	
	uint8_t key_1_val= 0; 
	
	while(1) {
		key_1_val= (KEY_IN_PORT >> KEY_1_PIN);
		
		switch(key_1_val) {
			case KEY_PRESSED:
			sbi(LED_OUT_PORT, LED_1_PIN);
			break;
			
			case KEY_RELEASED: // 1
			cbi(LED_OUT_PORT, LED_1_PIN);
			break;
		}
	}
	return (0);
}