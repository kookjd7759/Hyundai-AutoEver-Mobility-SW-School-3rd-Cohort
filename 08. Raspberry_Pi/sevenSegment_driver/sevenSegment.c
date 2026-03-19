#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char **argv){
    char buf[BUFSIZ];
    int fd = -1;

    if (argc < 2){
        printf("Usage: %s <0-9 | x | .>\n", argv[0]);
        printf("  0~9 : display number\n");
        printf("  x   : all off\n");
        printf("  .   : dot only\n");
        return 1;
    }

    memset(buf, 0, sizeof(buf));
    printf("7SEG Set : %s\n", argv[1]);

    fd = open("/dev/gpioSeg", O_RDWR);
    if (fd < 0){
        perror("open(/dev/gpioSeg)");
        return 1;
    }

    if (write(fd, argv[1], strlen(argv[1])) < 0){
        perror("write");
        close(fd);
        return 1;
    }

    ssize_t r = read(fd, buf, sizeof(buf) - 1);
    if (r < 0){
        perror("read");
        close(fd);
        return 1;
    }
    buf[r] = '\0';

    printf("Read data : %s\n", buf);

    close(fd);
    return 0;
}
