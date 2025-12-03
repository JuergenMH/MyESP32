// ----------------------------------------------------------------------------
// My ESP OLED module 
// functions for OLED connected to SPI
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void MyOLED_Init()
{
  if (display.begin(SSD1306_SWITCHCAPVCC, OLED_ADR))
  {
    #ifndef SilentMode
      SPLF("Display init complete");
    #endif  
    display.clearDisplay();
    display.setRotation(OLED_OR);
    display.setTextColor(SSD1306_WHITE);
    display.display();
    // Akutell auskommentiert; funktioniert nicht wirklich
    //display.ssd1306_command(0x81);
    //display.ssd1306_command(50);     
  }  
  else
  {
    #ifndef SilentMode
      SPLF("Display init failed");
    #endif  
  }
}

// ----------------------------------------------------------------------------
