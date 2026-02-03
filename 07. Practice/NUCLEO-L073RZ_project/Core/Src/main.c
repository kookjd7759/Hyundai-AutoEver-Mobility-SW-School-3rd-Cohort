int main(void){
	// RCC-IO_A Enable
	*(unsigned int*)0x4002102C = 0x01;
	
	// IO A Port
	// 0x5000 0000
	// mode conf
	*(unsigned int*)0x50000000 = 0xebfff4ff;
	// otype
	*(unsigned int*)0x50000004 = 0x0;
	// speed
	*(unsigned int*)0x50000008 = 0x0C000C00;
	// PULL UP/DOWN
	*(unsigned int*)0x5000000C = 0x24000000;
	// ODR
	*(unsigned int*)0x50000014 = 0x00000000;
	
	while (1){
		for(volatile int i = 0; i < 10000; ++i);
		*(volatile unsigned int*)0x50000014 = 0x00000020;
		for(volatile int i = 0; i < 10000; ++i);
		*(volatile unsigned int*)0x50000014 = 0x00000000;
	}
	
	while(1){}
}