#include <Arduino.h>
//#include <ArduinoOTA.h>
#include <WiFi.h>
#include <WebServer.h>
#include <AutoConnect.h>
#include <ArduinoJson.h>
#include <time.h>
#include <config.h>

// TFT Display
#include <TFT_eSPI.h> 

/////////////////////////////////////////////////////////////////////////// TFT Setup
TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

// Logo laden
#include <btc_logo.h>
#include <block_logo.h>
// Erweiterungen laden
#include <btc_mempool.h>
#include <uhr.h>
#include <btc_kurs.h>




WebServer Server;

AutoConnect       Portal(Server);
AutoConnectConfig Config;       // Enable autoReconnect supported on v0.9.4


int restart_nach_ap = 0;

/////////////////////////////////////////////////////////////////////////// Funktionsprototypen
//void callback                (char*, byte*, unsigned int);
void loop                      ();
void btc_mempool               ();
void rootPage                  ();
void Zeit_Datum                ();
void Zeit_Uhrzeit              ();
void btc_kurs                  ();
void display_del_nach_setup    ();


/////////////////////////////////////////////////////////////////////////// Intervall der Steuerung
unsigned long startSCHLEIFE_btckurs = 0;
unsigned long intervSCHLEIFE_btckurs = 10000; 

unsigned long startSCHLEIFE_zeit = 0;
unsigned long intervSCHLEIFE_zeit = 2000; 

unsigned long startSCHLEIFE_btcmempool = 0;
unsigned long intervSCHLEIFE_btcmempool = 40000; 

// Intervall MEMblock animation
const unsigned long mempool_animation_gesamtzyklus= 3*1000L; 
unsigned long mempool_animation_start = 0;

int wechsler = 0;


/////////////////////////////////////////////////////////////////////////// WiFi Root Page 
void rootPage() {
  String  content =
    "<html>"
    "<head>"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
    "</head>"
    "<body>"
    "<h2 align=\"center\" style=\"color:blue;margin:20px;\">Bitcoin Ticker</h2>"
    "<p style=\"text-align:center;\">esp32 Wifi Autoconnect</p>"
    "<p></p><p style=\"padding-top:15px;text-align:center\">" AUTOCONNECT_LINK(COG_24) "</p>"
    "</body>"
    "</html>";
  Server.send(200, "text/html", content);

}



/////////////////////////////////////////////////////////////////////////// display_del_nach_setup
void display_del_nach_setup(){

// Display nach dem Setup löschen

if (restart_nach_ap == 1) {
  Serial.println("Display löschen");
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  tft.setTextSize(1);
  
  restart_nach_ap = 2;
} 

}


/////////////////////////////////////////////////////////////////////////// Setup
void setup() {
  Serial.begin(115200);
  Serial.println("Setup start");

 //OTA Setup für Firmware
 // ArduinoOTA.setHostname("24KanalRelaisWohnzimmer");
//  ArduinoOTA.setPassword("7n6WkRpZtxtkykyMUx329");
 // ArduinoOTA.begin();  

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);
  tft.setTextSize(1);
  // text 1

  tft.setCursor(5, 2, 2);
  tft.println("Verbinden Sie sich bitte ");
  tft.setCursor(5, 22, 2);
  tft.println("mit Accesspoint esp32ap");
    delay(20);
  tft.setCursor(5, 42, 2);
  tft.println("Passwort 12345678");  
   delay(20);
  tft.setCursor(5, 62, 2);
  tft.println("Loggen Sie sich in Ihr");  
  tft.setCursor(5, 82, 2);
  tft.println("Wlan ein über das Portal.");   
     delay(20); 


  // Enable saved past credential by autoReconnect option,
  // even once it is disconnected.
  Config.autoReconnect = true;
  Config.hostName = "esp32-01";
  Portal.config(Config);

  // Behavior a root path of ESP8266WebServer.
  Server.on("/", rootPage);

  Serial.println("Creating portal and trying to connect...");
  // Establish a connection with an autoReconnect option.
  if (Portal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
    Serial.println(WiFi.getHostname());
    restart_nach_ap = 1;
  }


  ////////////////////////////////////////////////////// ntp Server init
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
 

}


/////////////////////////////////////////////////////////////////////////// Loop
void loop() {
  // Wifi Portal starten
  Portal.handleClient();

// Display löschen
display_del_nach_setup();


    // Block text ausgebn
  tft.setRotation(0);
  tft.setTextColor(TFT_ORANGE,TFT_BLACK);
  tft.setCursor(85, 3);
  tft.setTextSize(1);
  tft.print("BLOCK");
  tft.setRotation(1);

// Wuerfel lauflicht
if ( millis() - mempool_animation_start >= mempool_animation_gesamtzyklus ) mempool_animation_start = millis();
unsigned long phase = millis() - mempool_animation_start;  // phase ist immer im Bereich 0 .. gesamtzyklus

  if (phase < 1000 && phase > 800) {
    // TFT Ausgabe Wuerfel
    tft.drawBitmap(30, 5, blocklogo, 42, 42, TFT_GREEN);
    tft.drawBitmap(74, 5, blocklogo, 42, 42, TFT_BLUE);
    tft.drawBitmap(118, 5, blocklogo, 42, 42, TFT_BLUE);
  }

  if (phase < 2000 && phase > 1800) {
    tft.drawBitmap(30, 5, blocklogo, 42, 42, TFT_BLUE);
    tft.drawBitmap(74, 5, blocklogo, 42, 42, TFT_GREEN);
    tft.drawBitmap(118, 5, blocklogo, 42, 42, TFT_BLUE);
  }

  if (phase < 3000 && phase > 2800) {
    tft.drawBitmap(30, 5, blocklogo, 42, 42, TFT_BLUE);
    tft.drawBitmap(74, 5, blocklogo, 42, 42, TFT_BLUE);
    tft.drawBitmap(118, 5, blocklogo, 42, 42, TFT_GREEN);
  }



      ///////////////////////////////////////////////////////////////////////// ZEIT abfragen
      if (millis() - startSCHLEIFE_btcmempool > intervSCHLEIFE_btcmempool) {
          startSCHLEIFE_btcmempool = millis();   // aktuelle Zeit abspeichern
          // MEMpool abfragen
          btc_mempool();
          Serial.println("Mempool abfragen");
        }


///////////////////////////////////////////////////////////////////////// BTC Kurs abfragen

  if (millis() - startSCHLEIFE_btckurs > intervSCHLEIFE_btckurs) {
      startSCHLEIFE_btckurs = millis();   // aktuelle Zeit abspeichern
      // BTC Kurs abfragen
      btc_kurs();

      Serial.println("BTC");

   } 



///////////////////////////////////////////////////////////////////////// ZEIT abfragen
      if (millis() - startSCHLEIFE_zeit > intervSCHLEIFE_zeit) {
          startSCHLEIFE_zeit = millis();   // aktuelle Zeit abspeichern
          // BTC Kurs abfragen
          Zeit_Datum();
          Zeit_Uhrzeit();
 }





}