
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

int file;
int adapter_nr = 2;
char filename[20];


int main( int argc, char **argv){
	int i;
	int r;
	int fd;
	unsigned char command[2];
	unsigned char value[4];
	useconds_t delay = 2000;

	char *dev = "/dev/i2c-0";
	int addr = 0x68;

	fd = open(dev, O_RDWR);

	if(fd < 0){
		perror("Opening i2c device node\n");
		return 1;
	}

	r = ioctl(fd, I2C_SLAVE, addr);
	if(r < 0){
		perror("Selecting i2c device\n");
	}

	
}
