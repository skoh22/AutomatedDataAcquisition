#include "HeadphoneOut.h"
#include "PeakCapture.h"
#include "wm8731.h"
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>

#define SERIAL_FOR_BLUETOOTH_MODULE Serial1
#define WM8731_REG_DIGITAL  5

PeakCapture         peak0;
PeakCapture         peak1;


TimHeadphoneOut          headphoneOut1;      //xy=393,360
AudioOutputI2Sslave           i2s1;           //xy=597,356s
AudioConnection          patchCord1(headphoneOut1, 0, i2s1, 0);
AudioConnection          patchCord2(headphoneOut1, 1, i2s1, 1);

AudioInputI2Sslave            i2s2;           //xy=393,337
AudioConnection          patchCord3(i2s2, 0, peak0, 0);
AudioConnection          patchCord4(i2s2, 1, peak1, 0);

byte macAddressFromGooseName[6];


PhilAudioControlWM8731master wm8731;
void setup() {
  Serial.begin(9600);
  SERIAL_FOR_BLUETOOTH_MODULE.begin(9600);

  AudioInterrupts();
  AudioMemory(8);
  wm8731.enable();
  // wm8731.write(WM8731_REG_DIGITAL, 0x00);   // DAC unmuted
  wm8731.inputLevel(0.75);
  wm8731.inputSelect(AUDIO_INPUT_LINEIN);
  delay(50);
  Serial.println("Automated Test\n");
  pinMode(17, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(15, OUTPUT);
  analogRead(0);
  //analogRead(17);
  pinMode(41, OUTPUT);

}

String c;
void loop() {
  if (Serial.available()) {
    c = Serial.readString();
    //A command sets output voltage with Arduino int input, gives voltage in volts
    if (c.startsWith('A')) {
      //Serial.print("String: ");
      //Serial.println(c);
      c.remove(0, 2);
      //Serial.print("Removed String: ");
      //Serial.println(c);
      int DCoffset = c.toInt();
      if (DCoffset > -32768 && DCoffset < 32767) {
        float outputVolts = 0.000049312088 * DCoffset + 1.690461538;
        //Serial.printf("Output Voltage: %f V\n", outputVolts);
        //Serial.println();
        headphoneOut1.SetMagnitude(0);
        headphoneOut1.SetFrequency(440);
        headphoneOut1.SetInversion(false);
        headphoneOut1.SetDCoffsetLeft(DCoffset);
        Serial.printf("Set Left/Gate to %f V by setting to %d \n", outputVolts, DCoffset);
      }
      else {
        Serial.println("Value is outside range. Try again.\n");
      }
      wm8731.volume(.94);  // 3.4V peak-to-peak is clipping limit of headphone driver
      // 32000 mag and 0.94 volume gives 3.20V peak-peak output;
    }
    if (c.startsWith('B')) {
      c.remove(0, 2);
      int DCoffset = c.toInt();
      if (DCoffset > -32768 && DCoffset < 32767) {
        float outputVolts = 0.000049312088 * DCoffset + 1.690461538;
        //Serial.printf("Output Voltage: %f V\n", outputVolts);
        //Serial.println();
        headphoneOut1.SetMagnitude(0);
        headphoneOut1.SetFrequency(440);
        headphoneOut1.SetInversion(false);
        headphoneOut1.SetDCoffsetRight(DCoffset);
        Serial.printf("Set Right/Drain to %f V by setting to %d \n", outputVolts, DCoffset);
      }
      else {
        Serial.println("Value is outside range. Try again.\n");
      }
      wm8731.volume(.94);  // 3.4V peak-to-peak is clipping limit of headphone driver
      // 32000 mag and 0.94 volume gives 3.20V peak-peak output;
    }
    if (c.startsWith('c')) {
      Serial.printf( "Read vals: %f,%f\n", peak1.read(), peak1.readRMS());
    }
    if (c.startsWith('C')) {
      float firstPeak = peak1.readLast();
      Serial.printf( "%f,%f\n", peak0.readLast(), firstPeak);
      //float firstPeakVolts = .00004387953486 * firstPeak + 1.718627245;
      //Serial.printf("Input Volts: %f V", firstPeakVolts);
      //Serial.println();
    }

    if (c == 'D') {
      //set DC output
      int DCOffsetValue2 = -15000;
      headphoneOut1.SetMagnitude(0);
      headphoneOut1.SetFrequency(440);
      headphoneOut1.SetInversion(false);
      headphoneOut1.SetDCoffsetLeft(DCOffsetValue2);
      //calc output in volts
      float outputVolts = 0.000049312088 * DCOffsetValue2 + 1.690461538;
      Serial.printf("Output Voltage: %f V\n", outputVolts);
      //read input
      float drainRead = peak1.readLast();
      float drainVolts =  .00004387953486 * drainRead + 1.718627245;
      Serial.printf("Drain Voltage: %f V\n", drainVolts);
      //calc current
      float drainCurrent = 1000 * (outputVolts - drainVolts) / 10;
      Serial.printf("Drain Current: %f mA\n", drainCurrent);

      wm8731.volume(.94);  // 3.4V peak-to-peak is clipping limit of headphone driver
      // 32000 mag and 0.94 volume gives 3.20V peak-peak output;
    }

    if (c == 'h') {
      headphoneOut1.SetMagnitude(20000);
      headphoneOut1.SetFrequency(440);
      headphoneOut1.SetInversion(false);
      headphoneOut1.SetDCoffsetLeft(0);


      wm8731.volume(.94);  // 3.4V peak-to-peak is clipping limit of headphone driver
      // 30000 mag and 0.94 volume gives 3.00V peak-peak output;
    }

    if (c == 'i') {
      headphoneOut1.SetMagnitude(20000);
      headphoneOut1.SetFrequency(440);
      headphoneOut1.SetInversion(true);
      headphoneOut1.SetDCoffsetLeft(0);


      wm8731.volume(.94);  // 3.4V peak-to-peak is clipping limit of headphone driver
      // 30000 mag and 0.94 volume gives 3.00V peak-peak output;
    }
    if (c == 'q') {
      Serial.println("hello");
    }

    if (c == 'w') {

      Serial.println(analogRead(0));
    }
  }

}


void WaitWhileCheckingForMessages(int milliseconds) {
  uint32_t timeToStop = millis() + milliseconds;
  while (millis() < timeToStop) {
    CheckForResponseFromBlueTooth();
  }

}

void CheckForResponseFromBlueTooth() {
  if (SERIAL_FOR_BLUETOOTH_MODULE.available()) {
    c = SERIAL_FOR_BLUETOOTH_MODULE.read();
    Serial.print(c);
    delay(1);
  }
}

uint32_t timeForMessage2;
uint32_t timeForMessage3;
uint32_t timeToExitSub;
void PrepareBLE() {
  timeForMessage2 = millis() + 800; //400 ms is needed for this command to work, so set at double this just to be sure
  timeForMessage3 = millis() + 1000;
  timeToExitSub = millis() + 1100;

  Serial.println("sending message1");
  SERIAL_FOR_BLUETOOTH_MODULE.println("AT+BAUD8");

  while (true) {
    CheckForResponseFromBlueTooth();
    if (millis() > timeForMessage2) {
      timeForMessage2 = UINT32_MAX;
      Serial.println("changing teensy baud rate to 115200");
      SERIAL_FOR_BLUETOOTH_MODULE.flush();
      SERIAL_FOR_BLUETOOTH_MODULE.begin(115200);
      Serial.println("sending message2");
      uint32_t uuid = HW_OCOTP_MAC0;
      uint32_t trunc32 = uuid & 0x0000FFFF;
      uint16_t trunc16 = (uint16_t) trunc32;
      Serial.print("uuid lower 16 bits is ");
      Serial.println(trunc16);

      SERIAL_FOR_BLUETOOTH_MODULE.print("AT+NAMEGoose");
      SERIAL_FOR_BLUETOOTH_MODULE.println(trunc16);
      Serial.println("Must power cycle telejam for name change to take effect");
    }


    if (millis() > timeForMessage3) {
      timeForMessage3 = UINT32_MAX;
      SERIAL_FOR_BLUETOOTH_MODULE.println("AT+RESET");
    }

    if (millis() > timeToExitSub) break;

  }

  Serial.println("Exiting PrepareBLE");
}


uint32_t GetNumberForGooseName() {
  uint32_t uuid = HW_OCOTP_MAC0;
  uint32_t trunc32 = uuid & 0x0000FFFF;
  //     byte mac[] = {0x3C, 0x18, 0xA0, 0x99, 0x65, 0x21};
  macAddressFromGooseName[0] = 0x3C;
  macAddressFromGooseName[5] = 0x21;

  memcpy(macAddressFromGooseName + 1, & uuid, 4);
  return trunc32;

}
