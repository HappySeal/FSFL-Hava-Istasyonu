#include <HCRTC.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <SFE_BMP180.h>
#include <Wire.h>
#include <SD.h>

#define ALTITUDE 100
#define DHTPIN 3
#define DHTTYPE DHT22
#define I2CDS1307Add 0x68
#define SD_CS_DIO 10 

HCRTC HCRTC;
File DataFile;
DHT dht(DHTPIN,DHTTYPE);
LiquidCrystal_I2C lcd(0x27,16,2);
SFE_BMP180 bmp180;

byte degree = 0xDF;

void setup() {
  Serial.begin(9600);
  //HCRTC.RTCWrite(I2CDS1307Add, 17, 2, 28, 20, 00, 0, 2);
  
  if (!SD.begin(SD_CS_DIO)) 
  {
    /* If there was an error output this to the serial port and go no further */
    Serial.println("ERROR: SD card failed to initialise");
    while(1);
  }else
  {
    Serial.println("SD Card OK");
  }

  dht.begin();
  bmp180.begin();
  lcd.begin();
  
}

void loop() {
  int nem = dht.readHumidity();
  float sicaklik = dht.readTemperature();
  float basinc = readPressure();
  HCRTC.RTCRead(I2CDS1307Add);
  Serial.print(HCRTC.GetDateString());
  Serial.print(", ");
  Serial.print(HCRTC.GetTimeString());
  Serial.print("/");

  lcd.clear();
  lcd.setCursor(10,0);
  lcd.print("N: ");
  lcd.setCursor(13,0);
  lcd.print(nem);
  lcd.setCursor(15,0);
  lcd.print("%");

  lcd.setCursor(0,1);
  lcd.print("B: ");
  lcd.setCursor(3,1);
  lcd.print(basinc);
  lcd.setCursor(11,1);
  lcd.print("mB");

  lcd.setCursor(0,0);
  lcd.print("S: ");
  lcd.setCursor(3,0);
  lcd.print(sicaklik);
  lcd.setCursor(7,0);
  lcd.print((char)223);
  lcd.setCursor(8,0);
  lcd.print("C");

  Serial.print(int(basinc));
  Serial.print("/");
  Serial.print(nem);
  Serial.print("/");
  Serial.println(int(sicaklik));

  
  if((int(HCRTC.GetHour()) == 18 and int(HCRTC.GetMinute()) == 00)or(int(HCRTC.GetHour()) == 6 and int(HCRTC.GetMinute()) == 00)or(int(HCRTC.GetHour()) == 12 and int(HCRTC.GetMinute()) == 00)or(int(HCRTC.GetHour()) == 00 and int(HCRTC.GetMinute()) == 00))
    {
      Serial.print("Working MOAR fine");
      String FileName = String(HCRTC.GetDay()) + "_" + String(HCRTC.GetMonth()) + "_" + String(HCRTC.GetYear())+".txt";
      DataFile = SD.open(FileName,FILE_WRITE);
      DataFile.print(HCRTC.GetDateString());
      DataFile.print("   ");
      DataFile.print(HCRTC.GetTimeString());
      DataFile.print(" SICAKLIK: ");
      DataFile.print(sicaklik);
      DataFile.print(" NEM: ");
      DataFile.print(nem);
      DataFile.print(" BASINC: ");
      DataFile.println(basinc);
      DataFile.close();
    }
    

  delay(1000);


}
float readPressure()
{
  char status;
  double T,P,p0,a;

  status = bmp180.startTemperature();
  if (status != 0)
  {
    delay(status);
    status = bmp180.getTemperature(T);
    if (status != 0)
    { 
      status = bmp180.startPressure(3);
      if (status != 0)
      {
        delay(status);
        status = bmp180.getPressure(P,T);
        if (status != 0)
        {
          p0 = bmp180.sealevel(P,ALTITUDE);       
          return p0;
        }
      }
    }
  }
}

