#include <Adafruit_NeoPixel.h>
#include <PDM.h>

// Make Zurich 2020 badge LED configuration
#define LED_PIN 4
#define LED_COUNT 11

Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Buffer to read samples into, each sample is 16-bits
short sampleBuffer[256];

// Number of samples read
volatile int samplesRead;

void setup() {
  // Initialize serial monitor for debugging
  Serial.begin(9600);
  
  // Configure the data receive callback
  PDM.onReceive(onPDMdata);

  // Set the gain, defaults to 20
  PDM.setGain(30);

  // Initialize PDM with:
  // - one channel (mono mode)
  // - a 16 kHz sample rate
  if (!PDM.begin(1, 16000)) {
    Serial.println("Failed to start PDM!");
    while (1);
  }
  
  // Inizialize Neopixel LED strip
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  strip.setBrightness(7);
}


void loop() {
  // Wait for samples to be read
  if (samplesRead) {
    
    // Iterate over all samples in the buffer
    for (int i = 0; i < samplesRead; i++) {
      
//      int volume = abs(sampleBuffer[i]);
      // Poor man's lowpass filter: Take the average of the current and next 4 samples?
      int volume = (abs(sampleBuffer[i])+abs(sampleBuffer[i+1])+abs(sampleBuffer[i+2])+abs(sampleBuffer[i+3])+abs(sampleBuffer[i+4]))/5;
      Serial.print("Sample: ");
      Serial.println(sampleBuffer[i]);

      // Cancel background noise?
      if (volume < 32) {
        volume = 0;
      }
      Serial.print("Volume: ");
      Serial.println(volume);
      
      // Map brightness level to volume
      int brightness = map(volume,0,255,0,255);
      Serial.print("Brightness: ");
      Serial.println(brightness);
      
      // Set all pixels accordingly
      for(int i=0; i<LED_COUNT; i++) { 
        strip.setPixelColor(i, strip.Color((brightness/4), brightness, (brightness/2)));
      }
      // Send the updated pixel colors to the hardware.
      strip.show(); 
    }
    
    // Clear sample buffer read count
    samplesRead = 0;
  }
}

void onPDMdata() {
  // Query the number of bytes available
  int bytesAvailable = PDM.available();

  // Read into the sample buffer
  PDM.read(sampleBuffer, bytesAvailable);

  // 16-bit, 2 bytes per sample
  samplesRead = bytesAvailable / 2;
}
