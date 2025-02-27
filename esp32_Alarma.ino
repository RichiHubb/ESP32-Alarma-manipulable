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
#define BUZZER_PIN 25 // Buzzer

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

// 🔊 Secuencia de inicio de alarma
void alarmaInicio() {
    int notas[] = {741, 741, 741}; // Frecuencias en Hz
    int duraciones[] = {300, 300, 300}; // Duraciones en ms

    for (int i = 0; i < 3; i++) {
        tone(BUZZER_PIN, notas[i]); // Generar tono
        delay(duraciones[i]);
        noTone(BUZZER_PIN); // Apagar buzzer
        delay(100);
    }
}

// 🚨 Sonido de sirena alternando frecuencias
void alarmaSirena() {
    for (int i = 800; i <= 1500; i += 70) { // Subida más aguda
        tone(BUZZER_PIN, i);
        delay(40);
    }
    for (int i = 1500; i >= 800; i -= 70) { // Bajada más rápida
        tone(BUZZER_PIN, i);
        delay(40);
    }
}


void setup() {
    Serial.begin(115200);
    pinMode(RELE_PIN, OUTPUT);
    pinMode(PIR_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);

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

    // Iniciar la alarma
    Serial.println("Iniciando alarma...");
    alarmaInicio();
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

    // Mantener el sonido de la sirena activo
    alarmaSirena();
}
