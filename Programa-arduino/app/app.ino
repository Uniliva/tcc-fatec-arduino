
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



void loop(void){
  
   //Envia o comando para obter temperaturas
   sensor.requestTemperatures();

   //verifica a energia
   String temEnergia = verificaEnergia();
   Serial.println("Tem energia?");     
   Serial.println(temEnergia);  

   //------------------------------------------------------------------------------------------------------
    
   //Coleta os dados
   float temperaturaSensor1=sensor.getTempCByIndex(0);

   //Imprime na serial a varivel que recebe os dados do Sensor
   Serial.println("Temperatira do primeiro sensor");  
   Serial.println(temperaturaSensor1); 
   //envia os dados   
   enviaDados("https://umonitor-api.herokuapp.com/dados/sensor/1001",temperaturaSensor1, temEnergia);

   //------------------------------------------------------------------------------------------------------
   
   //Coleta os dados
   float temperaturaSensor2=sensor.getTempCByIndex(2);

   //Imprime na serial a varivel que recebe os dados do Sensor
   Serial.println("Temperatira do segundo sensor");  
   Serial.println(temperaturaSensor2); 
   //envia os dados   
   enviaDados("https://umonitor-api.herokuapp.com/dados/sensor/1002",temperaturaSensor2, temEnergia);


    //------------------------------------------------------------------------------------------------------

   //Delay de 5 minutos ate a proxima verificação
   //delay(120000); 
    delay(120); 

}


//metodo enviaDados para API
void enviaDados( char urlChar[200], float temp, String temEnergia){  

     //declarando as variaveis    
    Serial.println("---------------|  Montando rest");
    char response[10];
    char bodyChar[50];   

    //Montando body da requisição
    String body="{\"temperaturaAtual\":"; 
    body.concat(temp);
    body.concat(",\"temEnergia\":"); 
    body.concat(temEnergia); 
    body.concat("}"); 

    //convertendo de string para char
    body.toCharArray(bodyChar,50);

    //conectando ao sim800l
    http.configureBearer("movistar.es");   
    http.connect();

    //realizando o request
    Serial.println("---------------|  Iniciando o Post");
    http.post(urlChar,bodyChar, response);  

    //obtendo resposta
    Serial.println("---------------|  Response obtido");
    Serial.println(response);

    //fechando conexão
    http.disconnect();     
    Serial.println("---------------|  Post Finalizado");   
}

//metodo verifica se tem energia
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

    Serial.println("---------------|  Valor Corrente");   
     Serial.println(valorCorrente);

    //validando se tem energia
    if (valorCorrente < 0.4){
     //não tem energia
    return "false";
    }else {
     //tem energia
    return "true";
    }
}
