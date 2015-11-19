#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define Strobe     (26) // IO8
#define GP_4       (28) // IO4
#define GP_5       (17) // IO5
#define GP_6       (24) // IO6
#define GP_7       (27) // IO7
#define GPIO_DIRECTION_IN      (1)
#define GPIO_DIRECTION_OUT     (0)
#define ERROR                  (-1)
#define MSG_RESET 0
#define MSG_PING 1
#define MSG_GET 2
#define MSG_ACK 14

int fileHandleGPIO_4;
int fileHandleGPIO_5;
int fileHandleGPIO_6;
int fileHandleGPIO_7;
int fileHandleGPIO_S;

//open GPIO and set the direction
int openGPIO(int gpio, int direction)
{
        char buffer[256];
        int fileHandle;
        int fileMode;

        //Export GPIO
        fileHandle = open("/sys/class/gpio/export", O_WRONLY);
        if (ERROR == fileHandle)
        {
                puts("Error: Unable to open /sys/class/gpio/export");
                return(-1);
        }
        sprintf(buffer, "%d", gpio);
        write(fileHandle, buffer, strlen(buffer));
        close(fileHandle);

        //Direction GPIO
        sprintf(buffer, "/sys/class/gpio/gpio%d/direction", gpio);
        fileHandle = open(buffer, O_WRONLY);
        if (ERROR == fileHandle)
        {
                puts("Unable to open file:");
                puts(buffer);
                return(-1);
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


        //Open GPIO for Read / Write
        sprintf(buffer, "/sys/class/gpio/gpio%d/value", gpio);
        fileHandle = open(buffer, fileMode);
        if (ERROR == fileHandle)
        {
                puts("Unable to open file:");
                puts(buffer);
                return(-1);
        }

        return(fileHandle);  //This file handle will be used in read/write and close operations.
}

int closeGPIO(int gpio, int fileHandle)
{
        char buffer[256];

        close(fileHandle); //This is the file handle of opened GPIO for Read / Write earlier.

        fileHandle = open("/sys/class/gpio/unexport", O_WRONLY);
        if(ERROR == fileHandle)
        {
               puts("Unable to open file:");
               puts(buffer);
               return(-1);
        }
        sprintf(buffer, "%d", gpio);
        write(fileHandle, buffer, strlen(buffer));
        close(fileHandle);

        return(0);
}

//write value
int writeGPIO(int fHandle, int val)
{
        if (val == 0)
        {
                // Set GPIO low status
                write(fHandle, "0", 1);
        }
        else
        {
                // Set GPIO high status
                write(fHandle, "1", 1);
        }

        return(0);
}

int openFileForReading(gpio)
{
    char buffer[256];
    int fileHandle;

    sprintf(buffer, "/sys/class/gpio/gpio%d/value", gpio);

    fileHandle = open(buffer, O_RDONLY);
    if(ERROR == fileHandle)
    {
           puts("Unable to open file:");
           puts(buffer);
           return(-1);
    }
    return(fileHandle);  //This file handle will be used in read/write and close operations.
}

int readGPIO(int fileHandle)
{
        char  value;

        read(fileHandle, &value, 1);

        if('0' == value)
        {
             // Current GPIO status low
               value = 0;
        }
        else
        {
             // Current GPIO status high
               value = 1;
        }
        return value;
}

void ping()
{
        printf("Begin Ping\n");
        writeGPIO(fileHandleGPIO_4, 1); //LSB
        writeGPIO(fileHandleGPIO_5, 0);
        writeGPIO(fileHandleGPIO_6, 0);
        writeGPIO(fileHandleGPIO_7, 0); //MSB

        sleep(1);

        writeGPIO(fileHandleGPIO_S, 0);

        sleep(1);

        printf("Recieving Ping\n");

        writeGPIO(fileHandleGPIO_4, 0); //LSB
        writeGPIO(fileHandleGPIO_5, 0);
        writeGPIO(fileHandleGPIO_6, 0);
        writeGPIO(fileHandleGPIO_7, 0); //MSB

        closeGPIO(GP_4,fileHandleGPIO_4);
        closeGPIO(GP_5,fileHandleGPIO_5);
        closeGPIO(GP_6,fileHandleGPIO_6);
        closeGPIO(GP_7,fileHandleGPIO_7);

        fileHandleGPIO_4 = openGPIO(GP_4, GPIO_DIRECTION_IN);
        fileHandleGPIO_5 = openGPIO(GP_5, GPIO_DIRECTION_IN);
        fileHandleGPIO_6 = openGPIO(GP_6, GPIO_DIRECTION_IN);
        fileHandleGPIO_7 = openGPIO(GP_7, GPIO_DIRECTION_IN);

        sleep(1);

        if(ReadAndConvertVals() == MSG_ACK)
        {
                printf("Ping Successful\n");
        }
        else
        {
                printf("Ping Unsuccessful\n");
        }
}

void reset()
{
        printf("Begin Reset\n");
        writeGPIO(fileHandleGPIO_4, 0); //LSB
        writeGPIO(fileHandleGPIO_5, 0);
        writeGPIO(fileHandleGPIO_6, 0);
        writeGPIO(fileHandleGPIO_7, 0); //MSB

        sleep(1);

        writeGPIO(fileHandleGPIO_S, 0);

        sleep(1);

        printf("Recieving Reset Confirmation\n");

        //TEST!!!!! FOR RESETTING THE VALUES TO ZERO BEFORE SWITCH


        writeGPIO(fileHandleGPIO_4, 0); //LSB
        writeGPIO(fileHandleGPIO_5, 0);
        writeGPIO(fileHandleGPIO_6, 0);
        writeGPIO(fileHandleGPIO_7, 0); //MSB



        closeGPIO(GP_4,fileHandleGPIO_4);
        closeGPIO(GP_5,fileHandleGPIO_5);
        closeGPIO(GP_6,fileHandleGPIO_6);
        closeGPIO(GP_7,fileHandleGPIO_7);

        fileHandleGPIO_4 = openGPIO(GP_4, GPIO_DIRECTION_IN);
        fileHandleGPIO_5 = openGPIO(GP_5, GPIO_DIRECTION_IN);
        fileHandleGPIO_6 = openGPIO(GP_6, GPIO_DIRECTION_IN);
        fileHandleGPIO_7 = openGPIO(GP_7, GPIO_DIRECTION_IN);

        sleep(1);

        if(ReadAndConvertVals() == MSG_ACK)
        {
                printf("Reset Successful\n");
        }
        else
        {
                printf("Reset Unsuccessful\n");
        }
}

void get()
{
        int GetVal = 0;
        printf("Sending Get Command\n");
        writeGPIO(fileHandleGPIO_4, 0); //LSB
        writeGPIO(fileHandleGPIO_5, 1);
        writeGPIO(fileHandleGPIO_6, 0);
        writeGPIO(fileHandleGPIO_7, 0); //MSB

        sleep(1);

        writeGPIO(fileHandleGPIO_S, 0);//PIC reads command

        sleep(1);

        writeGPIO(fileHandleGPIO_4, 0); //LSB
        writeGPIO(fileHandleGPIO_5, 0);
        writeGPIO(fileHandleGPIO_6, 0);
        writeGPIO(fileHandleGPIO_7, 0); //MSB

        closeGPIO(GP_4,fileHandleGPIO_4);
        closeGPIO(GP_5,fileHandleGPIO_5);
        closeGPIO(GP_6,fileHandleGPIO_6);
        closeGPIO(GP_7,fileHandleGPIO_7);

        sleep(1);

        writeGPIO(fileHandleGPIO_S, 1);
        sleep(1);//Wait for pic to output

        fileHandleGPIO_4 = openGPIO(GP_4, GPIO_DIRECTION_IN);
        fileHandleGPIO_5 = openGPIO(GP_5, GPIO_DIRECTION_IN);
        fileHandleGPIO_6 = openGPIO(GP_6, GPIO_DIRECTION_IN);
        fileHandleGPIO_7 = openGPIO(GP_7, GPIO_DIRECTION_IN);

        sleep(1);

        printf("Recieving First Nibble\n");
        GetVal = ReadAndConvertVals();

        closeGPIO(GP_4,fileHandleGPIO_4);
        closeGPIO(GP_5,fileHandleGPIO_5);
        closeGPIO(GP_6,fileHandleGPIO_6);
        closeGPIO(GP_7,fileHandleGPIO_7);

        sleep(1);

        writeGPIO(fileHandleGPIO_S, 0);

        sleep(1);

        fileHandleGPIO_4 = openGPIO(GP_4, GPIO_DIRECTION_IN);
        fileHandleGPIO_5 = openGPIO(GP_5, GPIO_DIRECTION_IN);
        fileHandleGPIO_6 = openGPIO(GP_6, GPIO_DIRECTION_IN);
        fileHandleGPIO_7 = openGPIO(GP_7, GPIO_DIRECTION_IN);

        sleep(1);

        printf("Recieving Second Nibble\n");
        GetVal += ReadAndConvertVals2();

        closeGPIO(GP_4,fileHandleGPIO_4);
        closeGPIO(GP_5,fileHandleGPIO_5);
        closeGPIO(GP_6,fileHandleGPIO_6);
        closeGPIO(GP_7,fileHandleGPIO_7);

        writeGPIO(fileHandleGPIO_S, 1);
        sleep(1);

        fileHandleGPIO_4 = openGPIO(GP_4, GPIO_DIRECTION_IN);
        fileHandleGPIO_5 = openGPIO(GP_5, GPIO_DIRECTION_IN);
        fileHandleGPIO_6 = openGPIO(GP_6, GPIO_DIRECTION_IN);
        fileHandleGPIO_7 = openGPIO(GP_7, GPIO_DIRECTION_IN);

        sleep(1);

        printf("Recieving Last Crumb\n");
        GetVal += ReadAndConvertVals3();

        closeGPIO(GP_4,fileHandleGPIO_4);
        closeGPIO(GP_5,fileHandleGPIO_5);
        closeGPIO(GP_6,fileHandleGPIO_6);
        closeGPIO(GP_7,fileHandleGPIO_7);
        sleep(1);

        writeGPIO(fileHandleGPIO_S, 0);
        sleep(1);
        fileHandleGPIO_4 = openGPIO(GP_4, GPIO_DIRECTION_IN);
        fileHandleGPIO_5 = openGPIO(GP_5, GPIO_DIRECTION_IN);
        fileHandleGPIO_6 = openGPIO(GP_6, GPIO_DIRECTION_IN);
        fileHandleGPIO_7 = openGPIO(GP_7, GPIO_DIRECTION_IN);

        double volts;
        volts = (GetVal / 1023.0) * 5.0;

        if(ReadAndConvertVals() == 14)
        {
                printf("ADC Value Acquisition Complete\n");
                printf("ADC Value = %d\n", GetVal);
                printf("Volts: %lf\n", volts);
        }
        else
        {
                printf("ADC Value Acquisition Failed\n");
        }
        return;
}

int ReadAndConvertVals()
{
        int value = 0;

        if (readGPIO(fileHandleGPIO_4) == 1)
        {
                value += 1;
        }
        if (readGPIO(fileHandleGPIO_5) == 1)
        {
                value += 2;
        }
        if (readGPIO(fileHandleGPIO_6) == 1)
        {
                value += 4;
        }
        if (readGPIO(fileHandleGPIO_7) == 1)
        {
                value += 8;
        }

        printf("value = %d\n", value);

        return value;
}

int ReadAndConvertVals2()
{
        int value = 0;
        if (readGPIO(fileHandleGPIO_4) == 1)
        {
                value += 16;
        }
        if (readGPIO(fileHandleGPIO_5) == 1)
        {
                value += 32;
        }
        if (readGPIO(fileHandleGPIO_6) == 1)
        {
                value += 64;
        }
        if (readGPIO(fileHandleGPIO_7) == 1)
        {
                value += 128;
        }
        printf("Second Nibble: %d\n", value);
        return value;
}

int ReadAndConvertVals3()
{
        int value = 0;
        if (readGPIO(fileHandleGPIO_4) == 1)
        {
                value += 256;
        }
        if (readGPIO(fileHandleGPIO_5) == 1)
        {
                value += 512;
        }
        printf("Last Crumb = %d\n", value);
        return value;
}

int main()
{
        fileHandleGPIO_S = openGPIO(Strobe, GPIO_DIRECTION_OUT);

        while(1)
        {
                fileHandleGPIO_4 = openGPIO(GP_4, GPIO_DIRECTION_OUT);
                fileHandleGPIO_5 = openGPIO(GP_5, GPIO_DIRECTION_OUT);
                fileHandleGPIO_6 = openGPIO(GP_6, GPIO_DIRECTION_OUT);
                fileHandleGPIO_7 = openGPIO(GP_7, GPIO_DIRECTION_OUT);

                writeGPIO(fileHandleGPIO_S, 1);

                char input[10];
                printf("Enter Command:\n");
                scanf("%s",&input);
                if(strcmp(input,"ping") == 0)
                {
                        ping();
                }
                else if(strcmp(input,"reset") == 0)
                {
                        reset();
                }
                else if(strcmp(input,"get") == 0)
                {
                        get();
                }
                else
                {
                printf("Invalid Command \n");
                }

                closeGPIO(GP_4, fileHandleGPIO_4);
                closeGPIO(GP_5, fileHandleGPIO_5);
                closeGPIO(GP_6, fileHandleGPIO_6);
                closeGPIO(GP_7, fileHandleGPIO_7);
                sleep(1);
        }
        return 0;
}
