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
#ifndef SIM900_H
#define	SIM900_H

#include <xc.h> // include processor files - each processor file is guarded.  

// TODO Insert appropriate #include <>

// TODO Insert C++ class definitions if appropriate

// TODO Insert declarations
#define SENAL_UMBRAL                             1 //minimo 1

#define SIM900_CLIP_STATUS                       "+CLIP:"//\"+56225802936\""
#define SIM900_OK_STATUS                         "0"//OK" 

#define SIM900_RINGING_STATUS                    "2"//"RING"
#define SIM900_NOCARRIER_STATUS                  "3"//"NO CARRIER"
#define SIM900_ERROR_STATUS                      "4"//"ERROR"
#define SIM900_SIGNAL_STATUS                     "+CSQ"
#define SIM900_NOSIGNAL_STATUS                   "6"//"NO DIALTONE"
#define SIM900_BUSY_STATUS                       "7"//"BUSY"
#define SIM900_NOANSWER_STATUS                   "8"//MISSED_CALL"//"8"//"NO ANSWER"
#define SIM900_RANGING_STATUS                    "RANG"
#define SIM900_LLAMANDO_STATUS                   "MO RING"//"VOICE CALL: RING"
#define SIM900_LLAMADA_ACTIVA_STATUS             "MO CONN" //MO CONNECTED"//"VOICE CALL: BEGIN"
#define SIM900_ERROR_LLAMADA_STATUS              "+CME ERROR"
#define SIM900_SMS_ENTRANTE_STATUS               "+CMT: "
#define SIM900_SIM_MEM_STATUS                    "+CPBR:"


#define SIM900_SET_COMMAND                       "SET NUM"
#define SIM900_GET_COMMAND                       "GET NUM"

#define SIM900_ISALIVE                           "SOS VIVO"

#define SIM900_RING_STATUS                       "\n+WIND: 2"
#define SIM900_ABORTAR_STATUS                    "\n+WIND: 6,1"
#define SIM900_STATUS1_STATUS                    "\n+CPAS: 4"
#define SIM900_ALERTA_STATUS                     "\n+WIND: 1"
#define SIM900_SPEAKER0_STATUS                   "\n+SPEAKER: 1"
#define SIM900_SMS_STATUS                        "\n+cmti: \"SM\","
#define SIM900_BATERIA_STATUS                    "\nBateria"

typedef enum{
    WAIT,
    NO_DIALTONE,
    SINSENAL,
    NO_CARRIER,
    BUSY,
    RINGING,
    GETCALLINGNUM,
    RONDA,
    NO_ANSWER,
    ENLLAMADA,
    LLAMANDO,
    ERROR,
    ABORTAR,
    GETMEMNUUUUM,
    SMS_ENTRANTE,
    OK,
}SIM900_STATE;
//SIM900_STATE sim900_status;
static bool f_llamada_en_curso = 0;
static bool f_enviar_sms = 0;
static bool f_bateriaBaja = 0;
static uint16_t numero = 0;
static char numero_sms_recivido[13];
static char numero_llamando[14];
static char mensaje_sms[160];
static char numero_sms[14];
static char string[160];
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
// TODO Insert declarations or function prototypes (right here) to leverage 
// live documentation

void sim900Handler(bool data_available);
SIM900_STATE sim900getState();
bool sim900Init();
void sim900MarcarCentral();
void sim900SinSenal();
void sim900Reintentar();
void sim900Cortar();
void sim900Reiniciar();
void sim900EnableSpeaker();
void sim900Mensaje();
void sim900Bateria();
void sim900RecibirLlamada();
void sim900LoopLlamandoCentral();
void sim900EnviarSMS();
void sim900Ronda();
bool sim900ReadSMS();
void sim900GetSMSnumber();
bool isInvalidCaller();
void sim900GetCallingNumber();
bool isRondaCaller();
void sim900GetNumsFromSim();
void sim900DummyRead();
void sim900AudioON();
void sim900AudioOFF();
void sim900AudioOPi(bool pwr);
void sim900AudioGSM(bool pwr);


#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C 
    // linkage so the functions can be used by the c code. 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* XC_HEADER_TEMPLATE_H */

