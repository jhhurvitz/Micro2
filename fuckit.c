
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



int clock(){

	int i2cHandle;

	printf("We're doing things!\n");
	
	int deviceHandle;

	initiateGPIO(GP_I2C);
	i2cHandle = openGPIO(GP_I2C, GPIO_DIRECTION_OUT);
	
	writeGPIO(i2cHandle, 0);
	close(i2cHandle);

	int readStatus;
	int status;

	char buffer[7];
	
	buffer[0] = 0x00;

	int deviceI2CAddress = 0x68;

	if ((deviceHandle = open("/dev/i2c-0", O_RDWR)) < 0){
		printf("Turns out we're not.\n");
		return 1;
	}

	if(ioctl(deviceHandle, I2C_SLAVE, deviceI2CAddress) < 0){
		printf("Error in ioctl\n");
		return 1;
	}
	
        time_t result = time(NULL);
    	
	int sysSeconds, sysMinutes, sysHours, sysYears, sysDays, sysMonths;

	if(result != -1)
	{
		struct tm *timeInfo;

		timeInfo = gmtime(&result);

		sysSeconds = timeInfo->tm_sec;
        	sysMinutes = timeInfo->tm_min;
        	sysHours = (timeInfo->tm_hour)%24;//HOUR

	        sysYears = (timeInfo->tm_year+1900) %100 ;

        	sysDays = timeInfo->tm_mday;
        	sysMonths = (timeInfo->tm_mon+1);
	}
	
	int lowSec = sysSeconds % 10;
	lowSec &= 0x0f;
	int highSec = (sysSeconds/10) << 4;
	highSec &= 0x70;
	int lowMin = sysMinutes % 10;
	lowMin &= 0x0f;
	int highMin = (sysMinutes/10) << 4;
	highMin &= 0x70;

	int miltime = 0b10111111;

	int lowHour = sysHours % 10;
	lowHour &= 0x0f;
	int highHour = (sysHours /10) << 4;
	highHour &= 0x30;
	int lowDay = sysDays % 10;
	lowDay &= 0x0f;
	int highDay = (sysDays /10) <<4;
	highDay &= 0x30;
	int lowMonth = sysMonths % 10;
	lowMonth &= 0x0f;
	int highMonth = (sysMonths / 10) << 4;
	highMonth &= 0x10;
	int lowYear = sysYears % 10;
	lowYear &= 0x0f;
	int highYear = (sysYears / 10 ) << 4;
	highYear &= 0xf0;



	
	buffer[0] = 0x00;	//first word, before we start writing data
        buffer[1] = ( highSec  | lowSec );
        buffer[2] = ( highMin | lowMin); //
	buffer[3] = ( highHour | lowHour);
	buffer[4] = 0x01;
        buffer[5] = (lowDay | highDay); 
        buffer[6] = (lowMonth | highMonth);
        buffer[7] = (lowYear | highYear);

	//Enable these lines to see time data while storing it
	//printf("Date: %d - %d - %d\n", sysYears, sysMonths, sysDays);
	//printf("Time: %d : %d : %d\n", sysHours, sysMinutes, sysSeconds);	

		


	status = write(deviceHandle, buffer, 7);

	if(status != 7){
		printf("Error: more error! (no ack bit)\n");
		return 0;
	}

	buffer[0] = 0x00;
	status = write (deviceHandle, buffer, 1);
	if(status != 1){
		printf("information!\n");
		return -1;
	}else{
		status = read(deviceHandle, buffer, 7);
		if(status != 7){
			printf("Something's gone wrong again.\n");
			return -1;
		}
	}

	int year, month, day, hours, minutes, seconds;



	highSec = (0x70 & buffer[0])>>4;
	lowSec = 0x0f & buffer[0];
	lowMin = 0x0f & buffer[1];
	highMin = (0x70 & buffer[1])>>4;
	lowHour = 0x0f & buffer[2];
	highHour = (0x30 & buffer[2])>>4;
	lowDay = 0x0f & buffer[4];
	highDay = (0x30 & buffer[4])>>4;
	


	lowMonth = 0x0f & buffer[5];
	highMonth = (0x10 & buffer[5])>>4;





	lowYear = 0x0f & buffer[6];
	highYear = (0xf0 & buffer[6])>>4;





	printf("Date: %d%d - %d%d - %d%d\n", highYear,lowYear,highMonth,lowMonth, highDay,lowDay);
	printf("Time: %d%d : %d%d : %d%d\n", highHour,lowHour, highMin,lowMin, highSec, lowSec);

	
	//More basic data to possibly print
	//printf("Date: %d - %d - %d\n", year, month, day);
	//printf("Time: %d : %d : %d\n", hours, minutes, seconds);
	
	/*
	int i;
	for(i = 0; i < 7; i++){
		printf("buffer[%d]: %d\n", i, buffer[i]);
	} 
	*/
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
