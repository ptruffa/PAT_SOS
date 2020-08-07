/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_GLOBALES_H
#define	XC_GLOBALES_H

#include <xc.h> // include processor files - each processor file is guarded.  

// TODO Insert appropriate #include <>

// TODO Insert C++ class definitions if appropriate
#define DURACION_LLAMADA                        20 //segundos 10min
#define PERIODO_CHECKEAR_SENAL                   60 //En segundos
#define PERIODO_MUESTREO_VOLTAJES                60  //En segundos
#define CANTIDAD_RINGS                          3
#define CANTIDAD_DE_CENTRALES_MAX               5
#define LARGO_DEL_NUMERO                        16
#define UMBRAL_VOLTAJE_BATERIA                  10

#define MODEM_SERIAL_NUMBER_COMMAND           "AT+CGSN"
#define MODEM_CALL_COMMAND                    "ATD"
#define MODEM_HANGOUT_COMMAND                 "ATH"
#define MODEM_ANSWER_COMMAND                  "ATA"
#define MODEM_SIGNAL_QUALITY_COMMAND          "AT+CSQ"
#define MODEM_NETWORL_REGISTRATION_COMMAND    "AT+CREG"

#define MODEM_SIM900                            "SIM900"
#define MODEM_SIERRA                            "Sierra"

#define SIZE_BUFF_FM                            11

#define MODEM_OK_STATUS                         "OK" 
#define MODEM_NOSIGNAL_STATUS                   "\n+CSQ: 99.99"
#define MODEM_NOCARRIER_STATUS                  "NO CARRIER"
#define MODEM_RINGING_STATUS                    "\nRING"
#define MODEM_RING_STATUS                       "\n+WIND: 2"
#define MODEM_ABORTAR_STATUS                    "\n+WIND: 6,1"
#define MODEM_BUSY_STATUS                       "BUSY"
#define MODEM_STATUS1_STATUS                    "\n+CPAS: 4"
#define MODEM_ALERTA_STATUS                     "\n+WIND: 1"
#define MODEM_NOANSWER_STATUS                   "\n+WIND: 1"
#define MODEM_ERROR_STATUS                      "\n+CME ERROR:"
#define MODEM_SPEAKER0_STATUS                   "\n+SPEAKER: 1"
#define MODEM_SMS_STATUS                        "\n+cmti: \"SM\","
#define MODEM_BATERIA_STATUS                    "\nBateria"

#define ON  1
#define OFF 0

// TODO Insert declarations
    extern uint8_t EMULATE_EEPROM_Memory2[64];
    extern int reinicio;
    extern double VoltajeBateria;
    extern double VoltajePanel;
    extern uint8_t bateriaI2C;
    extern uint16_t solarI2C;
    
    extern uint8_t moduleResonseBuffer[160];
    extern uint16_t cantidad_centrales;
    extern uint16_t num_index;
    //extern uint8_t moduleBufferIndex;
    extern uint16_t maxTiempo;
    extern uint16_t secTicker;
    extern uint16_t secTickerLast;
    
    extern uint16_t largoNum;
    
    extern int button_state;
    extern int button_pushed;
    extern int gate_state;
    extern int gate_state_last;
    extern int estado_puerta;
    extern int estado_puerta_last;
    extern int aux_state;
    extern uint32_t gmsEnable;
    extern int make_call;
    extern int state_gpio;
    extern int uC2Opi;

//    extern int f_llamada_en_curso;
    
    //extern int numero;
    /*extern char numero_1[16];
    extern char numero_2[16];
    extern char numero_3[16];
    extern char numero_4[16];
    extern char numero_central[16];
    extern char numero_ronda[16];*/
    extern char numero_ECB[5][16];
    extern char numero_Ronda[5][16];
    extern char numero_RondaSMS[5][16];
    
    typedef enum{
        SIM900,
        WAVECOM,
        NONGSM,
    }modems;
    extern modems modem;
    enum{
        REGISTER_ID = 1,
        REGISTER_CONFIGURATION,
        REGISTER_GPIO,
        REGISTER_BATTERY,
        REGISTER_SOLAR
    };

    typedef enum{
        _05_HZ = 8,
        _1_HZ = 4,
        _2_HZ = 2,
        _4_HZ = 1,
        _LED_ON = 3,
        _LED_OFF = 0,
    } LED_HZ;    

    extern LED_HZ led_button_period;
    extern LED_HZ led_status_period;
    extern LED_HZ led_warning_period;

    typedef enum{
        GSM,
        IP,
    } MAIN_STATE;  
    
    extern MAIN_STATE main_state;
    extern int initialized;
 // Comment a function and leverage automatic documentation with slash star star
/**
    <p><b>Function prototype:</b></p>
  
    <p><b>Summary:</b></p>

    <p><b>Description:</b></p>

    <p><b>Precondition:</b></p>

    <p><b>Parameters:</b></p>

    <p><b>Returns:</b></p>

    <p><b>Example:</b></p>
    <code>
 
    </code>

    <p><b>Remarks:</b></p>
 */
    extern void delayms(unsigned int time_);
    extern void sendCommand(const char *string);
    //extern bool getModemResponse();
    //void __delay_ms(unsigned int time_);
// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

