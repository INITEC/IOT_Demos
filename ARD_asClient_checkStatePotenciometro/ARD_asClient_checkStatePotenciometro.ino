/*
*  ARD_asClient_checkStatePotenciometro
*  
*
*  CONSIDERAICION
*  Para este ejemplo, se requiere un servicio web activo en un servidor conocido
*  con el controlador indicado para su correcto funcionamiento.
*  @version v1.0
*  @autor Juan Basilio
*  @implementacion Juan Basilio
*/
#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h> 
#include <stdio.h>
#include <math.h>

/*
*  Definimos las variables de caso especifico
*  @global define pinControl - Es la posicion del pin digital que se utilizara para el control
*  @global boolean lastConnected - Estado de la conexion la ultima vez que se recorrio el loop
*/
#define pinControl 8
int id0=1; 
boolean lastConnected = false;
boolean flag=false;
String estado="OFF"; //Estado del Led inicialmente "OFF"


/*
*  Configuracion IP
*  @global byte mac - Direccion mac de la tarjeta Ethernet
*  @global IPAddress ip - Definicion de una ip por defecto
*  @global char server - Nombre del dominio de la plataforma con la que trabajara la tarjeta
*  @global EthernetClient client - Objeto de tipo cliente de la tarjeta Ethernet
*/
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,0,177);
char server[] = "potenciometro.waposat.com";
//byte server[] = { 172, 16, 13, 1 }; // IPServidor LAN
EthernetClient client;


void setup(){
  
  pinMode(pinControl, OUTPUT);
  digitalWrite(pinControl,HIGH);  // Nos aseguramos de iniciar el pin en LOW
  
  Serial.begin(9600);  // Iniciamos la comunicacion serial con la computadora
  delay(1000);
  // Probamos iniciar la tarjeta de red con DHCP
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  
}

void loop(){
  /*
  *  Verificacion de si el cliente ha recibido algun valor en cola, en caso sea verdadero
  *  muestra caracter a caracter los valores recibidos.
  */  
  if (client.available()) {
    char c = client.read();
    analizarRespuestaChar(c);
    //Serial.print(c);
  }
  /*
  *  Se detiene al cliente una vez que se ha realizado una conexion y esta ha terminado
  */
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  
  /*
  *  Si el cliente no esta conectado, realiza la secuencia para iniciar una nueva conexion
  *  utilizando la funcion httpRequest
  */
  if(!client.connected()) {  
    httpRequest();
  }
  
  lastConnected = client.connected();  // Se guarda el ultimo valor de la conexion

}

/*
*  function analizarRespuestaChar(char c)
*  Verifica la respuesta del servidor caracter por caracter
*  @param char c - respuesta del servidor, en un caracter a la vez
*  @return void
*  @autor: Juan Basilio
*/

void analizarRespuestaChar(char c){
  if (flag==true && c!='@'){
      if(c=='1'){
        Serial.println("Encendiendo");
        changePin(1);
      } else if (c=='0'){
        Serial.println("Apagando");
        changePin(0);
      } else{
        Serial.println("Estado no aceptado");
      }
      flag=false;
  }
  if (c=='@'){
    flag=true;
  }
}

/*
*  function httpRequest ()
*  Realiza una consulta de tipo HTTP utilizando el metodo GET al servidor remoto
*  @return void
*/
void httpRequest() {
  Serial.println("Iniciando");
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.print("GET /controller/get.statePotenciometro.php?id="); // Envia los datos utilizando GET
    client.print(id0);
    client.println(" HTTP/1.1");
    client.println("Host: potenciometro.waposat.com");
    client.println("Connection: close");
    client.println();    
    delay(500);
  } 
  else {
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    client.stop();
  }
}

void changePin(int state){
  if (state == 0){
    estado="OFF";
    digitalWrite(pinControl,HIGH);
  } else {
    estado="ON";
    digitalWrite(pinControl,LOW);
  }
}
