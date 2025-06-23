#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNIG_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#include "BluetoothSerial.h"
#include "timer_interno.h"
#include "definicoes_sistema.h"
#include "ibt_serial.h"
#include "sensor_dist.h"

String device_name = "ESP32-BT-Slave";

// Check if Bluetooth is available
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

// Check Serial Port Profile
#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Port Profile for Bluetooth is not available or not enabled. It is only available for the ESP32 chip.
#endif

TimerInterno tmr;
InterfaceBTSerial iBT;
SensorDist sensor;



enum estados {idleBaixa, idleMedia, idleAlta, ativoBaixa, ativoMedia, ativoAlta};
enum eventos {nenhum_evento, ativar, desativar, aumentarSense, diminuirSense, timeout};
enum acoes {nenhuma_acao, A01, A02, A03, A04, A05};
//ideia:
//A01 = LigarSistema
//A02 = DesligarSistema
//A03 = atualizarLED
//A04 = aumentarSense
//A05 = diminuirSense

struct transicao{
  char proximoEstado;
  char acao;
};

struct transicao matrizTransicao[numEstados][numEventos];

int estado = idleMedia;
int evento = nenhum_evento;
int acao = nenhuma_acao;
QueueHandle_t xQueue;

void iniciaMaquinaEstados(){
  for (int i = 0; i < numEstados; i++){
    for (int j = 0; j < numEventos; j++){
      matrizTransicao[i][j].proximoEstado = i;
      matrizTransicao[i][j].acao = nenhuma_acao;
    }
  }
  matrizTransicao[idleMedia][ativar].proximoEstado = ativoMedia;
  matrizTransicao[idleMedia][ativar].acao = A01;

  matrizTransicao[idleMedia][timeout].proximoEstado = idleMedia;
  matrizTransicao[idleAlta][timeout].proximoEstado = idleAlta;
  matrizTransicao[idleBaixa][timeout].proximoEstado = idleBaixa;

  matrizTransicao[ativoMedia][desativar].proximoEstado = idleMedia;
  matrizTransicao[ativoMedia][desativar].acao = A02;

  matrizTransicao[ativoMedia][aumentarSense].proximoEstado = ativoAlta;
  matrizTransicao[ativoMedia][aumentarSense].acao = A04;

  matrizTransicao[ativoMedia][diminuirSense].proximoEstado = ativoBaixa;
  matrizTransicao[ativoMedia][diminuirSense].acao = A05;

  matrizTransicao[ativoMedia][timeout].proximoEstado = ativoMedia;
  matrizTransicao[ativoMedia][timeout].acao = A03;

  matrizTransicao[ativoBaixa][aumentarSense].proximoEstado = ativoMedia;
  matrizTransicao[ativoBaixa][aumentarSense].acao = A04;

  matrizTransicao[ativoBaixa][timeout].proximoEstado = ativoBaixa;
  matrizTransicao[ativoBaixa][timeout].acao = A03;

  matrizTransicao[ativoBaixa][desativar].proximoEstado = idleBaixa;
  matrizTransicao[ativoBaixa][desativar].acao = A02;

  matrizTransicao[idleBaixa][ativar].proximoEstado = ativoBaixa;
  matrizTransicao[idleBaixa][ativar].acao = A01;

  matrizTransicao[ativoAlta][diminuirSense].proximoEstado = ativoMedia;
  matrizTransicao[ativoAlta][diminuirSense].acao = A05;

  matrizTransicao[ativoAlta][timeout].proximoEstado = ativoAlta;
  matrizTransicao[ativoAlta][timeout].acao = A03;

  matrizTransicao[ativoAlta][desativar].proximoEstado = idleAlta;
  matrizTransicao[ativoAlta][desativar].acao = A02;

  matrizTransicao[idleAlta][ativar].proximoEstado = ativoAlta;
  matrizTransicao[idleAlta][ativar].acao = A01;

}

unsigned int duration1 = 0;
unsigned int duration2 = 0;
unsigned int duration3 = 0;
unsigned int duration4 = 0;
unsigned int duration5 = 0;
unsigned int dist = distMedia;
void executarAcao(int acao){
  if (acao == A01){
    SerialBT.println("Sistema Ativado");
    digitalWrite(led_pin, HIGH);
    digitalWrite(led_pin2, HIGH);
    digitalWrite(led_pin3, HIGH);
    digitalWrite(led_pin4, HIGH);
    digitalWrite(led_pin5, HIGH);
  }
  if (acao == A02){
    SerialBT.println("Sistema Desativado");
    digitalWrite(led_pin, LOW);
    digitalWrite(led_pin2, LOW);
    digitalWrite(led_pin3, LOW);
    digitalWrite(led_pin4, LOW);
    digitalWrite(led_pin5, LOW);
  }
  if (acao == A04){
    if (estado == ativoBaixa){
      dist = distMedia;
      SerialBT.println("Aumentando para Media");
    }
    else if (estado == ativoMedia){
      dist = distAlta;
      SerialBT.println("Aumentando para Alta");
    }
  }
 
  if (acao == A05){
    if (estado == ativoAlta){
      dist = distMedia;
      SerialBT.println("Reduzindo para Media");
    }
    else if (estado == ativoMedia){
      dist =distBaixa;
      SerialBT.println("Reduzindo para baixa");
    }
  }
  else if (acao == A03){
    //completar com código de atualizar LED
    duration1 = sensor.leSensor(trig1,echo1);
    //Serial.print("-");
    //Serial.print(duration1);
    delay(5);

    duration2 = sensor.leSensor(trig2,echo2);
    //Serial.print("-");
    //Serial.print(duration2);
    delay(5);
    

    duration3 = sensor.leSensor(trig3,echo3);
    //Serial.print("-");
    //Serial.print(duration3);
    delay(5);

    duration4 = sensor.leSensor(trig4,echo4);
    //Serial.print("-");
    //Serial.print(duration4);
    delay(5);

    duration5 = sensor.leSensor(trig5,echo5);
    //Serial.print("-");
    //Serial.print(duration5);
    delay(5);
    
    
    if (duration1 > dist){
      digitalWrite(led_pin, HIGH);
    } else{
      digitalWrite(led_pin,LOW);
    }
    if (duration2 > dist){
      digitalWrite(led_pin2, HIGH);
    } else{
      digitalWrite(led_pin2,LOW);
    }  
    if (duration3 > dist){
      digitalWrite(led_pin3, HIGH);
    } else{
      digitalWrite(led_pin3,LOW);
    }
    if (duration4 > dist){
      digitalWrite(led_pin4, HIGH);
    } else{
      digitalWrite(led_pin4,LOW);
    }
    if (duration5 > dist){
      digitalWrite(led_pin5, HIGH);
    } else{
      digitalWrite(led_pin5,LOW);
    }
    //Serial.println("-");
    //Serial.println("Atualizando LED");

  }
}

void vTaskObterEvento(void *pvParameters){
  //volatile uint32_t ul;
  const TickType_t xDelay = pdMS_TO_TICKS(200);
  BaseType_t xStatus;
  for(;;){  
    char teclas;
    teclas = iBT.obterTeclas();
    if (teclas == 'a'){
      evento = ativar;
      xStatus = xQueueSendToBack(xQueue,&evento,0);
      if(xStatus != pdPASS){
        SerialBT.println("Erro ao enviar envento para fila");
      }
      continue;
    }
    else if (teclas == 'd'){
      evento = desativar;
      xStatus = xQueueSendToBack(xQueue,&evento,0);
      if(xStatus != pdPASS){
        SerialBT.println("Erro ao enviar envento para fila");
      }
      continue;
    }
    else if (teclas == 'u'){
      evento = aumentarSense;
      xStatus = xQueueSendToBack(xQueue,&evento,0);
      if(xStatus != pdPASS){
        SerialBT.println("Erro ao enviar envento para fila");
      }
      continue;
  }
  else if (teclas == 'l'){
      evento = diminuirSense;
      xStatus = xQueueSendToBack(xQueue,&evento,0);
      if(xStatus != pdPASS){
        SerialBT.println("Erro ao enviar envento para fila");
      }
      continue;
  }
    else if(tmr.timeout()){
      evento = timeout;
      xStatus = xQueueSendToBack(xQueue,&evento,0);
      if(xStatus != pdPASS){
        SerialBT.println("Erro ao enviar envento para fila");
      }
      continue;
    }
    else{
      evento = nenhum_evento;
    }
    vTaskDelay(xDelay);
  }

}

void vTaskMaquinaEstados(void *pvParameters){
  //volatile uint32_t ul;
  const TickType_t xDelay = pdMS_TO_TICKS(200);
  for(;;){
    if(xQueueReceive(xQueue,&evento,portMAX_DELAY)==pdPASS){
      if (evento != nenhum_evento){
        int estadoAtual = estado;
        acao = matrizTransicao[estadoAtual][evento].acao;
        executarAcao(acao);
        estado = matrizTransicao[estadoAtual][evento].proximoEstado;
        evento = nenhum_evento;
        //Serial.println("----------EVENTO------------");
        //Serial.print("Estado: ");
        //Serial.println(estado);
        //Serial.print("acao: ");
        //Serial.println(acao);
    }
  }
    vTaskDelay(xDelay);
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SerialBT.begin(device_name);
  Serial.println("Maquina ligada");
  Serial.print("Estado: ");
  Serial.println(estado);
  Serial.print("acao: ");
  Serial.println(acao);
  iniciaMaquinaEstados();
  tmr.init();
  pinMode(led_pin, OUTPUT);
  pinMode(led_pin2, OUTPUT);
  pinMode(led_pin3, OUTPUT);
  pinMode(led_pin4, OUTPUT);
  pinMode(led_pin5, OUTPUT);
  pinMode(trig1, OUTPUT);
  pinMode(echo1, INPUT);
  pinMode(trig2, OUTPUT);
  pinMode(echo2, INPUT);
  pinMode(trig3, OUTPUT);
  pinMode(echo3, INPUT);
  pinMode(trig4, OUTPUT);
  pinMode(echo4, INPUT);
  pinMode(trig5, OUTPUT);
  pinMode(echo5, INPUT);
  xQueue = xQueueCreate(5,sizeof(int));
  if(xQueue != NULL){
    xTaskCreatePinnedToCore(vTaskObterEvento, "Obter Evento", 1000, NULL, 1 , NULL, ARDUINO_RUNNING_CORE);
    xTaskCreatePinnedToCore(vTaskMaquinaEstados,"Marquina de Estados", 1000, NULL,1,NULL, ARDUINO_RUNNING_CORE);
  }
}
  

void loop() {


}
