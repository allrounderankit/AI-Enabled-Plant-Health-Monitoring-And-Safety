/*****************************************************
 * AI SMART AGRICULTURE MONITORING SYSTEM
 * Developed by : ANKIT TIWARI
 * Board : ESP32 DevKit V1
 *****************************************************/

//================= BLYNK =================
#define BLYNK_TEMPLATE_ID "TMPL3mpwubS63"
#define BLYNK_TEMPLATE_NAME "Smart Plant health monitoring and safety"
#define BLYNK_AUTH_TOKEN "nMsocFl0glzjNeLUVdxXOgaajrpWAKWJ"

//================ LIBRARIES ===============
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <SPI.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

//================ WIFI ====================
char ssid[] = "ANKIT";
char pass[] = "12345678";

//================ TFT =====================
#define TFT_CS   5
#define TFT_DC   2
#define TFT_RST  15

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

//================ SENSOR PINS =============
#define DHTPIN      4
#define DHTTYPE     DHT22

#define SOIL_PIN    34
#define LDR_PIN     35
#define RAIN_PIN    32
#define WATER_PIN   33
#define SMOKE_PIN   39

#define BUZZER_PIN  27
#define LED_PIN     26

//================ LIMITS ==================
#define TEMP_LIMIT     35
#define HUM_LIMIT      80

#define SOIL_LIMIT     1800
#define LIGHT_LIMIT    2000
#define RAIN_LIMIT     1500
#define WATER_LIMIT    1500
#define SMOKE_LIMIT    500

//================ OBJECTS =================
DHT dht(DHTPIN, DHTTYPE);

BlynkTimer timer;

//================ VARIABLES ===============

float temperature;
float humidity;

int soilValue;
int lightValue;
int rainValue;
int waterValue;
int smokeValue;

String soilStatus;
String lightStatus;
String rainStatus;
String waterStatus;
String smokeStatus;
String systemStatus;

//============== ALERT FLAGS ===============

bool tempAlert=false;
bool humAlert=false;
bool soilAlert=false;
bool rainAlert=false;
bool waterAlert=false;
bool smokeAlert=false;

//=========== FUNCTION DECLARATION =========

void readSensors();
void serialMonitor();
void displayData();
void sendToBlynk();

//==========================================

void setup()
{
Serial.begin(115200);

dht.begin();

pinMode(BUZZER_PIN,OUTPUT);
pinMode(LED_PIN,OUTPUT);

digitalWrite(BUZZER_PIN,LOW);
digitalWrite(LED_PIN,LOW);

// TFT

tft.init(240,320);
tft.setRotation(1);
tft.fillScreen(ST77XX_BLACK);

tft.setTextColor(ST77XX_GREEN);
tft.setTextSize(3);
tft.setCursor(15,20);
tft.println("SMART");

tft.setCursor(15,60);
tft.println("AGRICULTURE");

tft.drawLine(0,100,320,100,ST77XX_WHITE);

tft.setTextSize(2);
tft.setCursor(20,130);
tft.println("Initializing...");

delay(2000);

// WiFi + Blynk

Blynk.begin(BLYNK_AUTH_TOKEN,ssid,pass);

timer.setInterval(2000L,sendToBlynk);

tft.fillScreen(ST77XX_BLACK);

Serial.println();
Serial.println("=================================");
Serial.println("SMART AGRICULTURE SYSTEM");
Serial.println("ESP32 READY");
Serial.println("=================================");
}

void loop()
{
Blynk.run();
timer.run();

readSensors();
serialMonitor();
displayData();

delay(2000);
}
//==================================================
// READ ALL SENSORS
//==================================================

void readSensors()
{
  // Read DHT22
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();

  // Read Analog Sensors
  soilValue = analogRead(SOIL_PIN);
  lightValue = analogRead(LDR_PIN);
  rainValue = analogRead(RAIN_PIN);
  waterValue = analogRead(WATER_PIN);
  smokeValue = analogRead(SMOKE_PIN);

  //========================
  // SOIL MOISTURE
  //========================
  if (soilValue < SOIL_LIMIT)
  {
    soilStatus = "WET";
  }
  else
  {
    soilStatus = "DRY";
  }

  //========================
  // LDR
  // (> LIMIT = DARK)
  //========================
  if (lightValue > LIGHT_LIMIT)
  {
    lightStatus = "DARK";
    digitalWrite(LED_PIN, HIGH);      // LED ON
  }
  else
  {
    lightStatus = "BRIGHT";
    digitalWrite(LED_PIN, LOW);       // LED OFF
  }

  //========================
  // RAIN SENSOR
  //========================
  if (rainValue < RAIN_LIMIT)
  {
    rainStatus = "RAIN";
  }
  else
  {
    rainStatus = "NO RAIN";
  }

  //========================
  // WATER LEVEL
  //========================
  if (waterValue < WATER_LIMIT)
  {
    waterStatus = "LOW";
  }
  else
  {
    waterStatus = "HIGH";
  }

  //========================
  // SMOKE SENSOR
  //========================
  if (smokeValue > SMOKE_LIMIT)
  {
    smokeStatus = "DETECTED";
    digitalWrite(BUZZER_PIN, HIGH);
  }
  else
  {
    smokeStatus = "SAFE";
    digitalWrite(BUZZER_PIN, LOW);
  }

  //========================
  // SYSTEM STATUS
  //========================

  systemStatus = "SAFE";

  if (temperature > TEMP_LIMIT)
    systemStatus = "WARNING";

  if (humidity > HUM_LIMIT)
    systemStatus = "WARNING";

  if (soilValue < SOIL_LIMIT)
    systemStatus = "WARNING";

  if (rainValue < RAIN_LIMIT)
    systemStatus = "WARNING";

  if (waterValue < WATER_LIMIT)
    systemStatus = "WARNING";

  if (smokeValue > SMOKE_LIMIT)
    systemStatus = "DANGER";
}
//==================================================
// SERIAL MONITOR
//==================================================

void serialMonitor()
{
  Serial.println();
  Serial.println("========================================");
  Serial.println(" SMART AGRICULTURE SYSTEM");
  Serial.println("========================================");

  Serial.print("Temperature : ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.print("Humidity    : ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Soil Value  : ");
  Serial.print(soilValue);
  Serial.print("  --> ");
  Serial.println(soilStatus);

  Serial.print("Light Value : ");
  Serial.print(lightValue);
  Serial.print("  --> ");
  Serial.println(lightStatus);

  Serial.print("Rain Value  : ");
  Serial.print(rainValue);
  Serial.print("  --> ");
  Serial.println(rainStatus);

  Serial.print("Water Level : ");
  Serial.print(waterValue);
  Serial.print("  --> ");
  Serial.println(waterStatus);

  Serial.print("Smoke Value : ");
  Serial.print(smokeValue);
  Serial.print("  --> ");
  Serial.println(smokeStatus);
  smokeValue = analogRead(39);

Serial.print("GPIO39 = ");
Serial.println(smokeValue);

  Serial.println("----------------------------------------");

  Serial.print("SYSTEM STATUS : ");
  Serial.println(systemStatus);

  Serial.println("========================================");
}

//==================================================
// TFT DISPLAY
//==================================================

void displayData()
{
  tft.fillScreen(ST77XX_BLACK);

  // Heading
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);

  tft.setCursor(20,5);
  tft.println("SMART AGRICULTURE");

  tft.drawLine(0,28,320,28,ST77XX_WHITE);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);

  tft.setCursor(5,40);
  tft.print("Temp : ");
  tft.print(temperature);
  tft.println(" C");

  tft.setCursor(5,65);
  tft.print("Hum  : ");
  tft.print(humidity);
  tft.println(" %");

  tft.setCursor(5,90);
  tft.print("Soil : ");
  tft.println(soilStatus);

  tft.setCursor(5,115);
  tft.print("Light: ");
  tft.println(lightStatus);

  tft.setCursor(5,140);
  tft.print("Rain : ");
  tft.println(rainStatus);

  tft.setCursor(5,165);
  tft.print("Water: ");
  tft.println(waterStatus);

  tft.setCursor(5,190);
  tft.print("Smoke: ");
  tft.println(smokeStatus);

  tft.setCursor(5,230);
  tft.print("STATUS : ");

  if(systemStatus == "SAFE")
  {
    tft.setTextColor(ST77XX_GREEN);
  }
  else if(systemStatus == "WARNING")
  {
    tft.setTextColor(ST77XX_YELLOW);
  }
  else
  {
    tft.setTextColor(ST77XX_RED);
  }

  tft.println(systemStatus);
}
//==================================================
// BLYNK UPDATE + ALERTS
//==================================================

void sendToBlynk()
{
  // Send Sensor Values
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, soilValue);
  Blynk.virtualWrite(V3, lightValue);
  Blynk.virtualWrite(V4, rainValue);
  Blynk.virtualWrite(V5, waterValue);
  Blynk.virtualWrite(V6, smokeValue);
  Blynk.virtualWrite(V7, systemStatus);

  //================ Temperature ==================
  if (temperature > TEMP_LIMIT)
  {
    if (!tempAlert)
    {
      Blynk.logEvent("temperature", "High Temperature Detected!");
      tempAlert = true;
    }
  }
  else
  {
    tempAlert = false;
  }

  //================ Humidity =====================
  if (humidity > HUM_LIMIT)
  {
    if (!humAlert)
    {
      Blynk.logEvent("humidity", "High Humidity Detected!");
      humAlert = true;
    }
  }
  else
  {
    humAlert = false;
  }

  //================ Soil =========================
  if (soilValue < SOIL_LIMIT)
  {
    if (!soilAlert)
    {
      Blynk.logEvent("soil", "Soil is Dry");
      soilAlert = true;
    }
  }
  else
  {
    soilAlert = false;
  }

  //================ Rain =========================
  if (rainValue < RAIN_LIMIT)
  {
    if (!rainAlert)
    {
      Blynk.logEvent("rain", "Rain Detected");
      rainAlert = true;
    }
  }
  else
  {
    rainAlert = false;
  }

  //================ Water Level ==================
  if (waterValue < WATER_LIMIT)
  {
    if (!waterAlert)
    {
      Blynk.logEvent("water", "Water Tank Low");
      waterAlert = true;
    }
  }
  else
  {
    waterAlert = false;
  }

  //================ Smoke ========================
  if (smokeValue > SMOKE_LIMIT)
  {
    if (!smokeAlert)
    {
      Blynk.logEvent("smoke", "Smoke Detected");
      smokeAlert = true;
    }
  }
  else
  {
    smokeAlert = false;
  }
}