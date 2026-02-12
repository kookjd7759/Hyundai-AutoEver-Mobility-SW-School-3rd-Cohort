#include <stdio.h>

void func() {
    int* p = 0;
    *p = 1;
}

int main(void) {
    func();

    return 0;
}