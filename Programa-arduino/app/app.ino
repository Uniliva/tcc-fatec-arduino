
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

void setup(void)
{
  //Inicia a Serial
  Serial.begin(115200);
  Serial.println("Sensor de temperatura Dallas DS18b20");
  //Inicia o objeto da biblioteca do Dallas
  sensor.begin();
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

   salva("https://umonitor-api.herokuapp.com/dados/novo",leitura);


   //salva("fatecexpo.fatecosasco.com.br:2651/rest/dados/166667",leitura2);
   

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
    sensor1.concat("\"dataAtual\": \"2018-05-03T21:26:22Z\",\"temEnergia\": true,\"sensorId\": 1}"); 

   
    
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
