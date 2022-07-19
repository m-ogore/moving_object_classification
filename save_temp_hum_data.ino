#include <SPI.h> //Library for SPI communication (Pre-Loaded into Arduino)
#include <SD.h> //Library for SD card (Pre-Loaded into Arduino)
#include "DHT.h"
#define DHTTYPE DHT11 // DHT 11
uint8_t DHTPin = 6; 
DHT dht(DHTPin, DHTTYPE); 
float Temperature;
float Humidity;
const int chipSelect = 10; //SD card CS pin connected to pin 4 of Arduino
void setup()
{
  // Setup Serial connection
  Serial.begin(9600);
  pinMode(DHTPin, INPUT);
  dht.begin();
  Initialize_SDcard();
}
void loop()
{
  Write_SDcard();
  Read_TempHum();
  delay(5000);     //Wait for 5 seconds before writing the next data 
}
void Read_TempHum()
{
  Temperature = dht.readTemperature(); 
  Humidity = dht.readHumidity(); 
  Serial.print("Temperature = ");
  Serial.println(Temperature);
  Serial.print("Humidity = ");
  Serial.println(Humidity);
  // delay(1000);
}
void Initialize_SDcard()
{
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect))
  {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
   }
   // open the file. note that only one file can be open at a time,
   // so you have to close this one before opening another.
   File dataFile = SD.open("LoggerCD.txt", FILE_WRITE);
   // if the file is available, write to it:
   if (dataFile)
   {
    dataFile.println("Temperature,Humidity"); //Write the first row of the excel file
    dataFile.close();
   }
}
void Write_SDcard()
{
    // open the file. note that only one file can be open at a time,
   // so you have to close this one before opening another.
   File dataFile = SD.open("LoggerCD.txt", FILE_WRITE);
   // if the file is available, write to it:
   if (dataFile)
   {
    dataFile.print(Temperature); //Store date on SD card
    dataFile.print(","); //Move to next column using a ","
    dataFile.print(Humidity); //Store date on SD card
    dataFile.print(","); //Move to next column using a ","
    dataFile.println(); //End of Row move to next row
    dataFile.close(); //Close the file
   }
  else
  Serial.println("SD card writing failed");
}
