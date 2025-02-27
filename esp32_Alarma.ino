#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>

// Configuración WiFi
const char* ssid = "richiAIOT";
const char* password = "hola12345";

// Configuración del servidor web
WebServer server(80);

// URL del servidor remoto
const char* serverName = "http://192.168.181.110/AIOT/Practica/tacos/config/peticiones_bd.php";

// Pines
#define PIR_PIN 27   // Sensor PIR
#define RELE_PIN 26  // Relé

// Variables
bool estadoRele = LOW;
bool ultimoEstadoPIR = LOW;
unsigned long tiempoUltimaDeteccion = 0; // Guarda el tiempo de la última detección
const unsigned long delayPIR = 5000; // Delay de 5 segundos

// Función para obtener el estado del sensor PIR
void devolver_info() {
    bool movimientoDetectado = digitalRead(PIR_PIN);
    String datos = "{\"movimiento\": \"" + String(movimientoDetectado ? "Detectado" : "No Detectado") + "\", \"rele\": \"" + (estadoRele ? "ON" : "OFF") + "\"}";
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", datos);
}

// Función para encender el relé
void encender_rele() {
    digitalWrite(RELE_PIN, HIGH);
    estadoRele = HIGH;
    enviar_datos_a_servidor("rele", estadoRele);
    devolver_info();  
}

// Función para apagar el relé
void apagar_rele() {
    digitalWrite(RELE_PIN, LOW);
    estadoRele = LOW;
    enviar_datos_a_servidor("rele", estadoRele);
    devolver_info();  
}

// Función para enviar datos a la base de datos
void enviar_datos_a_servidor(String tipo, bool estado) {
    if (WiFi.status() == WL_CONNECTED) {
        WiFiClient client;
        HTTPClient http;

        // Preparar los datos POST a enviar
        String httpRequestData = tipo + "=" + String(estado ? "1" : "0");

        Serial.print("httpRequestData: ");
        Serial.println(httpRequestData);

        // Iniciar la conexión HTTP
        http.begin(client, serverName);

        // Especificar el content type para el header del POST
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");

        // Enviar los datos por POST
        int httpResponseCode = http.POST(httpRequestData);

        // Verificar la respuesta del servidor
        if (httpResponseCode > 0) {
            Serial.print("HTTP Codigo de respuesta: ");
            Serial.println(httpResponseCode);
        } else {
            Serial.print("Codigo de error: ");
            Serial.println(httpResponseCode);
        }

        // Cerrar la conexión HTTP
        http.end();
    } else {
        Serial.println("Desconectado del wifi");
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(RELE_PIN, OUTPUT);
    pinMode(PIR_PIN, INPUT);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConectado a WiFi");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());

    // Rutas del servidor web
    server.on("/", devolver_info);
    server.on("/rele/on", encender_rele);
    server.on("/rele/off", apagar_rele);

    server.begin();
}

void loop() {
    server.handleClient();

    // Leer el estado actual del sensor PIR
    bool estadoActualPIR = digitalRead(PIR_PIN);

    // Si se detecta movimiento y ha pasado el tiempo de delay
    if (estadoActualPIR && millis() - tiempoUltimaDeteccion >= delayPIR) {
        tiempoUltimaDeteccion = millis(); // Actualizar tiempo de última detección
        ultimoEstadoPIR = estadoActualPIR;
        enviar_datos_a_servidor("movimiento", estadoActualPIR);
    }
}
