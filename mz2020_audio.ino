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

// This routine runs once upon power on
void setup() {
  // Initialize serial monitor for debugging
  Serial.begin(9600);
  
  // Configure the data receive callback
  PDM.onReceive(onPDMdata);

  // Optionally set the gain, defaults to 20
  // PDM.setGain(30);

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
  strip.setBrightness(255);
}


void loop() {
  // Wait for samples to be read
  if (samplesRead) {

    // Print samples to the serial monitor or plotter
    for (int i = 1; i < samplesRead; i++) {
      Serial.println(sampleBuffer[i]);
      
      // Don't fully understand
      int brightness = abs(sampleBuffer[i]);
      for(int i=0; i<LED_COUNT; i++) { // For each pixel...
        strip.setPixelColor(i, strip.Color(brightness, brightness, brightness));
      }
      strip.show();   // Send the updated pixel colors to the hardware.
    }

    // Clear the read count
    samplesRead = 0;
  }
}

void onPDMdata() {
  // query the number of bytes available
  int bytesAvailable = PDM.available();

  // read into the sample buffer
  PDM.read(sampleBuffer, bytesAvailable);

  // 16-bit, 2 bytes per sample
  samplesRead = bytesAvailable / 2;
}
