#include "globales.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/pin_manager.h"
#include "sim900.h"
#include "mcc_generated_files/i2c2.h"
#include <libpic30.h>
#include <stdio.h>
#include <string.h>


void sim900Handler(bool data_available){
    static SIM900_STATE state;
    static int sim900_keep_alive = 0;
    //static bool f_llamando = false;
    
    static int smsRead = 0;
    //extern bool initialized;
    static bool enviar_comando = false;
    
    static PIC_STATES estado_operacion;
    static MODEM_STATES estado_modem;
    static int oks = 0;
    if(data_available){
        //printf("--%d--\r", estado_operacion);
        //estado_modem = sim900getState();
    }
    if(estado_puerta){
        estado_puerta = 0; 
        gate_state_last = gate_state;
        sim900Ronda(); 
    }
    estado_operacion = modemStateHandler(estado_modem, estado_operacion, data_available);
    switch(estado_operacion){
        case INICIALIZAR:
            sim900Init();
            sendCommand("AT+CPBR=1");
            estado_operacion = GET_SIM_NUMBER;
            break;
        case GET_SIM_NUMBER:
            //se queda acá para que nadie lo interrumpa
            break;
        case READ_SMS:
            //se queda acá para que nadie lo interrumpa
            break;
        case LLAMADA_ENTRANTE:
            
            break;
        case SEND_SMS:
            printf("enviarSMS\r");
            sim900EnviarSMS();
            f_enviar_sms = false;
            estado_operacion = IDLE;
            break;
        case LLAMAR_CENTRAL:
            if((maxTiempo >= DURACION_LLAMADA || button_pushed) && f_llamando){ //CAMBIAR POR f_llamada_en_curso
                sim900Cortar();
                printf("corta1-%d\r", button_pushed);
                button_pushed = false;
                f_llamando = false;
                f_llamada_en_curso = false;
                numero++;
                uC2Opi = 0;
                estado_operacion = IDLE;
            }else if(make_call){//make_call button_state){//
                printf("call\r");
                make_call = false;
                button_pushed = false;
                f_llamando = true;
                uC2Opi = 0;
                maxTiempo = 0;
                sim900AudioGSM(ON);
                //sim900MarcarCentral(); 
                secTickerLast = secTicker;
            }else{
                uC2Opi = 0;
                if((secTicker - secTickerLast >= TIEMPO_ESPERA_LLAMANDO) && f_llamando){ //20s tiempo ringing -> cortar enviar pulse el boton nuevamente
                    sim900Cortar();
                    printf("corta2\r");
                    uC2Opi = 2; //vuelva a presionar el boton
                    numero++;
                    button_pushed = false;
                    f_llamando = false;
                    f_llamada_en_curso = false;
                    estado_operacion = IDLE;
                    secTickerLast = secTicker;
                }
                if((secTicker - secTickerLast >= DURACION_LLAMADA) && f_llamada_en_curso){
                    sim900Cortar();
                    printf("corta3\r");
                    button_pushed = false;
                    f_llamando = false;
                    f_llamada_en_curso = false;
                    estado_operacion = IDLE;
                    secTickerLast = secTicker;
                }
                //estado_operacion = modemStateHandler(estado_modem, estado_operacion, data_available);
            }
            break;
        case IDLE:
            //chequear cosas
            if(button_pushed && (!f_llamada_en_curso && !f_llamando)){
                estado_operacion = LLAMAR_CENTRAL;
                estado_modem = NADA;
                button_pushed = 0;
                maxTiempo = 0;
                sim900AudioOPi(ON);
                uC2Opi = 1;
                secTickerLast = secTicker;
                sim900MarcarCentral(); //envia comando llamar mientras reprodce audio
                //printf("pushed-%d-%d\r",estado_operacion, estado_modem);
            }
            if(secTicker % PERIODO_CHECKEAR_SENAL == 0){
                //enviar comando de sela: CSQ
                printf("pushed-%d-%d-%d-%d\r",estado_operacion, button_pushed,f_llamada_en_curso, f_llamando);
                delaysegundos(1);
                sendCommand("AT+CSQ");
            }
            sim900Bateria(); //revisar si los voltajes estan fuera de rango
            if(f_enviar_sms){
                sim900EnviarSMS();
                f_enviar_sms = false;
            }
            //estado_operacion = modemStateHandler(estado_modem, estado_operacion, data_available);
            break;
    }

    
    /*
    if(!initialized){
        sim900Init();
        //sprintf(string, "AT+CPBR=1");
        //enviar_comando = true;
        sendCommand("AT+CPBR=1");
        initialized = true;
    }
    sim900Bateria(); // revisa si bateria es menor a 10V y envia sms
    if(enviar_comando){
        sendCommand((const char*) (string));
        enviar_comando = false;
    }
    
    if(data_available){
        state = sim900getState();
    }
    switch(state){
        case LLAMANDO:
            f_llamando = true;
            maxTiempo = 0;
            break;
        case ENLLAMADA:
            f_llamando = false;
            numero = 0;
            if(maxTiempo >= DURACION_LLAMADA){
                sim900Cortar();
                maxTiempo = 0;
            }
            break;
        case NO_DIALTONE:
            f_llamada_en_curso = false;
            uC2Opi = 3;
            MUX_CTRL_SetLow();
            //sim900SinSenal();
            state = WAIT;
            break;
        case SINSENAL:
            sim900SinSenal();
            state = WAIT;
            break;
        case NO_CARRIER:
            if(f_llamando)
                sim900Reintentar();
            else{
                uC2Opi = 0;
                f_llamada_en_curso = false;
                LED3_SetLow();
                sim900AudioOFF();
                
            }
            cuenta_rings = 0;   
            sim900AudioOPi(ON);
            //SD_PAMP_SetLow(); //deshabilita amplificador
            state = WAIT;
            break;
        case NO_ANSWER:
            if(f_llamando){
                sim900Reintentar();
                state = WAIT;
            }else{
                f_llamada_en_curso = false;
                MUX_CTRL_SetLow();
                uC2Opi = 2;
                //delayms(4000);
                state = WAIT;
            }
            break;
        case ABORTAR:
            break;
        case BUSY:
            sim900Reintentar();
            state = WAIT;
            break;
        case ERROR:
            //sendCommand("AT");
            MUX_CTRL_SetLow(); //audio OPi
            uC2Opi = 2;
            //while(1) LED3_SetHigh();
            //delayms(4000);
            UART1_ClearReceiveBuffer();
            f_llamada_en_curso = false;
            state = WAIT;
            break;
        case RINGING:
            cuenta_rings++;
            if(cuenta_rings >= CANTIDAD_RINGS){
                sim900RecibirLlamada();
                maxTiempo = 0;
                f_llamada_en_curso = true;
                state = ENLLAMADA;
            }
            state = WAIT;
            break;
        case GETCALLINGNUM:
            sim900GetCallingNumber();
            if(isRondaCaller()){
                state = RONDA;
                cuenta_rings = 0;
            /*}else if(isInvalidCaller()){
                sim900Cortar();
                cuenta_rings = 0;
                state = WAIT; *-/
            }else 
                state = WAIT;
            break;
        case RONDA:
            sim900Cortar();
            sim900Ronda();
            state = WAIT;
            break;
        case GETMEMNUM:
            sim900GetNumsFromSim();
            state = WAIT;
            break;
        case SMS_ENTRANTE:
            //if(sim900ReadSMS())
                //state = WAIT;
            
            if(smsRead){
                if(sim900ReadSMS())
                    state = WAIT;
            }else{
                sim900GetSMSnumber();
                smsRead = 1;
                //state = WAIT;
            }
            break;
        case OK:
            //sendCommand("AT+CPAS");
            if(cuenta_rings >= CANTIDAD_RINGS){ 
                state = ENLLAMADA;
                cuenta_rings = 0; 
            }else 
                state = WAIT;
            break;
        case WAIT:
            smsRead = 0;
            LED3_SetLow();
            if(f_llamada_en_curso){
                //hacer nada
            }else
                f_llamando = false;
            break;
        default:
            break;
    }
    //delayms(10);
    if(button_pushed && !f_llamada_en_curso) {
       button_pushed = 0;
       uC2Opi = 1;
    }//else uC2Opi = 0;
    
    if(make_call ){//make_call button_state){//
        make_call = false;
        button_state = false;
        uC2Opi = 0;
        LED3_SetHigh();
        if(f_llamada_en_curso == false){
            f_llamada_en_curso = true;
            f_llamando = true;
            //printf("--%d--%d--\r", numero, numero%cantidad_centrales);
            maxTiempo = 0;
            sim900MarcarCentral(); 
            numero++;
        }//else sim900Cortar();
    }
    
    
    
    if((maxTiempo >= DURACION_LLAMADA || button_state)&& f_llamada_en_curso){
        sim900Cortar();
        button_state = false;
        f_llamada_en_curso = false;
        maxTiempo = 0;
        state = WAIT;
    }
    
    if(estado_puerta){
        estado_puerta = 0; 
        gate_state_last = gate_state;
        //sim900Ronda(); 
        state = RONDA;
    }
    
    if(f_enviar_sms){
        f_enviar_sms = false;
        
        sim900EnviarSMS();
    }
     * */
}

PIC_STATES modemStateHandler(MODEM_STATES estado_modem, PIC_STATES estado_pic, int data_recibida){
    static int oks = 0;
    static int cuenta_rings = 0;
    
    if(data_recibida){
        estado_modem = sim900getState();
        switch(estado_modem){
            case NADA: 
                //printf("nada\r");
                break;
            case OK: //se recibe un 0
                if(estado_pic == GET_SIM_NUMBER){
                    oks++;
                    if(oks > 2*cantidad_centrales){
                        estado_pic = IDLE;
                        oks = 0;
                        printf("oks\r");
                    }
                }
                break;
            case LEER_SIM: //CPRB
                //printf("LEERSIM\r");
                sim900GetNumsFromSim();
                break;
            case RING: //se recibe un 2
                cuenta_rings++;
                if(cuenta_rings >= CANTIDAD_RINGS){
                    sim900RecibirLlamada();
                    maxTiempo = 0;
                    cuenta_rings = 0;
                    secTickerLast = secTicker;
                    f_llamada_en_curso = true;
                    estado_pic = LLAMAR_CENTRAL;
                }
                break;
            case NO_CARRIER: //se recibe un 3
                if(f_llamando == false && f_llamada_en_curso == false){
                    uC2Opi = 3;
                    sim900AudioGSM(OFF);
                    sim900AudioOPi(ON);
                }else{
                    uC2Opi = 0;
                    button_pushed = false;
                    f_llamando = false;
                    f_llamada_en_curso = false;
                    LED3_SetLow();
                    sim900AudioOFF();
                    sim900AudioGSM(OFF);
                    
                }
                estado_pic = IDLE;
                break;
            case ERROR: //se recibe un 4
                //printf("error\r");
                if(estado_pic == GET_SIM_NUMBER){
                    button_pushed = false;
                    f_llamando = false;
                    f_llamada_en_curso = false;
                    estado_pic = IDLE;
                }
                break;
            case NO_DIALTONE: //se recibe un 6
                uC2Opi = 3;
                button_pushed = false;
                f_llamando = false;
                f_llamada_en_curso = false;
                estado_pic = IDLE;
                break;
            case BUSY: //se recibe un 7
            case NO_ANSWER: //se recibe un 8
                sim900AudioGSM(OFF);
                sim900AudioOPi(ON);
                uC2Opi = 2; //vualva a presionar el boton?
                numero++;
                button_pushed = false;
                f_llamando = false;
                f_llamada_en_curso = false;
                estado_pic = IDLE;
                break;
            case LLAMANDO: // MO RING
                f_llamando = true;
                secTickerLast = secTicker;
                break;
            case LLAMADA_CONECTADA: // MO CONNected
                f_llamando = false;
                f_llamada_en_curso = true;
                secTickerLast = secTicker; 
                break; 
            case CSQ: //lee señal rssi
                sim900SinSenal();
                estado_modem = NADA;
                break;
            case SMS_ENTRANTE: 
                //printf("--%s--\r", moduleResonseBuffer);
                sim900GetSMSnumber();
                //estado_pic = READ_SMS;
                break;
            case SMS_GET_SET_NUM: 
                //printf("--%s--\r", moduleResonseBuffer);
                sim900ReadSMS();
                //estado_pic = IDLE;
                break;
            case GET_CALLING_NUM: 
                sim900GetCallingNumber();
                if(isRondaCaller()){
                    sim900Cortar();
                    sim900Ronda();
                    cuenta_rings = 0;
                /*}else if(isInvalidCaller()){
                    sim900Cortar();
                    cuenta_rings = 0; */
                }
                break;
            case CALL_MISSING: 
                cuenta_rings = 0;
                break;
            
            default:
                printf("default\r");
                break;
        }
    }
    return estado_pic;
}

SIM900_STATE sim900getState(){
    SIM900_STATE sim900_status;
    char* aux;
    aux = strchr((const char*)moduleResonseBuffer, '+');
    if(strncmp((const char*)moduleResonseBuffer, SIM900_NOSIGNAL_STATUS, strlen(SIM900_NOSIGNAL_STATUS)) == 0) 
        sim900_status = NO_DIALTONE;
    else if(strncmp((const char*)moduleResonseBuffer, SIM900_NOCARRIER_STATUS, strlen(SIM900_NOCARRIER_STATUS)) == 0) 
        sim900_status = NO_CARRIER;
    else if(strncmp((const char*)moduleResonseBuffer, SIM900_SIGNAL_STATUS, strlen(SIM900_SIGNAL_STATUS)) == 0) 
        sim900_status = CSQ;//SINSENAL;
    else if(strncmp((const char*)moduleResonseBuffer, SIM900_RANGING_STATUS, strlen(SIM900_RANGING_STATUS)) == 0) 
        sim900_status = RINGING;
    else if(strncmp((const char*)moduleResonseBuffer, SIM900_RINGING_STATUS, strlen(SIM900_RINGING_STATUS)) == 0) 
        sim900_status = RING;
    else if(strncmp((const char*)moduleResonseBuffer, SIM900_OK_STATUS, strlen(SIM900_OK_STATUS)) == 0) 
        sim900_status = OK;
    //else if(strncmp(moduleResonseBuffer, SIM900_ABORTAR_STATUS, strlen(SIM900_ABORTAR_STATUS)) == 0) sim900Cortar();
    else if(strncmp((const char*)moduleResonseBuffer, SIM900_BUSY_STATUS, strlen(SIM900_BUSY_STATUS)) == 0) 
        sim900_status = BUSY;
    else if(strncmp((const char*)moduleResonseBuffer, SIM900_LLAMADA_ACTIVA_STATUS, strlen(SIM900_LLAMADA_ACTIVA_STATUS)) == 0) 
        sim900_status = LLAMADA_CONECTADA;
    else if(strncmp((const char*)moduleResonseBuffer, SIM900_LLAMANDO_STATUS, strlen(SIM900_LLAMANDO_STATUS)) == 0) 
        sim900_status = LLAMANDO;
    //else if(strncmp(moduleResonseBuffer, SIM900_ALERTA_STATUS, strlen(SIM900_ALERTA_STATUS)) == 0) sim900Reiniciar();
    else if(strncmp((const char*)moduleResonseBuffer, SIM900_NOANSWER_STATUS, strlen(SIM900_NOANSWER_STATUS)) == 0) 
        sim900_status = NO_ANSWER;
    else if(strncmp((const char*)moduleResonseBuffer, SIM900_ERROR_STATUS, strlen(SIM900_ERROR_STATUS)) == 0) 
        sim900_status = ERROR;
    else if(strncmp((const char*)moduleResonseBuffer, SIM900_SPEAKER0_STATUS, strlen(SIM900_SPEAKER0_STATUS)) == 0) 
        sim900_status = WAIT;
    else if(strncmp((const char*)aux, SIM900_SMS_ENTRANTE_STATUS, strlen(SIM900_SMS_ENTRANTE_STATUS)) == 0) 
        sim900_status = SMS_ENTRANTE;
    else if(strncmp((const char*)moduleResonseBuffer, SIM900_SET_COMMAND, strlen(SIM900_SET_COMMAND)) == 0) 
        sim900_status = SMS_GET_SET_NUM;
    else if(strncmp((const char*)moduleResonseBuffer, SIM900_GET_COMMAND, strlen(SIM900_GET_COMMAND)) == 0) 
        sim900_status = SMS_GET_SET_NUM;
    else if(strncmp((const char*)moduleResonseBuffer, SIM900_LLAMADA_COLGADA_STATUS, strlen(SIM900_LLAMADA_COLGADA_STATUS)) == 0) 
        sim900_status = CALL_MISSING;
    else if(strncmp((const char*)aux, SIM900_SIM_MEM_STATUS, strlen(SIM900_SIM_MEM_STATUS)) == 0) 
        sim900_status = LEER_SIM;// GETMEMNUM;
    else if(strncmp((const char*)aux, SIM900_ERROR_LLAMADA_STATUS, strlen(SIM900_ERROR_LLAMADA_STATUS)) == 0) 
        sim900_status = ERROR;
    else if(strncmp((const char*)aux, SIM900_CLIP_STATUS, strlen(SIM900_CLIP_STATUS)) == 0) 
        sim900_status = GET_CALLING_NUM;
    //else if(strncmp(moduleResonseBuffer, SIM900_BATERIA_STATUS, strlen(SIM900_BATERIA_STATUS)) == 0) sim900Bateria();
    else;//Do nothing
    return sim900_status;
}
void delaysegundos(int segundos)
{
    maxTiempo = 0;
    while(1){
        ClrWdt();
        if(maxTiempo >= segundos)
            break;
    }
}

bool sim900Init(){
    EN_MEDIA_SetHigh();
    LED3_SetHigh();
    delaysegundos(1);
    GSM_RST_SetHigh();
    delaysegundos(4);
    GSM_RST_SetLow();
    delaysegundos(1);
    //LED3_Toggle();
    UART1_ClearReceiveBuffer();
    delaysegundos(10);
    maxTiempo = 0;
    UART1_ClearReceiveBuffer();
    sendCommand("ATE0v0q0");
    sim900DummyRead();
    sendCommand("ATX4"); // No dial tone, busy enable
    sim900DummyRead();
    //sendCommand("AT+CVHU=0"); // (solo SIM7600) habilita ATH para colgar. SI es 1 ATH se ignora y retorna OK 
    //sim900DummyRead();
    sendCommand("AT+CMGF=1"); //text mode 
    sim900DummyRead();
    sendCommand("AT+CLIP=1"); //habilita clip en las llamadas
    sim900DummyRead();
    //sendCommand("AT+CSCA=\"56964100020\",145"); //central de SMS entel (varia segun proveedor) ENTEL: 5698890005 WOM: 56964100020 
    //sim900DummyRead();
    sendCommand("AT+CMEE=0"); //Detalle de errores (0 operacion, 1 operacion y debug, 2 debug)
    sim900DummyRead();
/*    sendCommand("AT+CIURC=1"); //Con mensajes al energizarse. Envia un "Call Ready" al inciar
    sim900DummyRead();
    sendCommand("AT+SIDET=0,0"); //Sin echo
    sim900DummyRead(); */
    sendCommand("AT+MORING=1"); //habilita estados al llamar ("llamando" y "en llamada")
    sim900DummyRead();
    sendCommand("AT+CNMI=2,2,0,0,0"); //setiar envio de sms por serial
    sim900DummyRead();
    sendCommand("AT+CMGD=1,4"); // borra todos los mensajes
    sim900DummyRead();
    sendCommand("AT&W"); // graba la configuración
    sim900DummyRead();
    UART1_ClearReceiveBuffer();
    delaysegundos(2);
    //LED3_SetLow();
    return 1;
    
}

void sim900DummyRead(){
    uint16_t escape = 0;
    moduleResonseBuffer[0] = NULL;
    while(escape < 60000){
        ClrWdt();
        if(getModemResponse()){
            if(strncmp((const char*)moduleResonseBuffer, SIM900_OK_STATUS, strlen(SIM900_OK_STATUS)) == 0)
                break;
        }else
            escape++;
        //delayms(1);
    }
    delayms(50);
}

void sim900GetNumsFromSim(){
    char *aux, *aux2;
    char numero_aux[LARGO_DEL_NUMERO];
    static int j=1, k = 0;
    LED3_SetHigh();
    delayms(200);
    aux = strchr((const char*)moduleResonseBuffer+1, '\"');
    //if(aux != NULL){
        aux2 = strchr((const char*)aux+1, '\"');
        if(aux[1] == '+')
            largoNum = aux2 - aux -2;
        else
            largoNum = aux2 - aux -1;
        //printf("**%d**\r", largoNum);
        strxfrm(&numero_aux, (const char*)aux+1, largoNum+1);
        //printf("**%s**\r", numero_aux);
        if(j%2 == 0){ //si j es par
            //while(numero_Ronda[k][12] != ';'){
                //sprintf(numero_Ronda[k], "+%s;", numero_aux);
            //}
            //numero_Ronda[k][0] = '+';
            strxfrm(&numero_Ronda[k][0], numero_aux, 16);
            //numero_Ronda[k][12] = ';';
            //while(numero_RondaSMS[k][12] != '\"'){   
                //sprintf(numero_RondaSMS[k], "\"%s\"", numero_aux);
            //}
            //numero_RondaSMS[k][0] = '"';
            strxfrm(&numero_RondaSMS[k][0], numero_aux, 16);
            //numero_RondaSMS[k][12] = '"';
            k++;
        }else{
            //numero_ECB[k][0] = '+';
            strxfrm(&numero_ECB[k][0], numero_aux, 16);
            //printf("**%s**\r", numero_ECB[k]);
            //numero_ECB[k][12] = ';';
            //while(numero_ECB[k][12] != ';'){
                //sprintf(numero_ECB[k], "+%s;", numero_aux);
            //}
        }
        moduleResonseBuffer[0] = NULL;
        __delay_ms(100);

        if(numero_Ronda[0] != NULL || numero_RondaSMS[0] != NULL || numero_ECB[0] != NULL)
            ;

        printf("AT+CPBR=%d\r",j+1);
        j++;
        cantidad_centrales = j/2;
        //printf("--%d--\r", cantidad_centrales);
        /*
        if(j >= cantidad_centrales*2){
            cantidad_centrales = j;
            k = 0;
            j = 1;
        }else 
            j++;
         */
    //}
}

void sim900SetNumsToSim(){
    //PENDIENTE DE IMPLEMENTAR. Esta implementado en ReadSMS()
    int i = 1;
    //AT+CPBW=1,"984299584",129,"ECB1"
    for(i = 1; i <= cantidad_centrales; i++){
        delayms(200);
        //printf("AT+CPBW=%d,\"%s\",129,\"ECB%d\"", i, numero_ECB[i], i);
    }
}

bool isInvalidCaller(){
    int i = 0;
    for(i=0; i< cantidad_centrales; i++)
        if(strncmp((const char*)numero_llamando, (const char*)numero_ECB[i], largoNum) == 0)
            return 0;
    return 1;
}

bool isRondaCaller(){
    int i = 0;
    for(i=0; i<= cantidad_centrales; i++)
        if(memcmp((const void*)numero_llamando, (const void*)numero_Ronda[i], 13) == 0){
            numero = i;
            return 1;
        }
    return 0;
}

void sim900GetCallingNumber(){
    char *aux, *aux2;
    char numero_aux[16];
    aux = strchr((const char*)moduleResonseBuffer, '\"');
    aux2 = strchr((const char*)aux+1, '\"');
    //memcpy(&numero_aux, (const char*)aux+1, LARGO_DEL_NUMERO);
    if(aux[1] == '+'){
        strxfrm(&numero_aux, (const char*)aux+2+2, aux2-aux-2-2+1);
        //printf("-+%s+-\r",numero_aux);
    }else{
        strxfrm(&numero_aux, (const char*)aux+1, aux2-aux-1+1);
        //printf("--%s--\r",numero_aux);
    }
    sprintf(numero_llamando, "%s", numero_aux);
    //printf("--%s--\r",numero_llamando);
}

void sim900GetSMSnumber(){
    char *aux, *aux2;
    char numero_aux[LARGO_DEL_NUMERO];
    aux  = strchr((const char*)moduleResonseBuffer, '\"');
    aux2 = strchr((const char*)aux+1, '\"');
    if(aux[1] == '+'){
        strxfrm(&numero_aux, (const char*)aux+2, aux2-aux-2+1);
        //printf("-+%s+-\r",numero_aux);
    }else{
        strxfrm(&numero_aux, (const char*)aux+1, aux2-aux-1+1);
        //printf("--%s--\r",numero_aux);
    }
    sprintf(numero_sms_recivido, "%s", numero_aux);
    //printf("GETSMS--%s--\r",numero_sms_recivido);
}

bool sim900ReadSMS(){
    int i = 0, j = 0;
    char *aux, *aux2;
    char numero_aux[LARGO_DEL_NUMERO];
    //char aux2[80];
    bool sms_leido = false;
    if(strncmp(strchr((const char*)moduleResonseBuffer, '.'), ".", strlen(".")) == 0){
        if(strncmp((const char*)moduleResonseBuffer, SIM900_SET_COMMAND, strlen(SIM900_SET_COMMAND)) == 0){ 
            aux = strchr((const char*)moduleResonseBuffer+5, ' ');
            aux2 = strchr((const char*)aux+1, ' ');
            if(aux[1] == '+'){
                largoNum = aux2 - aux - 2;
                //printf("-+%d+-\r", largoNum);
            }else{
                largoNum = aux2 - aux - 1;
                //printf("--%d--\r", largoNum);
            }
            //printf("--%s--\r", numero_aux);
            //limpiar los 10 primeros espacios de memoria en la SIM
            for(j=1; j<=10; j++){ 
                printf("AT+CPBW=%d\r",j);
                sim900DummyRead();
                //delayms(100);
            }//----------------------------------------------------
            
            while(aux != NULL && i <= CANTIDAD_DE_CENTRALES_MAX){
                if(aux[1] == '+')
                    strxfrm(&numero_aux, (const char*)aux+2, largoNum+1);
                else
                    strxfrm(&numero_aux, (const char*)aux+1, largoNum+1);
                //printf("--%s--%s--%d--\r", aux+1, numero_aux, largoNum);
                if(aux != NULL){
                    sprintf(numero_ECB[i], "%s", numero_aux);
                    //printf("\r");
                    //printf("--%s--\r", numero_ECB[i]);
                    if(aux[1] == '+')
                        printf("AT+CPBW=%d,\"%s\",145,\"ECB%d\"\r", 2*i+1, numero_aux, i+1);
                    else
                        printf("AT+CPBW=%d,\"%s\",129,\"ECB%d\"\r", 2*i+1, numero_ECB[i], i+1);
                    //sim900DummyRead();
                    delayms(50);
                }else
                    ;//error en setiar datos
                aux = strchr((const char*)aux+1, ' ');
                if(aux[1] == '+')
                    strxfrm(&numero_aux, (const char*)aux+2, largoNum+1);
                else
                    strxfrm(&numero_aux, (const char*)aux+1, largoNum+1);
                if(aux != NULL){
                    sprintf(numero_Ronda[i], "%s", numero_aux);
                    if(aux[1] == '+')
                        printf("AT+CPBW=%d,\"%s\",145,\"RONDA%d\"\r", 2*i+2, numero_aux, i+1);
                    else 
                        printf("AT+CPBW=%d,\"%s\",129,\"RONDA%d\"\r", 2*i+2, numero_aux, i+1);
                    //sim900DummyRead();
                    delayms(50);
                    sprintf(numero_RondaSMS[i], "%s", numero_aux);
                }
                i++;
                aux2 = strchr((const char*)aux+1, '.');
                if(aux2 < aux || aux2 - aux < largoNum+1) break;
                aux = strchr((const char*)aux+1, ' ');
            }
            cantidad_centrales = i;
            sms_leido = true;
            
            
        }else if(strncmp((const char*)moduleResonseBuffer, SIM900_GET_COMMAND, strlen(SIM900_GET_COMMAND)) == 0){
            switch(cantidad_centrales){
                case 0:
                    sprintf(mensaje_sms, "NO HAY CENTRALES CONFIGURADAS");
                    break;
                case 1:
                    sprintf(mensaje_sms, "C1:%s R1:%s", numero_ECB[0], numero_Ronda[0]);
                    break;
                case 2:
                    sprintf(mensaje_sms, "C1:%s R1:%s C2:%s R2:%s", numero_ECB[0], numero_Ronda[0], numero_ECB[1], numero_Ronda[1]);
                    break;
                case 3:
                    sprintf(mensaje_sms, "C1:%s R1:%s C2:%s R2:%s C3:%s R3:%s", numero_ECB[0], numero_Ronda[0], numero_ECB[1], numero_Ronda[1], numero_ECB[2], numero_Ronda[2]);
                    break;
                case 4:
                    sprintf(mensaje_sms, "C1:%s R1:%s C2:%s R2:%s C3:%s R3:%s C4:%s R4:%s", numero_ECB[0], numero_Ronda[0], numero_ECB[1], numero_Ronda[1], numero_ECB[2], numero_Ronda[2], numero_ECB[3], numero_Ronda[3]);
                    break;
                case 5:
                    sprintf(mensaje_sms, "C1%s R1%s C2%s R2%s C3%s R3%s C4%s R4%s C5%s R5%s", numero_ECB[0], numero_Ronda[0], numero_ECB[1], numero_Ronda[1], numero_ECB[2], numero_Ronda[2], numero_ECB[3], numero_Ronda[3], numero_ECB[4], numero_Ronda[4]);
                    break;
            }
            strxfrm(&numero_sms, (const char*)numero_sms_recivido, strlen(numero_sms_recivido)+1);
            //printf("--%s--\r", numero_sms);
            f_enviar_sms = true;
            sms_leido = true;
        }
    }else sms_leido = false;
    return sms_leido;
}

void sim900MarcarCentral(){
    /*SD_PAMP_SetLow(); //deshabilita amplificador
    delayms(5);
    MUX_CTRL_SetHigh(); //habilita audio gsm
    MUX_MIC_SetHigh(); //habilita microfono gsm
    SD_PAMP_SetHigh(); //habilita amplificador*/
    
    //sim900AudioGSM(ON);
    //printf("AT+CSQ");
    delayms(100);
    //sendCommand("atd+56984299584;");
    //if (numero < cantidad_centrales ) 
        printf("ATD%s;\r",numero_ECB[numero%cantidad_centrales]);
    //else numero = 0;
}

void sim900SMSreceive(){
    if(strncmp((const char*)moduleResonseBuffer, SIM900_NOSIGNAL_STATUS, strlen(SIM900_NOSIGNAL_STATUS)) == 0) ;
}

void sim900EnviarSMS(){
    int i = 0;
    //char sms[20] = "hola, soy un SOS";
    //char sms[100] = "AT+CMGS=";
    delaysegundos(1);
    sprintf(string, "AT+CMGS=\"%s\"", numero_sms);
    //sendCommand("AT+CMGS=");
    //printf(sms);
    sendCommand((const char*) string);
    //printf("AT+CMGS=%s",numero_sms);
    //UART1_WriteBuffer((const uint8_t*)numero_sms, 13);
    //UART1_Write(0x0D);
    __delay_ms(100);
    printf("%s", mensaje_sms);
    //sprintf(string,"%s%c", mensaje_sms, 0x1A);
    //sendCommand((const char*) string);
    //printf("%s%c", mensaje_sms, 0x1A);
    UART1_Write(0x1A);
    UART1_Write(0x0D);
    __delay_ms(200);
    //sendCommand("AT+CMGD=1,4"); // borra todos los mensajes
    moduleResonseBuffer[0] = NULL;
    for(i=0;i<160;i++){
        mensaje_sms[i] = NULL;
        string[i] = NULL;
    }
    delayms(200);
    
    
}

void sim900SinSenal(){
    char aux1[2], *aux2;
    int rssi = 0;
    aux2 = strchr((const char*)moduleResonseBuffer, ' ');
    if(aux2 != NULL){
        memcpy(&aux1[0], aux2+1, 2);
        if(aux1[0] >= 0x30 && aux1[0] <= 0x39){
            if(aux1[1] >= 0x30 && aux1[1] <= 0x39){
                rssi = (((aux1[0]-0x30)*10) + (aux1[1]-0x30));
            }else{
                rssi = (aux1[0]-0x30);
            }
        }
        //printf("++%s++%s--%d--\r", aux2, aux1, rssi);
        if(rssi <= SENAL_UMBRAL || rssi == 99){
            EN_MEDIA_SetLow();
            delayms(300);
            EN_MEDIA_SetHigh();
            //initialized = false;
        }
    }//else printf("aux2 NULL\r");
}

void sim900Reintentar(){
    //sim900Cortar();
    delayms(400);
    
    if(f_llamada_en_curso == true && numero < cantidad_centrales)
        sim900MarcarCentral();  
    else{ 
        f_llamada_en_curso = false;
        uC2Opi = 2;
        numero = 0;
        //delayms(4000);
        sim900AudioGSM(OFF);
        /*SD_PAMP_SetLow(); //deshabilita amplificador
        delayms(5);
        MUX_CTRL_SetLow();
        SD_PAMP_SetHigh(); //habilita amplificador        
        */
    }
    numero++;
    
}

void sim900Cortar(){
    sendCommand("ATH");
    f_llamada_en_curso = false;
    uC2Opi = 0;
    sim900AudioGSM(OFF);
    //MUX_CTRL_SetLow();
}

void sim900Reiniciar(){
    f_llamada_en_curso = false;
    MUX_CTRL_SetLow();
    delayms(1000);
    sim900Init();
}

void sim900EnableSpeaker(){
    
}

void sim900Mensaje(){
    
}

void sim900Bateria(){
    //delayms(100);
    if(VoltajeBateria < UMBRAL_VOLTAJE_BATERIA && !f_bateriaBaja){
        sprintf(mensaje_sms, "Bateria baja: VBAT=%2.1f", VoltajeBateria);
        strxfrm(&numero_sms, numero_RondaSMS[numero%cantidad_centrales], strlen(numero_RondaSMS[numero%cantidad_centrales]));
        f_enviar_sms = true;
        printf("bateria\r");
        f_bateriaBaja = true;
    }else if(VoltajeBateria > UMBRAL_VOLTAJE_BATERIA + 1)
        f_bateriaBaja = false;
    else ;
}

void sim900RecibirLlamada(){
    //MUX_CTRL_SetHigh(); //habilita audio gsm
    //MUX_MIC_SetHigh(); //habilita microfono gsm
    //SD_PAMP_SetHigh(); //habilita amplificador
    sim900AudioGSM(ON);
    sendCommand("ATA");
}

void sim900Ronda(){
    delayms(100);
    sprintf(mensaje_sms, "VBAT=%2.1f;VPV=%2.1f;Puerta=%d", VoltajeBateria, VoltajePanel, gate_state);
    //memcpy(&mensaje_sms, SIM900_ISALIVE, strlen(SIM900_ISALIVE));
    //aux = strchr((const char*)numero_RondaSMS[numero], '+');
    //memcpy(&numero_sms, (const char*)aux+1, LARGO_DEL_NUMERO);
    //sprintf(numero_sms, "%s", aux);
    //printf("--%d--%d--", numero, cantidad_centrales);
    strxfrm(&numero_sms, numero_RondaSMS[numero%cantidad_centrales], largoNum+1);
    f_enviar_sms = true;
}

void sim900LoopLlamandoCentral(){

}
            
void sim900AudioGSM(bool pwr){
    sim900AudioON();
    if(pwr){
        MUX_CTRL_SetHigh(); //hebilita audio modem GSM
        MUX_MIC_SetHigh(); //habilita microfono gsm
    }else{
        MUX_CTRL_SetLow(); //deshabilita audio modem GSM
        MUX_MIC_SetLow(); //deshabilita microfono gsm
    }
}

void sim900AudioOPi(bool pwr){
    sim900AudioON();
    if(pwr){
        MUX_CTRL_SetLow(); //habilita audio modem OPi
        MUX_MIC_SetLow(); //habilita microfono Opi
    }else{
        MUX_CTRL_SetHigh(); //deshabilita audio modem OPi
        MUX_MIC_SetHigh(); //deshabilita microfono Opi
    }
}

void sim900AudioOFF(){
    SD_PAMP_SetLow(); //deshabilita amplificador
}
    
void sim900AudioON(){
    SD_PAMP_SetHigh(); //habilita amplificador
}