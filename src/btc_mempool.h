/////////////////////////////////////////////////////////////////////////// Btc MEMpool
void btc_mempool(){

WiFiClient client;
HTTPClient http;

// Send request
http.begin("https://mempool.space/api/blocks/tip/height");
int httpCode = http.GET();

  if (httpCode > 0) { //Check for the returning code
  
    String payload = http.getString();

          const size_t capacity = 2 * JSON_OBJECT_SIZE(1) + 10;
          DynamicJsonDocument doc(capacity);

          String json = payload;
          String mempool_height = json;
         
          // Mem Wert ausgeben
          Serial.print("MEMpool Block ");
          Serial.println(mempool_height);

          // TFT Ausgabe
          tft.fillRect(logo_tft_btc_block_x, logo_tft_btc_block_y, 48, 48, TFT_BLACK);
          tft.drawBitmap(logo_tft_btc_block_x, logo_tft_btc_block_y, blocklogo, 48, 48, TFT_BLUE);
          tft.setCursor(text_tft_btc_block_x, text_tft_btc_block_y);
          tft.setTextColor(TFT_ORANGE,TFT_BLACK);
          tft.setTextSize(2);
          tft.print(mempool_height);      

  } 
  
  else {
    Serial.println("Error on HTTP request");
  }
	
http.end(); //Free the resources


}