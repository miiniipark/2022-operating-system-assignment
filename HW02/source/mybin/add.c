#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    int num = 0;
    int result = 0;

    num = atoi(argv[1]);
    result += num;
    printf("%d ", num);
    for (int i = 2; i < argc; i++) {
        num = atoi(argv[i]);
        result += num;
        printf("+ %d ", num);
    }
    printf("= %d\n", result);

    return 0;
}