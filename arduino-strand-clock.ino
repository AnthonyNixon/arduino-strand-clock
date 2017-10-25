/*
 *  Simple HTTP get webclient test
 */
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#define PIN 15


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);
 
const char* ssid     = "";
const char* password = "";

// NTP Configuration
const char*   ntpServer   = "time.nist.gov";
const int8_t  ntpOffset   = -5; // hours
const uint8_t ntpInterval = 5; // minutes

const float minuteMult = 4.32;
const float secondMult = 4.32;
const float hourMult = 11.08;

// ESP8266 microseconds of compensation per sleep-cycle minute
const uint16_t espOffset  = 2000000; // microseconds

// Struct to store ESP8266 rst_info
struct rst_info {
  uint32 reason;
  uint32 exccause;
  uint32 epc1;
  uint32 epc2;
  uint32 epc3;
  uint32 excvaddr;
  uint32 depc;
} *rsti;

struct {
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
  uint8_t nextNtp;
} timeStruct;

struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} colorStruct;

long currentTime;
long lastTime;
long difference;
uint16_t elapsedSeconds;

void syncTime() {
  Serial.println("Syncing Time...");
  elapsedSeconds = 0;
  WiFiUDP udp;
  NTPClient timeClient(udp, ntpServer, ntpOffset * 3600);
  timeClient.begin();
  while (!timeClient.update()) yield();
    timeStruct.hours = timeClient.getHours();
    timeStruct.minutes = timeClient.getMinutes();
    timeStruct.seconds = timeClient.getSeconds();
  Serial.println(timeClient.getFormattedTime());
  timeStruct.nextNtp = ntpInterval;
  Serial.println("done");
  Serial.println();
}

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  pinMode(0, OUTPUT);
  Serial.begin(115200);
  delay(100);
 
  // We start by connecting to a WiFi network
 
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  syncTime();
}
 
int value = 0;
 
void loop() {
  timeStruct.seconds++;
  if (timeStruct.seconds==60){
    timeStruct.seconds = 0;
    timeStruct.minutes++;
  }
  if (timeStruct.minutes==60){
    timeStruct.minutes = 0;
    timeStruct.hours++;
  }
  colorStruct.r = timeStruct.hours * hourMult;
  colorStruct.g = timeStruct.minutes * minuteMult;
  colorStruct.b = timeStruct.seconds * secondMult;
  

   for( int i = 0; i<60; i++){
        if (i < timeStruct.minutes) {
          strip.setPixelColor(i, colorStruct.r, colorStruct.g, colorStruct.b);
        } else {
          strip.setPixelColor(i, 0, 0, 0);
        }
        
    } 
    strip.setPixelColor(timeStruct.seconds, (255 - colorStruct.r), (255 - colorStruct.g), (255 - colorStruct.b));
    strip.show();
    
  Serial.println(timeStruct.hours);
  Serial.println(timeStruct.minutes);
  Serial.println(timeStruct.seconds);
  Serial.printf("%d %d %d\n", colorStruct.r, colorStruct.g, colorStruct.b);
  Serial.println();



  lastTime=millis();
  delay(998);
  elapsedSeconds++;
  if (elapsedSeconds >= 86400) {
    syncTime();
  }
  
  
  }
