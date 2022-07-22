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
unsigned long intervSCHLEIFE_btckurs = 5000; 

unsigned long startSCHLEIFE_zeit = 0;
unsigned long intervSCHLEIFE_zeit = 2000; 

unsigned long startSCHLEIFE_btcmempool = 0;
unsigned long intervSCHLEIFE_btcmempool = 10000; 

// wechseln zwischen BTC Kurs und Blockzeit 
unsigned long wechsler_millis_zaehler = 0;        // will store last time LED was updated
const long wechsler_interval = 10000;           // interval at which to blink (milliseconds)



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
    delay(500);
  tft.setCursor(5, 42, 2);
  tft.println("Passwort 12345678");  
   delay(500);
  tft.setCursor(5, 62, 2);
  tft.println("Loggen Sie sich in Ihr");  
  tft.setCursor(5, 82, 2);
  tft.println("Wlan ein über das Portal.");   
     delay(500); 


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

 // /////////////////////////////////////////////////////////////////////////// wechsler
  unsigned long wechsler_start_millis = millis();
  if (wechsler_start_millis - wechsler_millis_zaehler >= wechsler_interval) {

    wechsler_millis_zaehler = wechsler_start_millis;


    if (wechsler == 0) {
      wechsler = 1;
      Serial.println("BTC");
      btc_kurs();
    } else {
      wechsler = 0;
      Serial.println("Block");
      btc_mempool();
    }
  }

/*
///////////////////////////////////////////////////////////////////////// BTC Kurs abfragen

  if (millis() - startSCHLEIFE_btckurs > intervSCHLEIFE_btckurs) {
      startSCHLEIFE_btckurs = millis();   // aktuelle Zeit abspeichern
      // BTC Kurs abfragen
      btc_kurs();

      Serial.println("BTC");

   } 
*/


///////////////////////////////////////////////////////////////////////// ZEIT abfragen
      if (millis() - startSCHLEIFE_zeit > intervSCHLEIFE_zeit) {
          startSCHLEIFE_zeit = millis();   // aktuelle Zeit abspeichern
          // BTC Kurs abfragen
          Zeit_Datum();
          Zeit_Uhrzeit();
 }

/*
      ///////////////////////////////////////////////////////////////////////// ZEIT abfragen
      if (millis() - startSCHLEIFE_btcmempool > intervSCHLEIFE_btcmempool) {
          startSCHLEIFE_btcmempool = millis();   // aktuelle Zeit abspeichern
          // MEMpool abfragen
          btc_mempool();
        }
*/

}