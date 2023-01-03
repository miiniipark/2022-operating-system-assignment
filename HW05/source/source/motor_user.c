#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cmd.h"

int main(int argc, char* argv[])
{
	int fd = open("/dev/motor_driver", O_WRONLY);
	int value;

	if (fd < 0) {
		printf("Unable to open device!\n");
		return -1;
	}

	if (argc == 2) {
		write(fd, argv[1], sizeof(argv[1]));
		printf("Update the power-on time using function 'write'...!!!\n");
	}
	else {
		value = atoi(argv[2]);
		if (strcmp(argv[1], "on") == 0) {
			ioctl(fd, USEC_ON, &value);
			printf("Update the power-on time using function 'ioctl'...!!!\n");
		}
		else if (strcmp(argv[1], "off") == 0) {
			ioctl(fd, USEC_OFF, &value);
			printf("Update the power-off time using function 'ioctl'...!!!\n");
		}
		else if (strcmp(argv[1], "state") == 0) {
			ioctl(fd, STATE, &value);
			printf("Update the state using function 'ioctl'...!!!\n");
		}
	}

	close(fd);
	return 0;
}