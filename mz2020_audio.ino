#include <Adafruit_NeoPixel.h>
#include <PDM.h>

// Make Zurich 2020 badge LED configuration
#define LED_PIN 4
#define LED_COUNT 11

// LED ring color config
// Set colour percentage in parts of one
// e.g. 4 = 1/4, 1 = 1/1 etc

// Pink <3
#define LED_R 1
#define LED_G 6
#define LED_B 1.3

//// Fresh green
//#define LED_R 4
//#define LED_G 1
//#define LED_B 2

//// Plain white
//#define LED_R 1
//#define LED_G 1
//#define LED_B 1

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
  strip.setBrightness(23);
}


void loop() {
  // Wait for samples to be read
  if (samplesRead) {
    
    // Iterate over all samples in the buffer
    for (int i = 0; i < samplesRead; i++) {
      int volume = abs(sampleBuffer[i]);

      // Cancel background noise
      if (volume < 32) {
        volume = 0;
      }

      // Set all pixels accordingly
      for(int i=0; i<LED_COUNT; i++) { 
        strip.setPixelColor(i, strip.Color((volume/LED_R), volume/LED_G, (volume/LED_B)));
      }
      
      // Send the updated pixel colors to the LED strip
      strip.show(); 
    }
    
    // Clear audio sample buffer read count
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
