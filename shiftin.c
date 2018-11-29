#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>  
#include <stdlib.h>  
#include <stdint.h>
#include <string.h>
#include <time.h>
 
 
#define NUMBER_OF_SHIFT_CHIPS   4
#define DATA_WIDTH   NUMBER_OF_SHIFT_CHIPS * 8
#define PULSE_WIDTH_USEC   1
#define POLL_DELAY_MSEC   1
#define BYTES_VAL_T uint32_t

int ploadPin        = 34;  // Connects to Parallel load pin the 165
int clockEnablePin  = 35;  // Connects to Clock Enable pin the 165
int dataPin         = 33; // Connects to the Q7 pin the 165
int clockPin        = 32; // Connects to the Clock pin the 165

BYTES_VAL_T pinValues;
BYTES_VAL_T oldPinValues;

BYTES_VAL_T read_shift_regs()
{
    long bitVal;
    BYTES_VAL_T bytesVal = 0;

    // load
    digitalWrite(clockEnablePin, HIGH);
    digitalWrite(ploadPin, LOW);
    delayMicroseconds(PULSE_WIDTH_USEC);
    digitalWrite(ploadPin, HIGH);
    digitalWrite(clockEnablePin, LOW);
    delayMicroseconds(PULSE_WIDTH_USEC);
    // shiftin
    for(int i = 0; i < DATA_WIDTH; i++)
    {
        bitVal = digitalRead(dataPin);

        bytesVal |= (bitVal << ((DATA_WIDTH-1) - i));

        digitalWrite(clockPin, HIGH);
        delayMicroseconds(PULSE_WIDTH_USEC);
        digitalWrite(clockPin, LOW);
    }

    return(bytesVal);
}

void display_pin_values()
{

    for(int i = 0; i < DATA_WIDTH; i++)
    {
        printf(" ");

        if((pinValues >> i) & 1)
            printf("1");
        else
            printf("0");

    }

    printf("\n");
}

int main(void)
{
    // setup GPIO, this uses actual BCM pin numbers 
    wiringPiSetupGpio();
    
    pinMode(ploadPin, OUTPUT);
    pinMode(clockEnablePin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    pinMode(dataPin, INPUT);

    digitalWrite(clockPin, LOW);
    digitalWrite(ploadPin, HIGH);

    pinValues = read_shift_regs();
    display_pin_values();
    oldPinValues = pinValues;
   
    for (;;) {
        pinValues = read_shift_regs();
        if(pinValues != oldPinValues)
        {
            display_pin_values();
            oldPinValues = pinValues;
        }

        delay(POLL_DELAY_MSEC);

    }

    return 0;
}


