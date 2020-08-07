
/**
  I2C2 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    i2c2.c

  @Summary
    This is the generated header file for the i2c2 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This header file provides APIs for driver for i2c2.
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

#include <p24FJ128GB204.h>

#include "i2c2.h"
#include "../globales.h"
#include "uart1.h"
#include "pin_manager.h"
#include "memory/flash_demo.h"
/**
 Section: Data Types
*/

/**
  I2C Slave Driver State Enumeration

  @Summary
    Defines the different states of the i2c slave.

  @Description
    This defines the different states that the i2c slave
    used to process transactions on the i2c bus.
*/
typedef enum
{
    S_SLAVE_IDLE,
    S_SLAVE_RECEIVE_MODE,
    S_SLAVE_TRANSMIT_MODE,
} I2C_SLAVE_STATES;


/**
 Section: Macro Definitions
*/
/* defined for I2C2 */
#define I2C2_TRANSMIT_REG                       I2C2TRN	// Defines the transmit register used to send data.
#define I2C2_RECEIVE_REG                        I2C2RCV	// Defines the receive register used to receive data.

#define I2C2_MASK_REG                           I2C2MSK	// Defines the address mask register.
#define I2C2_ADDRESS_REG                        I2C2ADD	// Defines the address register. 

// The following control bits are used in the I2C state machine to manage
// the I2C module and determine next states.
#define I2C2_GENERAL_CALL_ENABLE_BIT            I2C2CONLbits.GCEN	// I2C General Call enable control bit.
#define I2C2_10_BIT_ADDRESS_ENABLE_BIT          I2C2CONLbits.A10M	// I2C Address Mode (7 or 10 bit address) control bit.
#define I2C2_RELEASE_SCL_CLOCK_CONTROL_BIT      I2C2CONLbits.SCLREL	// I2C clock stretch/release control bit.

#define I2C2_ACKNOLAGE_DATA_BIT                 I2C2CONLbits.ACKDT
// The following status bits are used in the I2C state machine to determine
// the next states.

#define I2C2_READ_NOT_WRITE_STATUS_BIT          I2C2STATbits.R_W    // I2C current transaction read/write status bit.
#define I2C2_DATA_NOT_ADDRESS_STATUS_BIT        I2C2STATbits.D_A    // I2C last byte receive was data/address status bit.
#define I2C2_RECEIVE_OVERFLOW_STATUS_BIT        I2C2STATbits.I2COV	// I2C receive buffer overflow status bit.
#define I2C2_GENERAL_CALL_ADDRESS_STATUS_BIT    I2C2STATbits.GCSTAT	// I2C General Call status bit.
#define I2C2_ACKNOWLEDGE_STATUS_BIT             I2C2STATbits.ACKSTAT	// I2C ACK status bit.

/**
 Section: Local Functions
*/

inline void __attribute__ ((always_inline)) I2C2_TransmitProcess(void);
inline void __attribute__ ((always_inline)) I2C2_ReceiveProcess(void);
int requestHandler( REG_SELECT rs);
int gpioHandler(int gpio, int operation);

/**
 Section: Local Variables
*/

static I2C_SLAVE_STATES   i2c2_slave_state;
static uint8_t            *p_i2c2_write_pointer;
static uint8_t            *p_i2c2_read_pointer;

static REG_SELECT         regsel;

/**
  Prototype:        void I2C2_Initialize(void)
  Input:            none
  Output:           none
  Description:      I2C2_Initialize is an
                    initialization routine that takes inputs from the GUI.
  Comment:          
  Usage:            I2C2_Initialize();
*/
void I2C2_Initialize(void)
{
    // initialize the hardware
    // ACKEN disabled; STRICT disabled; STREN disabled; GCEN disabled; SMEN disabled; DISSLW enabled; I2CSIDL disabled; ACKDT Sends ACK; SCLREL Holds; RSEN disabled; A10M 7 Bit; PEN disabled; RCEN disabled; SEN disabled; I2CEN enabled; 
    I2C2CONL = 0x8000;
     
    I2C2CONH = 0x00;
    // BCL disabled; D_nA disabled; R_nW disabled; P disabled; S disabled; I2COV disabled; IWCOL disabled;
    I2C2STAT = 0x00;
    
    // ADD 32; 
    I2C2_SlaveAddressSet(0x20);
    // MSK 0; 
    I2C2_SlaveAddressMaskSet(0x00);

    // make sure this is set first
    i2c2_slave_state = S_SLAVE_IDLE;
    
    I2C2_ReadPointerSet(NULL);
    I2C2_WritePointerSet(NULL);
    
    /* SI2C2 - I2C2 Slave Events */
    // clear the master interrupt flag
    IFS3bits.SI2C2IF = 0;
    // enable the master interrupt
    IEC3bits.SI2C2IE = 1;
}


void I2C2_ReadPointerSet(uint8_t *p)
{
    p_i2c2_read_pointer = p;
}

void I2C2_WritePointerSet(uint8_t *p)
{
    p_i2c2_write_pointer = p;
}

uint8_t *I2C2_ReadPointerGet(void)
{
    return (p_i2c2_read_pointer);
}

uint8_t *I2C2_WritePointerGet(void)
{
    return (p_i2c2_write_pointer);
}

void I2C2_SlaveAddressMaskSet(
                                uint16_t mask)
{
    I2C2_MASK_REG = mask;
}

void I2C2_SlaveAddressSet(
                                uint16_t address)
{
    if (address > 0x7F)
    {
        // use 10 bit address
        I2C2_10_BIT_ADDRESS_ENABLE_BIT = true;
    }
    else
    {
        // use 7 bit address
        I2C2_10_BIT_ADDRESS_ENABLE_BIT = false;
    }
    i2c2_slave_state = S_SLAVE_IDLE;
    I2C2_ADDRESS_REG = address;

}

inline void __attribute__ ((always_inline)) I2C2_TransmitProcess(void)
{
    // get the data to be transmitted

    // sanity check (to avoid stress)
    if (p_i2c2_read_pointer == NULL)
        return;

    I2C2_TRANSMIT_REG = *p_i2c2_read_pointer;

    // set the SCL clock to be released
    I2C2_RELEASE_SCL_CLOCK_CONTROL_BIT = 1;

}

inline void __attribute__ ((always_inline)) I2C2_ReceiveProcess(void)
{   
    // store the received data 
    // sanity check (to avoid stress)
    if (p_i2c2_write_pointer == NULL)
        return;

    *p_i2c2_write_pointer = I2C2_RECEIVE_REG;

}


/* --------------- Declaracion de variables de la libreria I2C ------------- */
int dummy;
int RS;
int gpio;
int operation;
int response[2];
int ind;

//I2C Interrupt routine
void __attribute__((interrupt, no_auto_psv)) _SI2C2Interrupt(void)
{
    //if(I2C2STATbits.BCL == 1) 
      //  dummy = 0;
    switch(i2c2_slave_state){
        case S_SLAVE_IDLE:
        case S_SLAVE_RECEIVE_MODE:             //IDLE/RCV MODE
            if(I2C2STATbits.S == 1) I2C2CONLbits.ACKDT = 0; //ERRATA
            if (I2C2STATbits.D_A == 0) {	//ADDRESS
                if(I2C2STATbits.R_W == 0){  //WRITE TO SLAVE (slave read)
                    i2c2_slave_state = S_SLAVE_RECEIVE_MODE;
                    while(!I2C2STATbits.RBF);
                    dummy = I2C2RCV;        //RCV ADDRESS
                    while(I2C2STATbits.RBF);
                }
                else{                       //READ FROM SLAVE (slave transmit)
                        while(!I2C2STATbits.RBF);
                        dummy = I2C2RCV;        //RCV ADDRESS
                        while(I2C2STATbits.RBF);
                        while(I2C2STATbits.TBF);
                        I2C2TRN = response[0];          //TRANSMIT
                        while(!I2C2STATbits.TBF);
                        I2C2CONLbits.SCLREL = 1;
                        while(I2C2STATbits.RBF); //*************************
                        while(I2C2STATbits.ACKTIM);
                        i2c2_slave_state = S_SLAVE_TRANSMIT_MODE;   
                }
                ind = 0;
            }
            if (i2c2_slave_state == S_SLAVE_RECEIVE_MODE){                      //RCV MODE
                if (I2C2STATbits.D_A == 1){ //DATA
                    if(I2C2STATbits.I2COV != 1){
                        if(I2C2STATbits.R_W == 0){//WRITE (master ---->esclavo)
                            if(ind == 0){
                                while(!I2C2STATbits.RBF);
                                RS = I2C2RCV;
                                while(I2C2STATbits.RBF);
                                ind++;
                                requestHandler(RS);//<--------handler
                            }else if(ind == 1){
                                while(!I2C2STATbits.RBF);
                                gpio = I2C2RCV;
                                while(I2C2STATbits.RBF);
                                ind++;
                            }else{
                                while(!I2C2STATbits.RBF);
                                operation =I2C2RCV;
                                while(I2C2STATbits.RBF);
                                gpioHandler(gpio, operation);//<--------handler
                                ind=0;
                                i2c2_slave_state = S_SLAVE_IDLE;    
                            }
                        }                   
                    }else{                   //OVERFLOW DETECTED
                        dummy = I2C2RCV;
                        I2C2STATbits.I2COV = 0; //CLEAR OVERFLOW
                        
                    }
                }
            }
            break;            
        case S_SLAVE_TRANSMIT_MODE:                         //TRANSMIT MODE
            if(I2C2STATbits.S == 1) I2C2CONLbits.ACKDT = 0; //ERRATA *******************
            if(!I2C2STATbits.ACKSTAT){       //ACK received
                while(I2C2STATbits.TBF);
                I2C2TRN = response[1];
                while(!I2C2STATbits.TBF);
                I2C2CONLbits.SCLREL = 1;
                while(I2C2STATbits.RBF); //*******************
                while(I2C2STATbits.ACKTIM);
            }
            else{                            //NACK
                i2c2_slave_state = S_SLAVE_IDLE;
                I2C2CONLbits.SCLREL = 1;    
            }
            break;   
    }
    IFS3bits.SI2C2IF = 0;	//CLEAR INT FLAG
}
int asd =0;

int requestHandler( REG_SELECT rs){
    bool SendResponse;
    SendResponse = 1;
        switch(rs){
            case 1:
                //Register ID
                regsel = REG_ID;
                response[0]=0x16;
                response[1]=0x00;
                break;
            case 2:
                //Register Configuration
                regsel = REG_CONF;
                response[0]=uC2Opi;
                response[1]=0x00;
                break;
            case 3:
                //Register GPIO
                regsel = REG_GPIO;
                response[0] = state_gpio;
                response[1]=0x00;
                
                break;
            case 4:
                //Register Battery
                regsel = REG_BATT;
                response[0]= bateriaI2C;
                response[1]= 0x00;
                break;
            case 5:
                //Register Solar
                regsel = REG_SOLAR;
                response[0]= solarI2C;
                response[1]= (solarI2C & 0xFF00) >> 8;
                break;
    }
    return SendResponse;
}

int gpioHandler(int gpio, int operation){
    switch(gpio){
            case 1:
                switch(operation){
                    case 0: led_button_period = _1_HZ; break; //Operacion led del boton : Parpadear a 1 [Hz]
                    case 1: led_button_period = _2_HZ; break; //Operacion led del boton : Parpadear a 2 [Hz]
                    case 2: led_button_period = _05_HZ; break; //Operacion led del boton : Parpadear a 0.5 [Hz]
                    case 3: led_button_period = _LED_ON; break; //Operacion led del boton : Encendido   
                }
                break;
            case 2:
                switch(operation){
                    case 0: led_status_period = _4_HZ; break; //Operacion led de estado: Parpadear a 4 [Hz]
                    case 1: led_status_period = _LED_ON; break; //Operacion led de estado: Encendido
                    case 2: led_status_period = _LED_OFF; break; //Operacion led de estado: Apagado
                    case 3: led_status_period = _1_HZ; break; //Operacion led de estado: Parpadear a 1 [Hz]
                }
                break;
            case 3:
                switch(operation){
                    case 0: led_warning_period = _2_HZ; break; //Operacion baliza: Parpadear a 2 [Hz]
                    case 1: led_warning_period = _LED_OFF; break; //Operacion baliza: Apagada
                }
                break;
            case 4:
                make_call = operation;
                break;
            case 5:
                switch(operation){
                    case 0: 
                        gmsEnable = false; 
                        led_button_period = _LED_ON; 
                        break; //Modo GSM desactivado - Operacion led boton: Encendido
                    case 1: 
                        led_button_period = _05_HZ;  
                        gmsEnable = true; 
                        break; //Modo GSM activado - Operacion led boton: Parpadear a 0.5 [Hz]
                }
                break;
        }
    return 0;
}
