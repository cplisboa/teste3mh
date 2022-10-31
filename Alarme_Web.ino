/*Alarme_Web
 * Versao 1.0 31/10/2022
 */
/*https://lastminuteengineers.com/creating-esp8266-web-server-arduino-ide/
//https://www.instructables.com/A-Versatile-Door-Alarm-System-With-Telegram-Bot-RF/

*/
/*Comentar abaixo para operação AP*/
const char* ssid = "Figueira - Oi Fibra 2G";  // Enter SSID here
const char* password = "genesis1qaz";  //Enter Password here
char hostOTA[] = "Alarme_Web";
char passOTA[] = "";




/* 
Pinos usados no alarme
#define Luzes D6
#define Sirene D7
#define SensorExterno D1
#define SensorInterno D2
#define Controle D5
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>
ESP8266WebServer server(86);  //Cria um objeto ESP8266WebServer com nome server na porta 86



uint8_t Sirene = D7;
bool EstadoSirene = LOW;

uint8_t Luzes = D6;
bool EstadoLuzes = LOW;

void setupOTA()
{
 ArduinoOTA.setHostname(hostOTA);
  //ArduinoOTA.setPassword(passOTA);
  ArduinoOTA.onStart([]() {
    Serial.println("OTA: Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA: End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("OTA: Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("OTA: Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("OTA: Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("OTA: Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("OTA: End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA: Ready");  
}





void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(Sirene, OUTPUT);
  pinMode(Luzes, OUTPUT);

//Para conexão STA
  Serial.println("Connecting to ");
  Serial.println(ssid);
  //connect to your local wi-fi network
  WiFi.begin(ssid, password);
  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP()); // Envia para a porta serial o Ip atribuido ao esp
//Comente até aqui para operação AP

//Especifica as funçoes que devem ser executadas para cada solicitação do cliente
  server.on("/", handle_OnConnect);
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  server.on("/led2on", handle_led2on);
  server.on("/led2off", handle_led2off);
  server.onNotFound(handle_NotFound);

  server.begin();             //Inicializa o servidor
  Serial.println("HTTP server started");
   setupOTA();

}
void loop() {
   ArduinoOTA.handle();
  server.handleClient();      //Faz o tratamento das requisiçoes enviadas pelo cliente
  if(EstadoSirene)                              
  {digitalWrite(Sirene, HIGH);}    //Liga Led1
  else
  {digitalWrite(Sirene, LOW);}      //Desliga Led1
  
  if(EstadoLuzes)
  {digitalWrite(Luzes, HIGH);}
  else
  {digitalWrite(Luzes, LOW);}
 
}

void handle_OnConnect() {
  EstadoSirene = LOW;           
  EstadoLuzes = LOW;
  Serial.println("GPIO7 Status: OFF | GPIO6 Status: OFF");
  server.send(200, "text/html", SendHTML(EstadoSirene,EstadoLuzes)); 
}

void handle_led1on() {
  EstadoSirene = HIGH;
  Serial.println("GPIO7 Status: ON");
  server.send(200, "text/html", SendHTML(true,EstadoLuzes)); 
}

void handle_led1off() {
  EstadoSirene = LOW;
  Serial.println("GPIO7 Status: OFF");
  server.send(200, "text/html", SendHTML(false,EstadoLuzes)); 
}

void handle_led2on() {
  EstadoLuzes = HIGH;
  Serial.println("GPIO6 Status: ON");
  server.send(200, "text/html", SendHTML(EstadoSirene,true)); 
}

void handle_led2off() {
  EstadoLuzes = LOW;
  Serial.println("GPIO6 Status: OFF");
  server.send(200, "text/html", SendHTML(EstadoSirene,false)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

//Função que retorna em uma string a pagina HTML sempre que o servidor recebe uma requisição do cliente
String SendHTML(uint8_t led1stat,uint8_t led2stat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";  //Titulo da página
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
 //Seleciona cor, fonte e margens do corpo
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #1abc9c;}\n";
  ptr +=".button-on:active {background-color: #16a085;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  //Cabeçalho da pagina
  ptr +="<h1>ESP8266 Web Server</h1>\n";
  ptr +="<h3>Using Station(STA) Mode</h3>\n";
  //Usa if para criar os botoes dinamicamente
   if(led1stat)
  {ptr +="<p>SIRENE LIGADA</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";}
  else
  {ptr +="<p>SIRENE DESLIGADA</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";}

  if(led2stat)
  {ptr +="<p>LUZES LIGADAS</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";}
  else
  {ptr +="<p>LUZES DESLIGADAS</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";}

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr; // Retorna a string ptr com a pagina html
}
