
/**
  ADC1 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    adc1.c

  @Summary
    This is the generated header file for the ADC1 driver using Foundation Services Library

  @Description
    This header file provides APIs for driver for ADC1.
    Generation Information :
        Product Revision  :  Foundation Services Library - pic24-dspic-pic32mm : v1.26
        Device            :  PIC24FJ128GB204
    The generated drivers are tested against the following:
        Compiler          :  XC16 1.30
        MPLAB 	          :  MPLAB X 3.45
*/

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/

#include <xc.h>
#include <libpic30.h>
#include "adc1.h"

/**
  Section: Data Type Definitions
*/

/* ADC Driver Hardware Instance Object

  @Summary
    Defines the object required for the maintenance of the hardware instance.

  @Description
    This defines the object required for the maintenance of the hardware
    instance. This object exists once per hardware instance of the peripheral.

 */
typedef struct
{
	uint8_t intSample;
}

ADC_OBJECT;

static ADC_OBJECT adc1_obj;

/**
  Section: Driver Interface
*/


void ADC1_Initialize (void)
{
    // ASAM disabled; DMABM disabled; ADSIDL disabled; DONE disabled; DMAEN disabled; FORM Absolute decimal result, unsigned, right-justified; SAMP disabled; SSRC Clearing sample bit ends sampling and starts conversion; MODE12 10-bit; ADON enabled; 

   AD1CON1 = 0x8000;

    // CSCNA disabled; NVCFG0 AVSS; PVCFG AVDD; ALTS disabled; BUFM disabled; SMPI Generates interrupt after completion of every sample/conversion operation; OFFCAL disabled; BUFREGEN disabled; 

   AD1CON2 = 0x0000;

    // SAMC 0; EXTSAM disabled; PUMPEN disabled; ADRC FOSC/2; ADCS 0; 

   AD1CON3 = 0x0000;

    // CH0SA AN0; CH0SB AN0; CH0NB AVSS; CH0NA AVSS; 

   AD1CHS = 0x0000;

    // CSS31 disabled; CSS30 disabled; CSS29 disabled; CSS28 disabled; CSS27 disabled; 

   AD1CSSH = 0x0000;

    // CSS9 disabled; CSS7 disabled; CSS6 disabled; CSS5 disabled; CSS4 disabled; CSS3 disabled; CSS2 disabled; CSS1 disabled; CSS14 disabled; CSS0 disabled; CSS13 disabled; CSS12 disabled; CSS11 disabled; CSS10 disabled; 

   AD1CSSL = 0x0000;

    // VBG2EN disabled; VBGEN disabled; 

   ANCFG = 0x0000;


   adc1_obj.intSample = AD1CON2bits.SMPI;
   
}

void ADC1_Start(void)
{
   AD1CON1bits.SAMP = 1;
}
void ADC1_Stop(void)
{
   AD1CON1bits.SAMP = 0;
}
uint16_t ADC1_ConversionResultBufferGet(uint16_t *buffer)
{
    int count;
    uint16_t *ADC16Ptr;

    ADC16Ptr = (uint16_t *)&(ADC1BUF0);

    for(count=0;count<=adc1_obj.intSample;count++)
    {
        buffer[count] = (uint16_t)*ADC16Ptr;
        ADC16Ptr++;
    }
    return count;
}
uint16_t ADC1_ConversionResultGet(void)
{
    return ADC1BUF0;
}
bool ADC1_IsConversionComplete( void )
{
    return AD1CON1bits.DONE; //Wait for conversion to complete   
}
void ADC1_ChannelSelect( ADC1_CHANNEL channel )
{
    AD1CHS = channel;
}

uint16_t ADC1_GetConversion(ADC1_CHANNEL channel)
{
    ADC1_ChannelSelect(channel);
    
    ADC1_Start();
    ADC1_Stop();
    
    while (!ADC1_IsConversionComplete())
    {
    }
       
    return ADC1_ConversionResultGet();
}


void ADC1_Tasks ( void )
{
    // clear the ADC interrupt flag
    IFS0bits.AD1IF = false;
}

uint16_t ADC1_GetVbat(){
    uint16_t conversion;
    int i = 0;
    //ADC1_Initialize();
    ADC1_ChannelSelect(Vbat);
    ADC1_Start();
    //Provide Delay
    __delay_ms(2);
    ADC1_Stop();
    while(!ADC1_IsConversionComplete())
    {
        ADC1_Tasks();   
    }
    conversion = ADC1_ConversionResultGet();
    return conversion;
}

uint16_t ADC1_GetVpv(){
    uint16_t conversion;
    int i = 0;
    //ADC1_Initialize();
    ADC1_ChannelSelect(Vpv);
    ADC1_Start();
    //Provide Delay
    __delay_ms(2);
    ADC1_Stop();
    while(!ADC1_IsConversionComplete())
    {
        ADC1_Tasks();   
    }
    conversion = ADC1_ConversionResultGet();
    return conversion;
}

//Sorting function

// sort function (Author: Bill Gentles, Nov. 12, 2010)

void isort(int *a, int n) {
    int i;
  //  *a is an array pointer function
  for (i = 1; i < n; ++i)
  {
    int j = a[i];
    int k;
    for (k = i - 1; (k >= 0) && (j < a[k]); k--)
    {
      a[k + 1] = a[k];
    }
    a[k + 1] = j;
  }
}

//Mode function, returning the mode or median.

int mode(int *x, int n) {
  int i = 0;
  int count = 0;
  int maxCount = 0;
  int mode = 0;
  int bimodal;
  int prevCount = 0;
  while (i < (n - 1)) {
    prevCount = count;
    count = 0;
    while (x[i] == x[i + 1]) {
      count++;
      i++;
    }
    if (count > prevCount && count > maxCount) {
      mode = x[i];
      maxCount = count;
      bimodal = 0;
    }
    if (count == 0) {
      i++;
    }
    if (count == maxCount) { //If the dataset has 2 or more modes.
      bimodal = 1;
    }
    if (mode == 0 || bimodal == 1) { //Return the median if there is no mode.
      mode = x[(n / 2)];
    }
    return mode;
  }

}
/**
  End of File
*/
