/*
*  El siguiente programa utiliza un arduino con un shield de Ethernet
*  el arduino trabaja como un servidor, recibiendo las consultas desde cualquier
*  dispositivo conectado a su misma LAN, con una senal de salida digital activa o 
*  desactiva un relay por medido de las consultas enviadas desde los otros dispositivos
*  conectados en la misma red.
*  CONSIDERAICION
*  Para este ejemplo, se requiere que el shield de ethernet este conectado a un router con DHCP
*/

#include <SPI.h>
#include <Ethernet.h>

// Configuracion por defecto
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168,0,177);

// Definimos al arduino como servidor web por el puerto 80
EthernetServer server(80);

// Definimos la constante del led que se encendera
#define pinControl 3

String estado="OFF"; //Estado del Led inicialmente "OFF"

void setup(){
  Serial.begin(9600);
  delay(1000);
  Serial.print("Esperando obtener IP ... ");
  // Probamos iniciar la tarjeta de red con DHCP
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  
  server.begin();
  
  // Estableciendo puertos como salida para el primer led RGB
  pinMode(pinControl, OUTPUT);

}// Fin setup()



void loop(){ // Inicio loop()

  EthernetClient client = server.available(); //Creamos un cliente Web
  if (client) {
    boolean currentLineIsBlank = true; //Una petición HTTP acaba con una línea en blanco
    String cadena=""; //Creamos una cadena de caracteres vacía  
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();//Leemos la petición HTTP carácter por carácter
        Serial.write(c);//Visualizamos la petición HTTP por el Monitor Serial
        cadena.concat(c);//Unimos el String 'cadena' con la petición HTTP (c). De esta manera convertimos la petición HTTP a un String
 
        //Ya que hemos convertido la petición HTTP a una cadena de caracteres, ahora podremos buscar partes del texto.
        int posicion=cadena.indexOf("LED="); //Guardamos la posición de la instancia "LED=" a la variable 'posicion'
 
        if(cadena.substring(posicion)=="LED=ON"){//Si a la posición 'posicion' hay "LED=ON"
          changePin(1);
        }
        if(cadena.substring(posicion)=="LED=OFF"){//Si a la posición 'posicion' hay "LED=ON"
          changePin(0);
        }
        //Cuando reciba una línea en blanco, quiere decir que la petición HTTP ha acabado y el servidor Web está listo para enviar una respuesta
        if (c == '\n' && currentLineIsBlank) {
 
            // Enviamos al cliente una respuesta HTTP
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println();
            //Página web en formato HTML
            client.println("<html>");
            client.println("<head>");
            client.println("</head>");
            client.println("<body>");
            //Creamos los botones. Para enviar parametres a través de HTML se utiliza el metodo URL encode. Los parámetros se envian a través del símbolo '?'
            client.println("<div style='text-align:center;'>");
            client.print("<button onClick=location.href='./?LED=");
            if (estado == "ON"){
              client.print("OFF");
            } else {
              client.print("ON");
            }
            client.println("\' style='margin:auto;background-color: #d9534f;color: snow;padding: 10px;border: 1px solid #d9534f;width:100%;height:43%'>");
            client.println("PUSH");
            client.println("</button>");
            client.println("<br /><br />");
            client.println("<b>ESTADO = ");
            client.print(estado);
            client.println("</b><br />");
            client.println("</b></body>");
            client.println("</html>");
            break;
        }
        if (c == '\n') {
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          currentLineIsBlank = false;
        }  
      }
    }
    //Dar tiempo al navegador para recibir los datos
    delay(1);
    client.stop();// Cierra la conexión
  }
} // Fin loop()

void changePin(int state){
  if (state == 0){
    estado="OFF";
    digitalWrite(pinControl,LOW);
  } else {
    estado="ON";
    digitalWrite(pinControl,HIGH);
  }

}
