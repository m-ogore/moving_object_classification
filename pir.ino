#include <SPI.h> //Library for SPI communication (Pre-Loaded into Arduino)
#include <SD.h> //Library for SD card (Pre-Loaded into Arduino)


float pirvalue;
float pirPin =A0;
const int chipSelect = 10; //SD card CS pin connected to pin 4 of Arduino
void setup()
{
  // Setup Serial connection
  Serial.begin(9600);
  pinMode(pirPin, INPUT);

  Initialize_SDcard();
}
void loop()
{
  Write_SDcard();
  reading_pir();
  delay(5000);     //Wait for 5 seconds before writing the next data 
}

void reading_pir() {
  // Reading pir values takes 
  
pirvalue = analogRead(pirPin);
 
  
  // Check if any reads failed and exit early (to try again).
  if  (isnan(pirvalue) ) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  //debugging purposes
  Serial.print("pir value: "); 
  Serial.println(pirvalue);
  Serial.println(" ");
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
   File dataFile = SD.open("PIRdata.txt", FILE_WRITE);
   // if the file is available, write to it:
   if (dataFile)
   {
    dataFile.println("pir value"); //Write the first row of the excel file
    dataFile.close();
   }
}
void Write_SDcard()
{
    // open the file. note that only one file can be open at a time,
   // so you have to close this one before opening another.
   File dataFile = SD.open("PIRdata.txt", FILE_WRITE);
   // if the file is available, write to it:
   if (dataFile)
   {
    dataFile.print(pirvalue); //Store date on SD card
    dataFile.print(","); //Move to next column using a ","
    
    dataFile.println(); //End of Row move to next row
    dataFile.close(); //Close the file
   }
  else
  Serial.println("SD card writing failed");
}
