#include <SPI.h>
#include <Ethernet.h>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte ip[] = { 192, 168, 0, 200 };

EthernetServer server(80);
// Definimos constantes para el primer led RGB
#define up 3
#define down 5
#define left 6
#define right 9

const int motorPin1  = 3;  // Pin 14 of L293
const int motorPin2  = 4;  // Pin 10 of L293
const int motorPin3  = 6; // Pin  7 of L293
const int motorPin4  = 5;  // Pin 

String estado="OFF"; //Estado del Led inicialmente "OFF"
void setup(){
  // Inicio setup()
  Serial.begin(9600);
  delay(1000);
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
  Serial.println("Iniciando ...");
  delay(1000);
  // Estableciendo puertos como salida para el primer led RGB
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  
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
        if(cadena.substring(posicion)=="LED=up"){//Si a la posición 'posicion' hay "LED=ON"
          controlCarro(1);
          estado="AVANZA";
        }
        if(cadena.substring(posicion)=="LED=down"){//Si a la posición 'posicion' hay "LED=ON"
          controlCarro(2);
          estado="RETROCEDE";
        }
        if(cadena.substring(posicion)=="LED=left"){//Si a la posición 'posicion' hay "LED=ON"
          controlCarro(3);
          estado="GIRAR IZ";
        }
        if(cadena.substring(posicion)=="LED=right"){//Si a la posición 'posicion' hay "LED=ON"
          controlCarro(4);
          estado="right";
        }
        if(cadena.substring(posicion)=="LED=OFF"){//Si a la posición 'posicion' hay "LED=OFF"
          controlCarro(0);
          estado="OFF";
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
            client.println("<button onClick=location.href='./?LED=up\' style='margin:auto;background-color: #428bca;color: snow;padding: 10px;border: 1px solid #428bca;width:23%;height:43%'>");
            client.println("START");
            client.println("</button>");
            client.println("<button onClick=location.href='./?LED=down\' style='margin:auto;background-color: #5cb85c;color: snow;padding: 10px;border: 1px solid #5cb85c;width:23%;height:43%'>");
            client.println("BACK");
            client.println("</button>");
            client.println("<button onClick=location.href='./?LED=right\' style='margin:auto;background-color: #5bc0de;color: snow;padding: 10px;border: 1px solid #5bc0de;width:23%;height:43%'>");
            client.println("RIGHT");
            client.println("</button>");
            client.println("<button onClick=location.href='./?LED=left\' style='margin:auto;background-color: #f0ad4e;color: snow;padding: 10px;border: 1px solid #f0ad4e;width:23%;height:43%'>");
            client.println("LEFT");
            client.println("</button>");
            client.println("<button onClick=location.href='./?LED=OFF\' style='margin:auto;background-color: #d9534f;color: snow;padding: 10px;border: 1px solid #d9534f;width:100%;height:43%'>");
            client.println("OFF");
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
void controlCarro(int estado){
  /*
    estado:
    0 ->  Stop
    1 ->  Avanzar
    2 ->  Retroceder
    3 ->  Girar Izquierda
    4 ->  Girar Derecha
  */
  
  switch (estado) {
    case 1:
      Serial.println( "Avanzar");
      //MOTOR B DERECHA
      digitalWrite(motorPin1, HIGH);
      digitalWrite(motorPin2, LOW);
      //MOTOR A IZQUIERDA
      digitalWrite(motorPin3, LOW);
      digitalWrite(motorPin4, HIGH);
                
      break;
    case 2:
      Serial.println( "Retroceso");
      //MOTOR B DERECHA
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, HIGH);
      //MOTOR A IZQUIERDA
      digitalWrite(motorPin3, HIGH);
      digitalWrite(motorPin4, LOW);
                
      break;
    case 3:
      Serial.println( "Izquierda");
      //MOTOR B DERECHA
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, HIGH);
      //MOTOR A IZQUIERDA
      digitalWrite(motorPin3, LOW);
      digitalWrite(motorPin4, HIGH);
                
      break;
    case 4:
      Serial.println( "Derecha");
      //MOTOR B DERECHA
      digitalWrite(motorPin1, HIGH);
      digitalWrite(motorPin2, LOW);
      //MOTOR A IZQUIERDA
      digitalWrite(motorPin3, HIGH);
      digitalWrite(motorPin4, LOW);
      
      break;
    case 0:
      Serial.println("Apagar");
      //MOTOR B DERECHA
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, LOW);
      //MOTOR A IZQUIERDA
      digitalWrite(motorPin3, LOW);
      digitalWrite(motorPin4, LOW);
            
      break;
    default: 
      Serial.println("OFF by Default");
      //MOTOR B DERECHA
      digitalWrite(motorPin1, LOW);
      digitalWrite(motorPin2, LOW);
      //MOTOR A IZQUIERDA
      digitalWrite(motorPin3, LOW);
      digitalWrite(motorPin4, LOW);
      // default is optional
    break;
  }

}
