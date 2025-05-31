#include <Arduino.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <webpage.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// WiFi credentials
const char *ssid = "OPTIMUS";
const char *password = "qqwweeaaaa";

// MAX485 control pins
#define RE 4
#define DE 5
HardwareSerial npkSerial(1); // UART1 using 16(RX)/17(TX)

// Sensor configuration
#define NPK_ADDRESS 0x01
#define READ_HOLDING_REGISTERS 0x03

// Register addresses
#define NITROGEN_REGISTER 0x001E
#define PHOSPHORUS_REGISTER 0x001F
#define POTASSIUM_REGISTER 0x0020

// DHT22 config
#define DHTPIN 13
#define DHTTYPE DHT22

// GPS Configuration 
#define GPS_RX 25 // GPS TX -> ESP32 RX (25)
#define GPS_TX 26 // GPS RX -> ESP32 TX (26)
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);

// Web serverx
WebServer server(80);

// Sensor data structure
struct SensorData
{
    float nitrogen = -1;
    float phosphorus = -1;
    float potassium = -1;
    float temperature = -1;
    float humidity = -1;
    double latitude = 0;
    double longitude = 0;
    String lastUpdate = "Never";
};

// Global objects
DHT dht(DHTPIN, DHTTYPE);
TinyGPSPlus gps;
SensorData sensorData;

// Timing variables
unsigned long lastNPKRead = 0;
unsigned long lastGPSRead = 0;
unsigned long lastDHTRead = 0;
const unsigned long NPK_INTERVAL = 5000;
const unsigned long GPS_INTERVAL = 10000;
const unsigned long DHT_INTERVAL = 2000;
bool initialGPS_Read = false;

// Function prototypes
void setup();
void loop();
float readNPKParameter(uint16_t registerAddress);
uint16_t calculateCRC(uint8_t *bytes, uint16_t length);
void printHex(uint8_t *data, uint8_t length);
void readGPS();
void updateTimestamp();
void handleRoot();
void handleData();
String getContentType(String filename);
void handleNotFound();
void initWebServer();

// NPK Sensor Functions
float readNPKParameter(uint16_t registerAddress)
{
    uint8_t request[8] = {
        NPK_ADDRESS,
        READ_HOLDING_REGISTERS,
        highByte(registerAddress),
        lowByte(registerAddress),
        0x00,
        0x01,
        0x00,
        0x00};

    uint16_t crc = calculateCRC(request, 6);
    request[6] = lowByte(crc);
    request[7] = highByte(crc);

    digitalWrite(DE, HIGH);
    digitalWrite(RE, HIGH);
    delay(10);
    npkSerial.write(request, sizeof(request));
    npkSerial.flush();
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);
    delay(10);

    uint8_t response[7];
    uint8_t idx = 0;
    unsigned long startTime = millis();

    while (millis() - startTime < 1000 && idx < sizeof(response))
    {
        if (npkSerial.available())
        {
            response[idx++] = npkSerial.read();
        }
    }

    if (idx >= 5 && response[0] == NPK_ADDRESS && response[1] == READ_HOLDING_REGISTERS && response[2] == 0x02)
    {
        return (response[3] << 8) | response[4];
    }
    return -1.0;
}

uint16_t calculateCRC(uint8_t *bytes, uint16_t length)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t pos = 0; pos < length; pos++)
    {
        crc ^= (uint16_t)bytes[pos];
        for (int i = 8; i != 0; i--)
        {
            if ((crc & 0x0001) != 0)
            {
                crc >>= 1;
                crc ^= 0xA001;
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}

void printHex(uint8_t *data, uint8_t length)
{
    for (int i = 0; i < length; i++)
    {
        if (data[i] < 0x10)
            Serial.print("0");
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}

void readGPS()
{

    unsigned long start = millis();
    while (millis() - start < 1000)
    {
        while (gpsSerial.available() > 0)
        {
            if (gps.encode(gpsSerial.read()))
            {
                if (gps.location.isValid())
                {
                    
                    Serial.print("Location");
                    Serial.print(sensorData.latitude);
                    Serial.print("        ");
                    Serial.print(sensorData.longitude);
                    sensorData.latitude = gps.location.lat();
                    sensorData.longitude = gps.location.lng();
                    return;
                }
            }
            yield(); // Critical for WiFi stability
        }
    }
}


void handleRoot()
{
    server.send(200, "text/html", html);
}

void handleData()
{
    String json = "{";
    json += "\"nitrogen\":" + String(sensorData.nitrogen) + ",";
    json += "\"phosphorus\":" + String(sensorData.phosphorus) + ",";
    json += "\"potassium\":" + String(sensorData.potassium) + ",";
    json += "\"temperature\":" + String(sensorData.temperature) + ",";
    json += "\"humidity\":" + String(sensorData.humidity) + ",";
    json += "\"latitude\":" + String(sensorData.latitude, 6) + ",";
    json += "\"longitude\":" + String(sensorData.longitude, 6) + ",";
    json += "\"lastUpdate\":\"" + sensorData.lastUpdate + "\"";
    json += "}";

    server.send(200, "application/json", json);
}

void handleNotFound()
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++)
    {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

void initWebServer()
{
    server.on("/", handleRoot);
    server.on("/data", handleData);
    server.onNotFound(handleNotFound);
    server.begin();
}

// Main Functions
void setup()
{
    Serial.begin(115200);

    // Initialize NPK sensor serial
    npkSerial.begin(9600, SERIAL_8N1, 16, 17);
    pinMode(RE, OUTPUT);
    pinMode(DE, OUTPUT);
    digitalWrite(DE, LOW);
    digitalWrite(RE, LOW);

    // Initialize DHT
    dht.begin();

    // Initialize GPS SoftwareSerial
    gpsSerial.begin(9600);
    pinMode(GPS_RX, INPUT);
    pinMode(GPS_TX, OUTPUT);

    // Connect to WiFi
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Setup web server
    initWebServer();
    Serial.println("HTTP server started");
}

void loop()
{
    server.handleClient();

    unsigned long currentMillis = millis();

    
    // Read NPK sensors periodically
    if (currentMillis - lastNPKRead >= NPK_INTERVAL)
    {
        sensorData.nitrogen = readNPKParameter(NITROGEN_REGISTER);
        sensorData.phosphorus = readNPKParameter(PHOSPHORUS_REGISTER);
        sensorData.potassium = readNPKParameter(POTASSIUM_REGISTER);
        lastNPKRead = currentMillis;
        updateTimestamp();
    }

    // Read DHT sensor periodically
    if (currentMillis - lastDHTRead >= DHT_INTERVAL)
    {
        sensorData.temperature = dht.readTemperature();
        sensorData.humidity = dht.readHumidity();
        lastDHTRead = currentMillis;
        updateTimestamp();
    }

    // Read GPS periodically
    if (currentMillis - lastGPSRead >= GPS_INTERVAL || !initialGPS_Read)
    {
        initialGPS_Read = true;
        readGPS();
        lastGPSRead = currentMillis;
        updateTimestamp();
    }
    
}

void updateTimestamp()
{
    time_t now = time(nullptr);
    sensorData.lastUpdate = String(ctime(&now));
    sensorData.lastUpdate.trim();
}