#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    int num = 0;
    int result = 0;

    if (argc > 3) {
        printf("Too many arguments\n");

        return 1;
    }

    num = atoi(argv[1]);
    result = num;
    printf("%d ", num);
    num = atoi(argv[2]);
    result /= num;
    printf("/ %d ", num);
    printf("= %d\n", result);

    return 0;
}