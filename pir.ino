//#include <a80_movingobject_classes_inferencing.h>
#include <a1sec166cnn_inferencing.h>
#include <SPI.h> //Library for SPI communication (Pre-Loaded into Arduino)
#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#define EIDSP_QUANTIZE_FILTERBANK   0

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiWire oled;

#define pirPin A0
//const int chipSelect = 10; //SD card CS pin connected to pin 4 of Arduino

#define THRESHOLD 0.7

/////////////////////////////////////////////////////////////////
#define FREQUENCY_HZ        166
#define INTERVAL_MS         (100000 / (FREQUENCY_HZ + 1))
/////////////////////////////////////////////////////////////////


static unsigned long last_interval_ms = 0;
// to classify 1 frame of data you need EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE values
float features[EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE];
// keep track of where we are in the feature array
size_t feature_ix = 0;

float pirvalue;


void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Wire.setClock(400000L);
  
  Serial.begin(9600);
  pinMode(pirPin, INPUT);
#if RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS, RST_PIN);
#else   // RST_PIN >= 0
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
#endif  // RST_PIN >= 0
oled.setFont(Adafruit5x7);

  uint32_t m = micros();
  oled.clear();
  oled.set1X();
  oled.print("waiting");
  delay(500);


}


void loop() {
    static unsigned long last_interval_ms = 0;

    if (millis() > last_interval_ms + INTERVAL_MS) {
        last_interval_ms = millis();
       
    
  pirvalue = analogRead(pirPin);
 
  
  // Check if any reads failed and exit early (to try again).
  if  (isnan(pirvalue) ) {
    Serial.print("Failed to read from sensor!");
     oled.clear();
  oled.set1X();
  oled.println("Failed to read from sensor!");
    return;
  }
  
  //debugging purposes
  
  Serial.print("pir value: "); 
 Serial.println(pirvalue);
  Serial.println(" ");

      //  Serial.println(EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE); 
 


        // keep filling the features array until it's full
        features[feature_ix++] = pirvalue;
   // ei_printf("Edge Impulse standalone inferencing (Arduino)\n");


        // features buffer full? then classify!
        if (feature_ix == EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE) {
            ei_impulse_result_t result;

            // create signal from features frame
            signal_t signal;
            numpy::signal_from_buffer(features, EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE, &signal);

            // run classifier
            EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);
            ei_printf("run_classifier returned: %d\n", res);
            if (res != 0) return;

            // print predictions
            ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
                result.timing.dsp, result.timing.classification, result.timing.anomaly);

            // print the predictions
            for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
                ei_printf("%s:\t%.5f\n", result.classification[ix].label, result.classification[ix].value);
                 oled.clear();
                 oled.set1X();
                 oled.setCursor(50,18+(10*ix));
                 oled.print(result.classification[ix].label);
                 oled.print(result.classification[ix].value);
               
                 
           /*    if (result.classification[ix].label == "human" && result.classification[ix].value > THRESHOLD)
        { 
             digitalWrite(BLUE, HIGH); 
            
        }
        else 
        {
          
           digitalWrite(GREEN, HIGH);
        }*/
            }
        #if EI_CLASSIFIER_HAS_ANOMALY == 1
            ei_printf("anomaly:\t%.3f\n", result.anomaly);
        //    lcd.print(result.anomaly);
        //oled.clear();
  //oled.set1X();
  //oled.print(result.anomaly);

        #endif

            // reset features frame
            feature_ix = 0;
        }
    }
}
void ei_printf(const char *format, ...) {
    static char print_buf[1024] = { 0 };

    va_list args;
    va_start(args, format);
    int r = vsnprintf(print_buf, sizeof(print_buf), format, args);
    va_end(args);

    if (r > 0) {
        Serial.write(print_buf);
    }
}
