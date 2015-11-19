
#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#define GP_I2C   (29)
#define GPIO_DIRECTION_OUT  (0)
#define ERROR               (-1)



int main(void){

	int i2cHandle;



	printf("We're doing things!\n");
	
	int deviceHandle;

	initiateGPIO(GP_I2C);
	i2cHandle = openGPIO(GP_I2C, GPIO_DIRECTION_OUT);
	
	writeGPIO(i2cHandle, 0);
	close(i2cHandle);



	int readStatus;
	int writeStatus;

	char buffer[7];
	
	buffer[0] = 0x00;

	int deviceI2CAddress = 0x68;

	if ((deviceHandle = open("/dev/i2c-0", O_RDWR)) < 0){
		printf("Turns out we're not.\n");
		return 1;
	}

	if(ioctl(deviceHandle, I2C_SLAVE, deviceI2CAddress) < 0){
		printf("We dun fucked up again!\n");
		return 1;
	}
	
	
//	struct timespec sysTime;
//      clock_gettime(CLOCK_REALTIME, &sysTime);
                   
//	printf("Time in seconds since unix epoch: %d\n", sysTime.tv_sec);
//	printf("Remainder in nanoseconds	: %d\n", sysTime.tv_nsec);
	int sysSeconds,sysMinutes,sysHours,sysYears,sysDays,sysMonths ;
//	sysSeconds = system.tv_sec

         time_t result = time(NULL);
    if(result != -1)
	{

	
	struct tm *timeInfo;

	timeInfo = gmtime(&result);

	//printf("Seconds: %d\n", timeInfo->tm_sec);
	//printf("Minute: %d\n", timeInfo->tm_min);
	//printf("Hour: %d\n",(timeInfo->tm_hour)%24);//HOUR
	
	//printf("Year: %d\n", (timeInfo->tm_year+1900) %100 );
	//printf("Day: %d\n", timeInfo->tm_mday);
	//printf("Month: %d\n", timeInfo->tm_mon+1);



	sysSeconds = timeInfo->tm_sec;
        sysMinutes = timeInfo->tm_min;
        sysHours = (timeInfo->tm_hour)%24;//HOUR

        sysYears = (timeInfo->tm_year+1900) %100 ;
        sysDays = timeInfo->tm_mday;
        sysMonths = (timeInfo->tm_mon+1);
	printf("%d\n", sysMonths);



	}
//        printf("The current time is %s(%ju seconds since the Epoch)\n",
//               asctime(gmtime(&result)), (uintmax_t)result);
//	printf("%d\n",result);

	buffer[0] = sysSeconds;
        buffer[1] = sysMinutes;
        buffer[2] = sysHours; //
        buffer[4] = sysYears; //this is the year, I think?
        buffer[5] = sysDays;
        buffer[6] = sysMonths;


	//buffer[0] = 0xaa;
	//buffer[1] = 0xbb;
	//buffer[2] = 0xcc; //
	//buffer[4] = 0xdd; //this is the year, I think?
	//buffer[5] = 0xee;
	//buffer[6] = 0xff;

	writeStatus = write(deviceHandle, buffer, 1);

	if(writeStatus != 1){
		printf("Error: more error! (no ack bit)\n");
		return 0;
	}else{
		int seconds = buffer[0];
		int minutes = buffer[1];
		int hours = buffer[2];
		int dayOfWeek = buffer[3];
		int day = buffer[4];
		int month = buffer[5];
		int year = buffer[6];

		printf("Actual RTC-time:\n");
		printf("Date: %d-%d-%d\n", year, month, day);
		printf("Time: %d:%d:%d\n", hours, minutes, seconds);
	}
}




int initiateGPIO (int gpio)
{

	int fileHandle;
	char buffer[256];
	

	fileHandle = open("/sys/class/gpio/export", O_WRONLY);
	if (ERROR == fileHandle)
	{
		printf("[\033[0;31m Error \033[m]\t Unable to open /sys/class/gpio/export\n");
		sleep(1);
		exit (-1);
	}
	else
	{
		sprintf(buffer, "%d", gpio);
		write(fileHandle, buffer, strlen(buffer));

		close(fileHandle);
	}

	return (0);
}



int openGPIO (int gpio, int direction)
{
	char buffer [256];
	int fileHandle;
	int fileMode;

      //specify gpio direction

  sprintf(buffer, "/sys/class/gpio/gpio%d/direction", gpio);
  fileHandle = open(buffer, O_WRONLY);
  if(ERROR == fileHandle)
  {
         printf("[\033[0;31m Error \033[m]\t Unable to open: %s \n", buffer);
         exit(-1);
  }

  if (direction == GPIO_DIRECTION_OUT)
  {
         // Set out direction
         write(fileHandle, "out", 3);
         fileMode = O_WRONLY;
  }
  else
  {
         // Set in direction
         write(fileHandle, "in", 2);
         fileMode = O_RDONLY;
  }
  close(fileHandle);

  //Set how I/O will be handled
  sprintf(buffer, "/sys/class/gpio/gpio%d/drive",gpio);
  fileHandle = open(buffer, O_WRONLY);
  if(ERROR == fileHandle)
  {
  	printf("[\033[0;31m Error \033[m]\t Unable to open: %s \n", buffer);
      exit(-1);
  }
  if (direction == GPIO_DIRECTION_OUT)
  {
         // Set out direction
         write(fileHandle, "strong", 6);
  }
  else
  {
         // Set in direction
         write(fileHandle, "pulldown", 8);
  }
  close(fileHandle);


//Open GPIO for Read / Write
  sprintf(buffer, "/sys/class/gpio/gpio%d/value", gpio);
  fileHandle = open(buffer, fileMode);
  if(ERROR == fileHandle)
  {
         printf("[\033[0;31m Error \033[m]\t Unable to open: %s \n", buffer);
         exit(-1);
  }

  return(fileHandle);
}


int writeGPIO(int fileHandle, int val)
{
        if(val ==  0)
        {
               // Set GPIO low status
               write(fileHandle, "0", 1);
        }
        else
        {
               // Set GPIO high status
               write(fileHandle, "1", 1);
        }
        

        return 0;
}
