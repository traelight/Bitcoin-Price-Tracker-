/***************************************************************************************
 * *******************TEKO Glattbrugg Mikrokontrollertechnik****************************
 * Titel: Bitcoin Preis Tracker
 * Beschreib: Durch den ESP8266 wird per WLAN eine Verbindung zum Internet aufgebaut.
 *            Der aktuelle Bitcoin Preis wird von der offen zugängliche Coingecko API
 *            aufgerufen und auf dem QAPASS LCD per integriertem I2C Buss ausgestrahlt.
 *            
 * Erforderliche Geräte: ESP8266 NODE MCU Ver 1.0,16x2 QAPASS I2C LCD,5V Speisung
 * Ersteller: Eren Karkin
 * REV 1.1 - Datum:26.1.2023 
*/
//-------------------------------------------------------------------------------------
//Rev1.0 erstellt 24.1.2023  MAINCODE
//Rev1.1 erstellt 26.1.2023 "LIVE" blink Text hinzugefügt mit blinkText Funktion
         
//--------------- Includes ---------------
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h> 

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27,16,2); // Initialisierung/ Erkennung welches Display 

//----------- Variabel deklaration -------
//int previousValue = 0;  // ALTE THIRD PARTY Variabeln
//int threshold = 1;
//#define Blink_interval  200
//unsigned long previousMillis = 0;
int lastKnown = 0;
bool blinkOnOff = false;
int blinkTextTimer = 0;  

// Wlan Einstellungen
const char* ssid     = "UPC7216819"; //Z-TEKO 
const char* password = "gwWhrc8T5sfd"; //teko2015

// API server von COINDESK
const char* host = "api.coindesk.com"; // URL:https://api.coindesk.com/v1/bpi/currentprice.json

//-----------------------------Programm beginn---------------------------------
void setup() {

  lcd.init();          // LCD initialisierung
  lcd.backlight();     // Hintergrundsbeleuchtung einschalten 

  // SerialMonitor
  Serial.begin(115200);
  delay(10);
  // Wlan Verbindung auf Serial Monitor
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  //Verbindungsabfrage
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void blinkText(char *msgtxt, int col, int row) {  // BlinkFunktion mit Text
   // ALTES THIRD PARTY CODE   
   // unsigned long currentMillis = millis(); 
   // if(currentMillis - previousMillis > Blink_interval) {}
   // previousMillis = currentMillis;
   lcd.setCursor(col, row);
   if(blinkOnOff){
     for(blinkTextTimer = 1; blinkTextTimer < strlen(msgtxt); blinkTextTimer++) { lcd.print(" ");} //Alle Zeichen der Nachricht wird gelöscht
    } 
    else {lcd.print(msgtxt);} 
    blinkOnOff = !blinkOnOff;       //Boolsche Wert wird gekehrt (False-->True)
    } 
 
//---------------------------Endlosschleife----------------------------     
void loop() {
    
  // Verbindung zum Coindesk API
  Serial.print("connecting to ");
  Serial.println(host);
  
  // WiFiClient Bibliothek baut TCP Verbindung auf
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }                                           
  
  // Anfrage URL Link wird generiert
  String url = "/v1/bpi/currentprice.json";
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // Anfrage mit URL Link wird gesendet an Server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(100);
  
  // Antwort von Server an Serial Monitor schreiben
  String answer;
  while(client.available()){
    String line = client.readStringUntil('\r');
    answer += line;
  }

   client.stop();
   Serial.println();
   Serial.println("closing connection");
   Serial.println();
   Serial.println("Answer: ");
   Serial.println(answer);

  // Antwort auf JSON konventieren
  String jsonAnswer;
  int jsonIndex;

  for (int i = 0; i < answer.length(); i++) {
    if (answer[i] == '{') {
      jsonIndex = i;
      break;
    }
  }

  // Schreibe JSON data auf Serial Monitor
  jsonAnswer = answer.substring(jsonIndex);
  Serial.println();
  Serial.println("JSON answer: ");
  Serial.println(jsonAnswer);
  jsonAnswer.trim(); // Funktion um Leerzeichen abschneiden

  // Preis ohne Kommastelle (Get rate as float) wird gewünschte Währung gewählt von der JSON-Antwort 
  int rateIndex = jsonAnswer.indexOf("rate_float"); 
  String priceString = jsonAnswer.substring(rateIndex + 12, rateIndex + 18); // Index der JSON Antwort (bpi)
  priceString.trim();
  int price = priceString.toFloat();

  // Schreibe Preis auf Serial Monitor
  Serial.println();
  Serial.println("Bitcoin price: ");
  Serial.println(price);

  lcd.clear();
  blinkText("LIVE", 0, 1);
  lcd.setCursor(0, 0);
  lcd.print("Bitcoin Preis:");    //Titelanzeige
  lcd.setCursor(13,1);
  lcd.print("E.K");               //Signaturanzeige
  lcd.setCursor(5,1);
  lcd.print("$");                 //Währungsanzeige
  
  if (price >= 1) {               //Preis wird angezeigt,letzter Preis wird angezeigt bei Verbindungsunterbruch                              
    lcd.print(price);               
    lastKnown = price;}
   else{
     lcd.print(lastKnown);}     
   
// ALTES THIRD PARTY CODE     
// if (price > 5000) {lastKnown = price;}       // Wenn Preis höcher als 5000 ist,speichert letzter Wert
// if (price == 0) {lcd.print(lastKnown);}      // Falls keine Internetverbindung wird letzter Preis angezeigt
// else lcd.print(lastKnown);
 
}     
