
/**
  TMR3 Generated Driver API Source File 

  @Company
    Microchip Technology Inc.

  @File Name
    tmr3.c

  @Summary
    This is the generated source file for the TMR3 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This source file provides APIs for driver for TMR3. 
    Generation Information : 
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.75.1
        Device            :  PIC24FJ128GB204
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.35
        MPLAB             :  MPLAB X v5.05
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
#include "tmr3.h"
#include "pin_manager.h"
#include "i2c2.h"
#include "../globales.h"

/**
  Section: Data Type Definitions
*/

/** TMR Driver Hardware Instance Object

  @Summary
    Defines the object required for the maintainence of the hardware instance.

  @Description
    This defines the object required for the maintainence of the hardware
    instance. This object exists once per hardware instance of the peripheral.

  Remarks:
    None.
*/

typedef struct _TMR_OBJ_STRUCT
{
    /* Timer Elapsed */
    bool                                                    timerElapsed;
    /*Software Counter value*/
    uint8_t                                                 count;

} TMR_OBJ;

static TMR_OBJ tmr3_obj;

/**
  Section: Driver Interface
*/

void TMR3_Initialize (void)
{
    //TMR3 0; 
    TMR3 = 0x00;
    //Period = 0.01 s; Frequency = 4000000 Hz; PR3 625; 
    PR3 = 0x271;
    //TCKPS 1:64; TON enabled; TSIDL disabled; TCS FOSC/2; TECS SOSC; TGATE disabled; 
    T3CON = 0x8020;

    
    IFS0bits.T3IF = false;
    IEC0bits.T3IE = true;
	
    tmr3_obj.timerElapsed = false;

}



void __attribute__ ( ( interrupt, no_auto_psv ) ) _T3Interrupt (  )
{
    /* Check if the Timer Interrupt/Status is set */

    //***User Area Begin
    static volatile unsigned int CountCallBack = 0;

    // callback function - called every 25th pass
    if (++CountCallBack >= TMR3_INTERRUPT_TICKER_FACTOR)
    {
        // ticker function call
        TMR3_CallBack();

        // reset ticker counter
        CountCallBack = 0;
    }

    //***User Area End

    tmr3_obj.count++;
    tmr3_obj.timerElapsed = true;
    IFS0bits.T3IF = false;
}


void TMR3_Period16BitSet( uint16_t value )
{
    /* Update the counter values */
    PR3 = value;
    /* Reset the status information */
    tmr3_obj.timerElapsed = false;
}

uint16_t TMR3_Period16BitGet( void )
{
    return( PR3 );
}

void TMR3_Counter16BitSet ( uint16_t value )
{
    /* Update the counter values */
    TMR3 = value;
    /* Reset the status information */
    tmr3_obj.timerElapsed = false;
}

uint16_t TMR3_Counter16BitGet( void )
{
    return( TMR3 );
}

/*
typedef enum{
    _05_HZ = 8,
    _1_HZ = 4,
    _2_HZ = 2,
    _4_HZ = 1,
    _LED_ON = 3,
    _LED_OFF = 0,
} LED_HZ;    
*/
LED_HZ led_button_period = _1_HZ;
LED_HZ led_status_period = _2_HZ;
LED_HZ led_warning_period = _1_HZ; //4

int tmrCnt = 0;

/*
void __attribute__ ((weak)) TMR3_CallBack(void)
{
    
    if(!(tmrCnt % led_button_period))
        LED_BTN_CTR_Toggle();
    if(!(tmrCnt % led_status_period))
        LED1_Toggle();
    if(!(tmrCnt % led_warning_period))
        LED2_Toggle();
    if(!(tmrCnt % 8)){
        tmrCnt = 0;
    }
    tmrCnt++;
        
}
*/

void __attribute__ ((weak)) TMR3_CallBack(void)
{
    switch(led_button_period){
        case _LED_ON:
            LED_BTN_CTR_SetHigh();
            break;
        case _LED_OFF:
            LED_BTN_CTR_SetLow();
            break;
        default:
            if(!(tmrCnt % led_button_period))
                LED_BTN_CTR_Toggle();
        break;
    }
    switch(led_status_period){
        case _LED_ON:
            LED1_SetHigh();
            break;
        case _LED_OFF:
            LED1_SetLow();
            break;
        default:
            if(!(tmrCnt % 2))
                LED1_Toggle();
        break;
    }
    switch(led_warning_period){
        case _LED_ON:
            LED2_SetHigh();
            break;
        case _LED_OFF:
            LED2_SetLow();
            break;
        default:
            if(!(tmrCnt % led_warning_period))
                LED2_Toggle();
        break;        
    }
    if(!(tmrCnt % 4)){
        maxTiempo++;
        secTicker++;
    }
    if(!(tmrCnt % 8)){
        tmrCnt = 0;
    }
    tmrCnt++;        
}


void TMR3_Start( void )
{
    /* Reset the status information */
    tmr3_obj.timerElapsed = false;

    /*Enable the interrupt*/
    IEC0bits.T3IE = true;

    /* Start the Timer */
    T3CONbits.TON = 1;
}

void TMR3_Stop( void )
{
    /* Stop the Timer */
    T3CONbits.TON = false;

    /*Disable the interrupt*/
    IEC0bits.T3IE = false;
}

bool TMR3_GetElapsedThenClear(void)
{
    bool status;
    
    status = tmr3_obj.timerElapsed;

    if(status == true)
    {
        tmr3_obj.timerElapsed = false;
    }
    return status;
}

int TMR3_SoftwareCounterGet(void)
{
    return tmr3_obj.count;
}

void TMR3_SoftwareCounterClear(void)
{
    tmr3_obj.count = 0; 
}

/**
 End of File
*/
