/**
  System Interrupts Generated Driver File 

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.c

  @Summary:
    This is the generated manager file for the MPLAB(c) Code Configurator device.  This manager
    configures the pins direction, initial state, analog setting.
    The peripheral pin select, PPS, configuration is also handled by this manager.

  @Description:
    This source file provides implementations for MPLAB(c) Code Configurator interrupts.
    Generation Information : 
        Product Revision  :  MPLAB(c) Code Configurator - pic24-dspic-pic32mm : 1.75.1
        Device            :  PIC24FJ128GB204
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.35
        MPLAB             :  MPLAB X v5.05

    Copyright (c) 2013 - 2015 released Microchip Technology Inc.  All rights reserved.

    Microchip licenses to you the right to use, modify, copy and distribute
    Software only when embedded on a Microchip microcontroller or digital signal
    controller that is integrated into your product or third party product
    (pursuant to the sublicense terms in the accompanying license agreement).

    You should refer to the license agreement accompanying this Software for
    additional information regarding your rights and obligations.

    SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
    EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
    MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
    IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
    CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
    OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
    INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
    CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
    SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
    (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

*/


/**
    Section: Includes
*/
#include <xc.h>
#include "pin_manager.h"
#include "../globales.h"
/**
    void PIN_MANAGER_Initialize(void)
*/
void PIN_MANAGER_Initialize(void)
{
    /****************************************************************************
     * Setting the Output Latch SFR(s)
     ***************************************************************************/
    LATA = 0x0000;
    LATB = 0x0488;
    LATC = 0x0000;

    /****************************************************************************
     * Setting the GPIO Direction SFR(s)
     ***************************************************************************/
    TRISA = 0x079D;
    TRISB = 0xAA7F;
    TRISC = 0x0008;
    
    //_TRISA1 = 0;

    /****************************************************************************
     * Setting the Weak Pull Up and Weak Pull Down SFR(s)
     ***************************************************************************/
    CNPD1 = 0x0000;
    CNPD2 = 0x0000;
    CNPD3 = 0x0000;
    CNPU1 = 0x0000;
    CNPU2 = 0x0000;
    CNPU3 = 0x0000;

    /****************************************************************************
     * Setting the Open Drain SFR(s)
     ***************************************************************************/
    ODCA = 0x0000;
    ODCB = 0x0000;
    ODCC = 0x0000;

    /****************************************************************************
     * Setting the Analog/Digital Configuration SFR(s)
     ***************************************************************************/
    ANSA = 0x000D;
    ANSB = 0xA240;
    ANSC = 0x0000;


    /****************************************************************************
     * Set the PPS
     ***************************************************************************/
    __builtin_write_OSCCONL(OSCCON & 0xbf); // unlock PPS

    RPOR5bits.RP10R = 0x0003;   //RB10->UART1:U1TX;
    RPINR18bits.U1RXR = 0x000B;   //RB11->UART1:U1RX;

    __builtin_write_OSCCONL(OSCCON | 0x40); // lock   PPS

    /****************************************************************************
     * Interrupt On Change for group CNEN1 - any
     ***************************************************************************/
	CNEN1bits.CN0IE = 1; // Pin : RA4

    /****************************************************************************
     * Interrupt On Change for group CNEN2 - any
     ***************************************************************************/
	CNEN2bits.CN28IE = 1; // Pin : RC3

    /****************************************************************************
     * Interrupt On Change for group CNEN3 - any
     ***************************************************************************/
	CNEN3bits.CN35IE = 1; // Pin : RA9

    IEC1bits.CNIE = 1; // Enable CNI interrupt 
}

/* Interrupt service routine for the CNI interrupt. */
void __attribute__ (( interrupt, no_auto_psv )) _CNInterrupt ( void )
{
    if(IFS1bits.CNIF == 1)
    {
        DigitalInputHandler();
        delayms(16);
        //LED3_Toggle();
        // Clear the flag
        IFS1bits.CNIF = 0;
    }
}

void DigitalInputHandler(){
    char contador;
    int k;
    reinicio = 0;
    button_state = !BTN_IN_GetValue();
    while(!BTN_IN_GetValue()){
        delayms(50);
        contador++;
        if(contador > 100){
            //printf("Reseteo OPI\r");
            /*HEARTBEAT_SetLow();
            delayms(2000);
            HEARTBEAT_SetHigh();*/
            reinicio = 1;
            state_gpio =  (button_state ) + (gate_state << 1) + (aux_state << 2) + (reinicio << 3);
            MUX_CTRL_SetLow();
            while(k < 8)
            {
                delayms(300);
                k++;
            }
            MUX_CTRL_SetHigh();
            contador = 0;
            k = 0;
            while(k < 20)
            {
                delayms(300);
                k++;
            }
            RCONbits.SWDTEN = 1;
        }
    }
    gate_state = !PUERTA_GetValue();
    aux_state = !DIN_AUX_GetValue();
    //printf("--%d--%d--\r", gate_state, gate_state_last);
    delayms(50);
    if(gate_state != gate_state_last){
        estado_puerta = 1;  
        //printf("PUERTA\r");
    }
    //printf("status button--%d--\r", button_state);
    //printf("status reset--%d--\r", reinicio);
    state_gpio =  (button_state ) + (gate_state << 1) + (aux_state << 2) + (reinicio << 3);
    //printf("GPIO--%d--\r", state_gpio);
    if(button_state)
    {
        button_pushed = 1;
		if(!gmsEnable)
		{
        button_state=0;
        delayms(5);
        //printf("status button--%d--\r", button_state);
        state_gpio =  (button_state ) + (gate_state << 1) + (aux_state << 2) + (reinicio << 3);
        delayms(5);
        //printf("GPIO--%d--\r", state_gpio);
		}
    }
}