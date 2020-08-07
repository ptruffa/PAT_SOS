/*
 * Fecha último cambio: 17-12-2019
 * Change log:
 *  llamadas con lista circular
 *  cuelga al apretar el boton
 *   
 */
/**
  Section: Included Files
*/
#include <xc.h>
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/clock.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/tmr3.h"
#include "mcc_generated_files/adc1.h"
#include "globales.h"
#include <stdio.h>
#include <string.h>
#include "sim900.h"
#include "mcc_generated_files/i2c2.h"
#include "mcc_generated_files/memory/flash_demo.h"
#include <libpic30.h>

//#define FCY  8000000
//void __delay_ms(unsigned long);

void wavecomHandler(bool data_available);

void gsmInit();

void LecturaVoltajes();
void delayms(unsigned int time_);
void sendCommand(const char *string);
bool getModemResponse();
bool initModem(void);
//void sendCommand(const char *string);
int button_pushed = 0;
int button_state = 0;
int gate_state = 0;
int gate_state_last = 0;
int estado_puerta = 0;
int estado_puerta_last = 0;
int aux_state;

int reinicio = 0;

uint16_t maxTiempo = 0;
uint16_t secTicker = 0;

double VoltajeBateria;
double VoltajePanel;
uint8_t bateriaI2C;
uint16_t solarI2C;
    
int state_gpio;
int state_gpio_last;

MAIN_STATE main_state = GSM;
//static bool gsm_initialized = 0;
int initialized = false;
modems modem = NONGSM;

uint8_t moduleResonseBuffer[160];
uint8_t moduleBufferIndex = 0;
bool readMessage = false;
//static bool isEndChar = 0;

uint32_t gmsEnable = false;
int make_call = false;
int uC2Opi = 0;
uint16_t largoNum = 1;
int primera_vez = true;
unsigned int timeout = 0;

/*
//int numero = 1;
char numero_1[16] = "+56984299584;";
char numero_2[16] = "+56984299584;";
char numero_3[16] = "+56984299584;";
char numero_4[16] = "+56978889366;";
char numero_central[16] = "\"56984299584\"";//"\"56973483808\"";
char numero_ronda[16] = "+56225802936;";
char numero_aux[16];
char* aux;
*/
char numero_ECB[5][16];// = {{"+56225802936;"}, {"+56225802911;"}};
char numero_Ronda[5][16];// = {{"+56984299584;"}, {"+56978889366;"}, {"+56225802910;"}};
char numero_RondaSMS[5][16];// = {{"\"56984299584\""},{"\"56978889366\""}, {"\"56225802910\""}};

uint16_t cantidad_centrales = 2;
uint16_t num_index = 1;
int main(void)
{
    // initialize the device
    reinicio = 0;  
    state_gpio =  (!BTN_IN_GetValue()) + (!PUERTA_GetValue() << 1) + (!DIN_AUX_GetValue() << 2) + (reinicio << 3);
    SYSTEM_Initialize();
    uC2Opi = 0;
    //HEARTBEAT_SetHigh();
    printf("INICIANDO...\r");
    U1SCCONbits.SCEN = 0;
    //while(1);
    MUX_CTRL_SetLow(); //audio OPi
    MUX_MIC_SetLow();
    SD_PAMP_SetHigh(); // habilita amplificador
    //EN_MEDIA_SetHigh();
    //gsmInit();
    //SD_PAMP_SetHigh(); //habilita amplificador de audio
    //MUX_MIC_SetHigh();
    //MUX_CTRL_SetHigh();
    DO1_Toggle();
    DO2_Toggle();
    //readFlash(&gmsEnable);
    //while(secTicker < 30);
    //_RESET;
    //I2C2_Initialize();
    //while(1);
    /*LED3_SetHigh();
    int i = 1, j;
    printf("hola!!\n");
    uint16_t adcValue = 0;
    int rangeValue[SIZE_BUFF_FM];
    int sortedrangeValue[SIZE_BUFF_FM];
    int medianaActual = 0;
    int rangeValue2[SIZE_BUFF_FM];
    int sortedrangeValue2[SIZE_BUFF_FM];
    int medianaActual2 = 0;
    while(1){
        adcValue = ADC1_GetVpv();
        rangeValue[i++] = adcValue; //se guarda el valor en un arreglo
        //adcValue = ADC1_GetVpv();
        //rangeValue2[i++] = adcValue; //se guarda el valor en un arreglo
        if(i > SIZE_BUFF_FM){ //vuelve el contador del arreglo al inicicio
            i = 0;
            for(j=1;j<SIZE_BUFF_FM;j++){//se pasa el arreglo a otro para ordenarlo
                sortedrangeValue[j] = rangeValue[j];
                //sortedrangeValue2[j] = rangeValue2[j];
            }
            isort(sortedrangeValue, SIZE_BUFF_FM);//se ordena el arreglo
            //isort(sortedrangeValue2, SIZE_BUFF_FM);//se ordena el arreglo
            medianaActual = mode(sortedrangeValue,SIZE_BUFF_FM);//se entrega la mediana
            //medianaActual2 = mode(sortedrangeValue2,SIZE_BUFF_FM);//se entrega la mediana
        }
        if(maxTiempo == 1){
            printf("Vbat: %d \t %2.1f\n", medianaActual, medianaActual*30.0/505);
            maxTiempo = 0;
        }
        //__delay_ms(100);
    }*/    
    /*int i=1;
    EN_MEDIA_SetHigh();
    LED3_SetHigh();
    __delay_ms(500);
    GSM_RST_SetHigh();
    LED3_Toggle();
    __delay_ms(1500);
    LED3_Toggle();
    GSM_RST_SetLow();
    UART1_ClearReceiveBuffer();
    __delay_ms(1500);
    bool enviar_comando = true;
    sprintf(string, "AT+CPBR=1");
    while(1){
        if(getModemResponse())
            readMessage = true;
        else
            readMessage = false;
        
        if(enviar_comando){
            sendCommand((const char*) string);
            //printf("%s\r",(const char*)(string));
            enviar_comando = false;
        }
        
        
        if(readMessage){
            printf("hola!\r");
            aux = strchr((const char*)moduleResonseBuffer, '+');
            if(strncmp((const char*)aux, SIM900_SIM_MEM_STATUS, strlen(SIM900_SIM_MEM_STATUS)) == 0){ 
                sprintf(string, "AT+CPBR=%d", i);
                i++;
                enviar_comando = true;
                timeout = 0;
                LED1_Toggle();
                delayms(500);
            }
        }else{
            if(timeout++ > 60000){
                enviar_comando = true;
                timeout = 0;
                LED2_Toggle();
            }
        }
        if(i > 4) i = 1;   
    }*/
    /*while(1){ //leer hasta que llegue una respuesta
        if(getModemResponse()){
            if(strncmp((const char*)moduleResonseBuffer, SIM900_SET_COMMAND, strlen(SIM900_SET_COMMAND)) == 0){ 
                cantidad_centrales = 0;
                aux = strchr((const char*)moduleResonseBuffer, '+');
                //if(aux == NULL) cantidad_centrales = 2;
                while(aux != NULL && cantidad_centrales <= 5){
                    memcpy(&numero_aux, (const char*)aux+1, LARGO_DEL_NUMERO);
                    if(aux != NULL)
                        sprintf(numero_ECB[cantidad_centrales], "+%s;", numero_aux);
                    else
                        ;//error en setiar datos
                    aux = strchr((const char*)aux+1, '+');
                    memcpy(&numero_aux, (const char*)aux+1, LARGO_DEL_NUMERO);
                    if(aux != NULL){
                        sprintf(numero_Ronda[cantidad_centrales], "+%s;", numero_aux);
                        sprintf(numero_RondaSMS[cantidad_centrales], "\"%s\"", numero_aux);
                    }
                    aux = strchr((const char*)aux+1, '+');
                    cantidad_centrales++;
                }

            }else if(strncmp((const char*)moduleResonseBuffer, SIM900_GET_COMMAND, strlen(SIM900_GET_COMMAND)) == 0){
                delayms(1000);
                printf("AT+CMGS=%s\r", numero_sms_recivido);
                switch(cantidad_centrales){
                    case 0:
                        sprintf(mensaje_sms, "NO HAY CENTRALES CONFIGURADAS");
                        break;
                    case 1:
                        sprintf(mensaje_sms, "C1%s R1%s", numero_ECB[0], numero_Ronda[0]);
                        break;
                    case 2:
                        sprintf(mensaje_sms, "C1%s R1%s C2%s R2%s", numero_ECB[0], numero_Ronda[0], numero_ECB[1], numero_Ronda[1]);
                        break;
                    case 3:
                        sprintf(mensaje_sms, "C1%s R1%s C2%s R2%s C3%s R3%s", numero_ECB[0], numero_Ronda[0], numero_ECB[1], numero_Ronda[1], numero_ECB[2], numero_Ronda[2]);
                        break;
                    case 4:
                        sprintf(mensaje_sms, "C1%s R1%s C2%s R2%s C3%s R3%s C4%s R4%s", numero_ECB[0], numero_Ronda[0], numero_ECB[1], numero_Ronda[1], numero_ECB[2], numero_Ronda[2], numero_ECB[3], numero_Ronda[3]);
                        break;
                    case 5:
                        sprintf(mensaje_sms, "C1%s R1%s C2%s R2%s C3%s R3%s C4%s R4%s C5%s R5%s", numero_ECB[0], numero_Ronda[0], numero_ECB[1], numero_Ronda[1], numero_ECB[2], numero_Ronda[2], numero_ECB[3], numero_Ronda[3], numero_ECB[4], numero_Ronda[4]);
                        break;
                }
                //sprintf(mensaje_sms, "%s %s %s %s ", numero_ECB[0], numero_Ronda[0], numero_ECB[1], numero_Ronda[1]);
                delayms(200);
                printf("%s", mensaje_sms);
                UART1_Write(0x1A);
                UART1_Write(0x0D);
                sendCommand("AT+CMGD=1,4"); // borra todos los mensajes
                delayms(200);
                
            }else if(strncmp((const char*)moduleResonseBuffer, SIM900_SMS_ENTRANTE_STATUS, strlen(SIM900_SMS_ENTRANTE_STATUS)) == 0){
                aux = strrchr((const char*)moduleResonseBuffer, '+');
                memcpy(&numero_aux, (const char*)aux+1, LARGO_DEL_NUMERO);
                sprintf(numero_sms_recivido, "\"%s\"", numero_aux);
            }
        }else i = 0;   
        //if(timeout > 5000) break;
    }*/  
    /*
    //Elegir el modem a utilizar en base al que esta conectado
    sendCommand("ATI"); //enviar comando de identificacion 
    while(!getModemResponse()){ //leer hasta que llegue una respuesta
        timeout++;
        //if(timeout > 5000) break;
    }
    if(strncmp((const char*)moduleResonseBuffer, MODEM_SIM900, strlen(MODEM_SIM900)) == 0)
        modem = SIM900;
    else if(strncmp((const char*)moduleResonseBuffer, MODEM_SIERRA, strlen(MODEM_SIERRA)) == 0)
        modem = WAVECOM;
    else
        modem = NONGSM;
    sendCommand("AT+CMGD=1,4"); // borra todos los mensajes
    delayms(200);*/
    UART1_ClearReceiveBuffer();
    modem = SIM900;
    LecturaVoltajes();
    //-----------------------------
    while (1)
    {
        if(secTicker == 0xFFFF) secTicker -= 0xFFFE;
        if(secTicker % PERIODO_MUESTREO_VOLTAJES == 0){
            LecturaVoltajes();
            //secTicker = 0;
        }
        if(gmsEnable || f_llamada_en_curso){
        //if(1 || f_llamada_en_curso){
            if(secTicker % PERIODO_CHECKEAR_SENAL == 0){
                //enviar comando de sela: CSQ
                sendCommand("AT+CSQ");
            }
            LED3_SetLow();
            if(getModemResponse())
                readMessage = true;
            else
                readMessage = false;
            
            switch(modem){
                case SIM900:
                    sim900Handler(readMessage);
                    break;
                case WAVECOM:
                    wavecomHandler(readMessage);
                    break;
                case NONGSM:
                    break;
            }
        }else{
            LED3_SetHigh();
            EN_MEDIA_SetLow();
            MUX_CTRL_SetLow();
            MUX_MIC_SetLow();
            SD_PAMP_SetHigh();
            initialized = false;
        }

    }

    return 1;
}

void gsmInit(){
    EN_MEDIA_SetHigh(); //habilita modem y media converter
    delayms(200);
    GSM_RST_SetHigh();
    delayms(3000);
    GSM_RST_SetLow();
    delayms(3000);
    UART1_ClearReceiveBuffer();
    delayms(1000);
    UART1_ClearReceiveBuffer();
}

void wavecomHandler(bool data_available){
    printf("wavecom!\n");
}

void sendCommand(const char *string){
    while(*string){
        UART1_Write(*string++);
    }
    UART1_Write(0x0D);
}

bool getModemResponse()
{   
    static int isEndChar = 0;
    //static uint8_t moduleResonseBuffer[64];
    static uint8_t moduleBufferIndex = 0;
    bool terminatorReceived = false;    // Return false; Unless Terminator character (0x0D) is captured
    // disable the I2C master interrupt
    //IEC3bits.SI2C2IE = 0;
    if ((UART1_TransferStatusGet() & UART1_TRANSFER_STATUS_RX_DATA_PRESENT)) // See if Bytes are available
    {
        uint8_t bufferByte = 0; // Restore for read
        bufferByte = UART1_Read();
        // See what we got
        if ((bufferByte == 0x0D && isEndChar))
        {
            isEndChar = false;
            moduleResonseBuffer[moduleBufferIndex] = 0x00; // Add Null terminator
            terminatorReceived = true;
            moduleBufferIndex = 0; // Prepare index for next message
            //delayms(200);
        }
        /*else if ( bufferByte == 0x2E )
        {
            moduleResonseBuffer[moduleBufferIndex] = bufferByte; // Add Byte to Buffer
            isEndChar = false;
            moduleResonseBuffer[moduleBufferIndex] = 0x00; // Add Null terminator
            terminatorReceived = true;
            moduleBufferIndex = 0; // Prepare index for next message
        }*/
        else if ( (bufferByte == 0x00) || (bufferByte == 0x0A) || (bufferByte == 0x0D)|| (bufferByte == 0xFF) )
        {
            // Do nothing
        }
        else
        {
            moduleResonseBuffer[moduleBufferIndex] = bufferByte; // Add Byte to Buffer
            // Increment Buffer for next byte
            moduleBufferIndex++;
            isEndChar = true;
        }
    }
    // enable the I2C master interrupt
    //IEC3bits.SI2C2IE = 1;
    return terminatorReceived;
}
/**
 End of File
*/

void delayms(unsigned int time_){
    unsigned long long time_delay = (time_*_XTAL_FREQ)/1000ULL;      // Contador de ciclos de FCY a esperar
    __delay32(time_delay);
}

void __delay_ms(unsigned long time_){
    unsigned long long time_delay = (time_*_XTAL_FREQ)/1000ULL;      // Contador de ciclos de FCY a esperar
    __delay32(time_delay);
}

void LecturaVoltajes(){
    uint16_t adcValue = 0;
    int i = 0;
    int j;
    static int rangeValue[SIZE_BUFF_FM];
    static int sortedrangeValue[SIZE_BUFF_FM];
    int medianaActual = 0;
    static int rangeValue2[SIZE_BUFF_FM];
    static int sortedrangeValue2[SIZE_BUFF_FM];
    int medianaActual2 = 0;
    
    for(i = 0; i < SIZE_BUFF_FM; i++){
        adcValue = ADC1_GetVbat();
        //printf("adc: %d\t", adcValue);
        rangeValue[i] = adcValue; //se guarda el valor en un arreglo
        __delay_ms(10);
        adcValue = ADC1_GetVpv();
        //printf("adc: %d\n", adcValue);
        rangeValue2[i] = adcValue; //se guarda el valor en un arreglo
        __delay_ms(10);
    }
    if(i >= SIZE_BUFF_FM){ //vuelve el contador del arreglo al inicicio
        i = 0;
        for(j=1;j<SIZE_BUFF_FM;j++){//se pasa el arreglo a otro para ordenarlo
            sortedrangeValue[j] = rangeValue[j];
            sortedrangeValue2[j] = rangeValue2[j];
        }
        isort(sortedrangeValue, SIZE_BUFF_FM);//se ordena el arreglo
        isort(sortedrangeValue2, SIZE_BUFF_FM);//se ordena el arreglo
        medianaActual = mode(sortedrangeValue,SIZE_BUFF_FM);//se entrega la mediana
        medianaActual2 = mode(sortedrangeValue2,SIZE_BUFF_FM);//se entrega la mediana
        VoltajeBateria = medianaActual*15.0/626;
        VoltajePanel = medianaActual2*30.0/505;
        //printf("Vbat: %2.1f\t Vpv: %2.1f\r\n", VoltajeBateria, VoltajePanel);
        bateriaI2C = VoltajeBateria * 10;
        solarI2C = VoltajePanel * 10;
        //printf("B: %d - S: %d\r\n", bateriaI2C, solarI2C);
    }
}