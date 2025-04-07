#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define ledPin 15
#define ledNum 8
Adafruit_NeoPixel pixels(ledNum, ledPin, NEO_GRB + NEO_KHZ800);

const int pulseA = 23;
const int pulseB = 22;
volatile int lastEncoded = 0;
volatile long encoderValue = 0;

IRAM_ATTR void handleRotary() {
    int MSB = digitalRead(pulseA); 
    int LSB = digitalRead(pulseB); 
    int encoded = (MSB << 1) | LSB; 
    int sum = (lastEncoded << 2) | encoded; 
    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue++;
    if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue--;
    lastEncoded = encoded; 

    if (encoderValue > 255) {
        encoderValue = 255;
    } else if (encoderValue < 0) {
        encoderValue = 0;
    }
}

void waterDrop(int i, int R, int G, int B, int maxTail = 4, int tail = 1) {
    int divider = exp(tail);
    pixels.setPixelColor(i, pixels.Color(R / divider, G / divider, B / divider));
    if (i < ledNum && tail < maxTail) {
        waterDrop(++i, R, G, B, maxTail, ++tail);
    }
}

void setup() {
    pixels.begin();

    pinMode(pulseA, INPUT_PULLUP);
    pinMode(pulseB, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pulseA), handleRotary, CHANGE);
    attachInterrupt(digitalPinToInterrupt(pulseB), handleRotary, CHANGE);

    Serial.begin(115200);
    Serial.println("Setup complete. Start rotating the encoder...");
}

void loop() {
   
    int delayTime = map(encoderValue, 0, 255, 50, 1000);


    static long lastPrintTime = 0;
    if (millis() - lastPrintTime > 500) { 
        Serial.print("Encoder Value: ");
        Serial.print(encoderValue);
        Serial.print(" | Delay Time: ");
        Serial.println(delayTime);
        lastPrintTime = millis();
    }


    int R = 0;
    int G = 0;
    int B = 255;

   
    for (int i = 7; i >= 0; i--) {
        pixels.clear();
        waterDrop(i, R, G, B); 
        pixels.show();
        delay(delayTime); 
    }
}