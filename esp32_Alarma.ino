#include <WiFi.h>
#include <WebServer.h>

// Configuración WiFi
const char* ssid = "richiAIOT";
const char* password = "hola12345";

// Configuración del servidor web
WebServer server(80);

// Pines
#define PIR_PIN 13   // Sensor PIR
#define RELE_PIN 26  // Relé

// Variables
bool estadoRele = LOW;
bool movimientoDetectado = false;

// Función para obtener el estado del sensor PIR
void devolver_info() {
    movimientoDetectado = digitalRead(PIR_PIN);
    String datos = "{\"movimiento\": \"" + String(movimientoDetectado ? "Detectado" : "No Detectado") + "\", \"rele\": \"" + (estadoRele ? "ON" : "OFF") + "\"}";
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", datos);
}

// Función para encender el relé
void encender_rele() {
    digitalWrite(RELE_PIN, HIGH);
    estadoRele = HIGH;
    devolver_info();
}

// Función para apagar el relé
void apagar_rele() {
    digitalWrite(RELE_PIN, LOW);
    estadoRele = LOW;
    devolver_info();
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
}
