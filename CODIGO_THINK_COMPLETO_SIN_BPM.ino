#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <ThingSpeak.h>

// Define los pines SDA y SCL
#define SDA_PIN 41 // Cambia este valor según el pin que quieras usar
#define SCL_PIN 42 // Cambia este valor según el pin que quieras usar

// Configuración de la dirección I2C de la pantalla LCD
#define LCD_ADDRESS 0x27 // Cambia esto si tu dirección es diferente

// Inicializa el objeto LiquidCrystal_I2C
LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2); // 16 columnas y 2 filas

// Definición de pines para los sensores (fijos)
const int pinTemp = 18;    // Pin para el sensor de temperatura
const int pinPresion = 8; // Pin para el sensor de presión
const int pinPH = 17;      // Pin para el sensor de pH
const int pinCO2 = 15;     // Pin para el sensor de CO2

int sensorValue = 0; // Valor leído del sensor
unsigned long lastTime = 0; // Tiempo de la última lectura
int beatCount = 0; // Contador de latidos
const int threshold = 2150; // Umbral para detectar un latido

// Credenciales de WiFi
const char* ssid = "Rrr";
const char* password = "09112005";

// Credenciales de ThingSpeak
unsigned long channelID = 2681634; // Define esto antes de cargar
const char* writeAPIKey = "F45JNVY8JSW3RR2J"; // Define esto antes de cargar

WiFiClient client;

void setup() {
    Serial.begin(115200);
    
    // Inicia la comunicación I2C con los pines definidos
    Wire.begin(SDA_PIN, SCL_PIN);
    
    // Inicia la pantalla LCD
    lcd.init(); // Cambia lcd.begin() por lcd.init()
    lcd.backlight(); // Enciende la luz de fondo
    
    // Muestra un mensaje en la pantalla LCD
    lcd.setCursor(0, 0);
    lcd.print("Iniciando...");
    delay(1000);
    lcd.clear();

    // Conexión a la red WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConectado al WiFi");

    // Inicializar ThingSpeak
    ThingSpeak.begin(client);
}

void loop() {
    // Leer valores de los sensores
    float temperatura = analogRead(pinTemp) * (3.3 / 4090.0) * 100; // Ejemplo de conversión
    float presion = analogRead(pinPresion) * (3.3 / 4090.0) * 100; // Ejemplo de conversión
    float pH = analogRead(pinPH) ; // Ejemplo de conversión
    int CO2 = analogRead(pinCO2);  // Ejemplo de conversión
    
    Serial.println(CO2);
    float co2PPM = map(CO2, 3800, 1000, 100, 0);

    // Imprimir los valores en el monitor serial
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.println(" °C");


    // Detecta si hay un latido
    if (pH > threshold) {
        beatCount++;
        delay(300); // Espera un tiempo para evitar múltiples detecciones de un solo latido
    }

    // Contabiliza el tiempo
    unsigned long currentTime = millis();
    if (currentTime - lastTime >= 1000) { // Cada segundo
        Serial.print("Latidos en 1 segundo: ");
        Serial.println(beatCount);
        
        // Calcula BPM
        float bpm = beatCount * 60; // Conversión a BPM
        Serial.print("Frecuencia cardíaca: ");
        Serial.print(bpm);
        Serial.println(" BPM");

        // Reinicia el contador y el tiempo
        lastTime = currentTime;
        beatCount = 0;
    }


    Serial.print("Presion: ");
    Serial.print(presion);
    Serial.println(" hPa");

    Serial.print("CO2: ");
    Serial.print(co2PPM);
    Serial.println("%");

    // Mostrar los valores en el LCD
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperatura);
    lcd.print("C");

    lcd.setCursor(0, 1);
    lcd.print("CO2: ");
    lcd.print(co2PPM);
    lcd.println("%");

    // Enviar datos a ThingSpeak
    ThingSpeak.setField(1, temperatura);
    ThingSpeak.setField(2, presion);
    ThingSpeak.setField(3, pH);
    ThingSpeak.setField(4, co2PPM);
    ThingSpeak.writeFields(channelID, writeAPIKey);

    delay(500); // Esperar 2 segundos antes de la siguiente lectura
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Presion: ");
    lcd.print(presion);
    lcd.print("hPa");

    lcd.setCursor(0, 1);
    lcd.print("pH: ");
    lcd.print(pH);
    delay(500); // Esperar 2 segundos antes de la siguiente lectura
    lcd.clear();
}
