
/*
* Servidor web con ESP32
* Librerias necesarias para conectarnos a un entorno Wifi y poder configurar  
* un servidor WEB
*/
#ifdef ESP32
  #include <WiFi.h>
  #include <WebServer.h>
#else
  #include <ESP8266WiFi.h>
  #include <WiFiClient.h>
#endif

/* Añade tu SSID & Clave para acceder a tu Wifi */
const char* ssid = "";  // Tu SSID
const char* password = "";  // Tu Clave

/*
    Declaramos un objeto de la libreria WebServer para poder acceder a sus funciones
    Y como parametro 80, que es el puerto estandar de todos los servicios WEB HTTP
*/
WebServer server(80);

/*
   Declaramos el estado inicial de los pines
*/
const int pirPin = 32; // Pin digital para el sensor PIR
const int relayPin = 18; // Pin digital para el relé
const int ledPin = 19; // Pin digital para el LED
const int buttonPin = 23; // Pin digital para el botón

bool relayEstado = LOW;
bool movimientoDetectado = false;

/*
   Todos las siguientes funciones ejecutan tres tareas:
   1 Cambian de estado las variables  de los LED
   2 Muestran por el monitor Serial de Arduino IDE, informacion relevante al estado de los LED
   3 Actualizan la vista de la pagina del servidor WEB con ESP32, envia al navegador un codigo 200 indican el exito
   de la conexion y llama a otra funcion SendHTML con dos parametros que modificaran la pagina 
   del servidor WEB con Arduino.
*/

// Definimos funciones
void devolver_info() 
{
  String datos;
  if(relayEstado == LOW)
  {
    datos = "{\"estado\": \"OFF\"}";
  }
  else
  {    
    datos = "{\"estado\": \"ON\"}";
  }
  Serial.println(datos); // 2
  server.sendHeader("Access-Control-Allow-Origin","*");
  server.send(200, "application/json", datos); // 3
}

void encender_rele() 
{
  digitalWrite(relayPin, HIGH); //1
  relayEstado = HIGH;
  digitalWrite(ledPin, HIGH);
  String datos = "{\"estado\": \"ON\"}";
  Serial.println(datos); // 2
  server.sendHeader("Access-Control-Allow-Origin","*");
  server.send(200, "application/json", datos); //3
}

void apagar_rele() 
{
  digitalWrite(relayPin, LOW); //1
  relayEstado = LOW;
  digitalWrite(ledPin, LOW);
  String datos = "{\"estado\": \"OFF\"}";
  Serial.println(datos); // 2
  server.sendHeader("Access-Control-Allow-Origin","*");
  server.send(200, "application/json", datos); //3
}

void setup() 
{
   /*
   * Declaracion de la velocidad de comunicacion entre Arduino IDE y ESP32
   * Configura el comportamiento de los pines
   */
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);

/*
 * Configuracion de la conexion a la Wifi de tu casa
 */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Conectado a ");
  Serial.println(ssid);
  Serial.print("Direccion IP: ");
  Serial.println(WiFi.localIP());

/*
 * Para procesar las solicitudes HTTP necesitamos definir el codigo que debe de ejecutar en
 * cada estado. Para ello utilizamos el metodo "on" de la libreria WebServer que hemos 
 * habilitador en la linea 13 de este codigo
 * 1 El primero se ejecuta cuando te conectas al Servidor WEB con ESP32 http://la_ip_del_esp32/
 * 2 Los 4 siguientes procesan los 2 estados que puede tener cada LED ON/OFF
 * 3 El ultimo gestiona los errores por ejemplo si pones http://la_ip_del_esp32/holaquetal
 * esta pagina no existe, por lo tanto actualizara la pagina WEB con un mensaje de error
 */
 
   server.on("/", devolver_info); // 1
   server.on("/rele/on", encender_rele);
   server.on("/rele/off", apagar_rele);

/*
 * Arrancamos el Servicio WEB
 */
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() 
{
  // Verificar si se detecta movimiento
  if (digitalRead(pirPin) == HIGH) {
    if (!movimientoDetectado) {
      movimientoDetectado = true;
      encender_rele();
    }
  }

  // Verificar si se presiona el botón para apagar
  if (digitalRead(buttonPin) == LOW) {
    if (movimientoDetectado) {
      movimientoDetectado = false;
      apagar_rele();
    }
  }

  // Manejar las solicitudes del servidor web
  server.handleClient();
}
