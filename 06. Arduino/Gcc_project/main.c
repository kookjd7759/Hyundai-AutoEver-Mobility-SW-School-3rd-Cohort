#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>

#define LED_DELAY_TIME	(100) //ms

int main(void) {
	DDRB = 0xFF; // PB[7:0]을 모두 출력으로 설정한다.
	PORTB = 0x00; // PB[7:0]에 모두 0을 출력한다. (=LED가 꺼진다)
	
	while (1) {
		PORTB= 0xFF; // PB[7:0]에 모두 1을 출력한다. (=LED가 켜진다)
		_delay_ms(LED_DELAY_TIME); // LED_DELAY_TIME 만큼 기다린다.

		PORTB= 0x00; // PB[7:0]에 모두 0을 출력한다. (=LED가 꺼진다)
		_delay_ms(LED_DELAY_TIME); // LED_DELAY_TIME 만큼 기다린다.
		// 무한 루프로 계속 반복한다.
	}
	return (0);
}