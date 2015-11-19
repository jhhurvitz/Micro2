#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>

//general values
int addr = 0x68;
int secLoc = 0x0;
int minLoc = 0x01;
int hourLoc = 0x02;


int i2cFileHandle; 
int errorCheck;

unsigned char command[2];
unsigned char readValue[4];
useconds_t delay = 2000;

void init(){
	i2cFileHandle = open("/dev/i2c-0",O_RDWR);
	errorCheck = ioctl(i2cFileHandle, I2C_SLAVE, addr);
	if(errorCheck < 0){
		printf("Error selecting i2c device?\n");
		exit(-1);
	}
}

int main(){
	init();

	errorCheck = read(i2cFileHandle, &readValue[0], 1);
	if(errorCheck != 1){
		printf("We dun fucked up.\n");
		exit(-1);
	}

	printf("What do we have here: %hhX %hhX\n", readValue[0]);
}







