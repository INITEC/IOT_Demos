/*
*  ARD_asClient_sendADC
*  
*  El siguiente programa utiliza un arduino con un shield de Ethernet
*  el arduino trabaja como cliente, enviando consultas de tipo HTTP a un
*  servidor remoto utilizando el metodo GET para envio de informacion,
*  la informacion enviada por el arduino es la lectura de una senial analogica
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
*  @global int analogPin0 - Valor digital de la conversion de analogo a digital, varia entre 0 y 1023
*  @global int id0 - Codigo identificador del potenciometro en la base de datos del servidor
*  @global float val0 - Valor convertido entre 0.0 - 5.0
*  @global boolean lastConnected - Estado de la conexion la ultima vez que se recorrio el loop
*/
int analogPin0=0;
int id0=1;          
float val0=0.0;
boolean lastConnected = false;
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
    Serial.print(c);
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
*  function read_port(int analogPin)
*  Lee un puerto analogico con resolucion de 10bits
*  y convierte el valor en uno entre 0.0 - 5.0
*  @param int analogPin - Pin analogico que se utilizara para la conversion AD
*  @return float - retorna el valor de la lectura analogica en un rango entre 0.0 - 5.0
*  @autor: Juan Basilio
*/
float read_port(int analogPin){
  float val = analogRead(analogPin);   //read the input pin
  val=(val*5.0)/1023.0;
  Serial.print("POTA");
  Serial.print(analogPin);
  Serial.print(" : ");
  Serial.println(val);
  delay(10);
  return val;
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
    val0 = read_port(analogPin0);
    // send the HTTP PUT request:
    client.print("GET /controller/set.dataMeasure.php?id1="); // Envia los datos utilizando GET
    client.print(id0);
    client.print("&v1=");
    client.print(val0);
    client.println(" HTTP/1.1");
    client.println("Host: potenciometro.waposat.com");
    client.println("Connection: close");
    client.println();    
    delay(5000);
  } 
  else {
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    client.stop();
  }
}
