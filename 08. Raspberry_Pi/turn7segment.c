#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <time.h>

#define GPIO_BASE   (0xFE200000) // GPIO controller address
#define GPIO_SIZE   (256)

#define GPIO_IN(g)  (*(gpio + ((g) / 10)) &= ~(7 << (((g) % 10) * 3))) // input
#define GPIO_OUT(g) (*(gpio + ((g) / 10)) |= (1 << (((g) % 10) * 3)))  // output

#define GPIO_SET(g) (*(gpio + 7) = 1 << g)	  // bit set
#define GPIO_CLR(g) (*(gpio + 10) = 1 << g)	  // bit clear
#define GPIO_GET(g) (*(gpio + 13) & (1 << g)) // get current GPIO infomation

volatile unsigned* gpio;

int main(int argc, char **argv) {
	int mem_fd;
	void* gpio_map;
	
	// get GPIO number
	if (argc < 1) { // input verification
		printf("Usage : %s GPIO_NO\n", argv[0]);
		return -1;
	}

	// /dev/mem device open
	if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
		perror("open() /dev/mem\n");
		return -1;
	}

	// GPIO and mmap
	gpio_map = mmap(NULL, GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, GPIO_BASE);
	if (gpio_map == MAP_FAILED) {
		printf("[Error] mmap() : %d\n", gpio_map);
		return -1;
	}

	gpio = (volatile unsigned*)gpio_map;

	GPIO_OUT(25); GPIO_OUT(8); GPIO_OUT(7); GPIO_OUT(1);
	GPIO_CLR(25); GPIO_CLR(8); GPIO_CLR(7); GPIO_CLR(1);

	GPIO_OUT(6); GPIO_OUT(13); GPIO_OUT(19); GPIO_OUT(26);
	GPIO_CLR(6); GPIO_CLR(13); GPIO_CLR(19); GPIO_CLR(26);

	while (1) {

	}

	munmap(gpio_map, GPIO_SIZE);
	close(mem_fd);

	return 0;
}