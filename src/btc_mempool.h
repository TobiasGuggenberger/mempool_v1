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
          tft.setCursor(text_tft_btc_block_x, text_tft_btc_block_y);
          tft.setTextColor(TFT_ORANGE,TFT_BLACK);
          tft.setTextSize(2);
          tft.print(mempool_height);    

        tft.drawBitmap(30, 5, blocklogo, 42, 42, TFT_ORANGE);
        tft.drawBitmap(74, 5, blocklogo, 42, 42, TFT_ORANGE);
        tft.drawBitmap(118, 5, blocklogo, 42, 42, TFT_ORANGE);          

  } 
  
  else {
    Serial.println("Error on HTTP request");
  }
	
http.end(); //Free the resources


}