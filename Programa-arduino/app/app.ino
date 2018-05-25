
#include <Http.h>
#include <string.h>
#include <stdio.h>
#include <Sim800.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>  

unsigned int RX_PIN = 5;
unsigned int TX_PIN = 6;
unsigned int RST_PIN = 7;

HTTP http(9600, RX_PIN, TX_PIN, RST_PIN,true);

#define ONE_WIRE_BUS 2
//Instacia o Objeto oneWire e Seta o pino do Sensor para iniciar as leituras
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensor(&oneWire);

//configuração do sensor de energia
int pinoSensor =A0;
 
int sensorValue_aux = 0;
float valorSensor = 0;
float valorCorrente = 0;
float voltsporUnidade = 0.004887586;// 5%1023
// Para ACS712 de  5 Amperes use 0.185
// Para ACS712 de 10 Amperes use 0.100
//  Para ACS712 de 5 Amperes use 0.066
float sensibilidade = 0.066;
 
//Tensao da rede AC 110 Volts e na verdade (127 volts)
int tensao = 127;

void setup(void)
{
  //Inicia a Serial
  Serial.begin(115200);
  Serial.println("Sensor de temperatura Dallas DS18b20");
  //Inicia o objeto da biblioteca do Dallas
  sensor.begin();

  pinMode(pinoSensor, INPUT); 
}



void loop(void)
{
  //Envia o comando para obter temperaturas
  sensor.requestTemperatures();

 // A temperatura em Celsius para o dispositivo 1 no índice 0 (é possivel ligar varios sensores usando a mesma porta do arduino)
   float leitura=sensor.getTempCByIndex(0);
   float leitura2=sensor.getTempCByIndex(2);

   //Imprime na serial a varivel que recebe os dados do Sensor
   Serial.println("Primeiro sensor");  
   Serial.println(leitura); 

   Serial.println("Segundo sensor");  
   Serial.println(leitura2);  
   
  // salva("https://umonitor-api.herokuapp.com/dados/sensor/1001",leitura);
  // salva("https://umonitor-api.herokuapp.com/dados/sensor/1002",leitura2); 

  String energia = verificaEnergia();

  Serial.println("verificando a energia");  

  Serial.println(energia);  
  
   

   //delay(120000); 
    delay(120); 

}



void salva(char url[200], float temp){  
    //declarando as variaveis    
    Serial.println("----------------------->Montando rest");
    char response[200];
    char s1[26];
    String sensor1="{\"temperaturaAtual\":"; 
    sensor1.concat(temp);
    sensor1.concat(",\"temEnergia\": true}"); 
   
    
    Serial.println("sensor 1");
    Serial.println(sensor1);
       
    sensor1.toCharArray(s1,200);  

    Serial.println("Rest 1");
    Serial.println(s1);
    
    http.configureBearer("movistar.es");   
    http.connect();

    Serial.println("----------------------------->Iniciando o Post");
    http.post(url,s1, response);  
    
    Serial.println("----------------------------->response 1");
    Serial.println(response);
        
    http.disconnect();     
    Serial.println("----------------------->Post Finalizado");

}

String verificaEnergia(){

   for(int i=10000; i>0; i--){
    // le o sensor na pino analogico A0 e ajusta o valor lido ja que a saída do sensor é (1023)vcc/2 para corrente =0
    sensorValue_aux = (analogRead(pinoSensor) -510);
    // somam os quadrados das leituras.
    valorSensor += pow(sensorValue_aux,2);
    delay(1);
  }
 
  // finaliza o calculo da média quadratica e ajusta o valor lido para volts
  valorSensor = (sqrt(valorSensor/ 10000)) * voltsporUnidade;
  // calcula a corrente considerando a sensibilidade do sernsor (185 mV por amper)
  valorCorrente = (valorSensor/sensibilidade);


  //tratamento para possivel ruido
  //O ACS712 para 30 Amperes é projetado para fazer leitura
  // de valores alto acima de 0.25 Amperes até 30.
  // por isso é normal ocorrer ruidos de até 0.20A
  //por isso deve ser tratado
  if(valorCorrente <= 0.095){
    valorCorrente = 0;
  }
 
  valorSensor =0;
 
 
  //Mostra o valor da corrente
  Serial.print("Corrente : ");
  // Irms
  Serial.print(valorCorrente, 3);
 
 
  //Calcula e mostra o valor da potencia
  Serial.print(" Potencia (Consumo) : ");
  Serial.print(valorCorrente * tensao);
  Serial.println(" Watts ");

  return "funfou";

}
