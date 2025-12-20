// ----------------------------------------------------------------------------
// My ESP SHT31 module 
// functions for temperature / humidity modle with SHT31 chip
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
void MySHT31_PrintValues()                  // print global data
{
  if (SHT31_ENABLED)
  {
    #ifndef SilentMode
      MySHT31_ReadTValues();
      SP("SHT31 Temperature : "); SPLF(MySHT31_Temperature); 
      SP("SHT31 Humidity    : "); SPLF(MySHT31_Humidity);
    #endif  
  }
}

// ----------------------------------------------------------------------------
void MySHT31_ReadTValues()                  // copy sensor data to global data
{
  if (SHT31_ENABLED)
  { 
    MySHT31_Temperature = sht31.readTemperature();
    MySHT31_Humidity    = sht31.readHumidity();
  }
}

// ----------------------------------------------------------------------------
void MySHT31_Init()                         // Init device hardware
{ 
  if (SHT31_ENABLED) 
  {
    #ifndef SilentMode
      SPLF(" ");
      SP("Init SHT31 temperature and humidity module: ");
    #endif  
    if (!sht31.begin(0x44))
    {
      SPLF("failed, SHT31 not responding");  
    }
    else
    {
      SPLF("success");  
    }
    MySHT31_PrintValues();                  // final: echo temp. and humidity
  }
}

// ----------------------------------------------------------------------------
