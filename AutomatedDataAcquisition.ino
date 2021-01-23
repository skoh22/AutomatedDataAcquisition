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
  Serial.println("Automated Test");
  pinMode(17, OUTPUT);
  pinMode(16, OUTPUT);
  pinMode(15, OUTPUT);
  analogRead(0);
  //analogRead(17);
  pinMode(41, OUTPUT);

}

char c;
bool passThruMode = false;
void loop() {
  if (passThruMode) CheckForResponseFromBlueTooth() ;

  if (Serial.available()) {
    c = Serial.readLine();
    if (c == 'x') passThruMode = false;
    if (passThruMode) {
      if (c == '\n') {
        SERIAL_FOR_BLUETOOTH_MODULE.println();
      } else {
        SERIAL_FOR_BLUETOOTH_MODULE.print(c);
      }
      return;
    }

    if (c == 'a') {
      headphoneOut1.SetMagnitude(32000);
      headphoneOut1.SetFrequency(440);
      headphoneOut1.SetInversion(true);
      headphoneOut1.SetDCoffset(0);

      wm8731.volume(.94);  // 3.4V peak-to-peak is clipping limit of headphone driver
      // 32000 mag and 0.94 volume gives 3.20V peak-peak output;
    }
    if (c == 'A') {
      headphoneOut1.SetMagnitude(5000);
      headphoneOut1.SetFrequency(440);
      headphoneOut1.SetInversion(false);
      headphoneOut1.SetDCoffset(-10000);

      wm8731.volume(.94);  // 3.4V peak-to-peak is clipping limit of headphone driver
      // 32000 mag and 0.94 volume gives 3.20V peak-peak output;
    }

    if (c == 'b') {
      wm8731.volume(0);
    }

    if (c == 'B') {
      int DCOffsetValue1 = 0;
      headphoneOut1.SetMagnitude(0);
      headphoneOut1.SetFrequency(440);
      headphoneOut1.SetInversion(false);
      headphoneOut1.SetDCoffset(DCOffsetValue1);
      float outputVolts = 0.000049312088*DCOffsetValue1 + 1.690461538;
      Serial.printf("Output Volts: %f V", outputVolts);
      Serial.println();

      wm8731.volume(.94);  // 3.4V peak-to-peak is clipping limit of headphone driver
      // 32000 mag and 0.94 volume gives 3.20V peak-peak output;
    }
    if (c == 'c') {
      Serial.printf( "%f,%f,%f,%f\n", peak0.read(), peak0.readRMS(), peak1.read(), peak1.readRMS());
    }
    if (c == 'C') {
      float firstPeak = peak1.readLast();
      Serial.printf( "%f,%f\n", peak0.readLast(), firstPeak);
      float firstPeakVolts = .00004387953486 * firstPeak + 1.718627245;
      Serial.printf("Input Volts: %f V", firstPeakVolts);
      Serial.println();
    }



    if (c == 'd') {
      PrepareBLE();
      SERIAL_FOR_BLUETOOTH_MODULE.println("AT+DEFAULT");
      WaitWhileCheckingForMessages(100);
      PrepareBLE();
    }
    
    if (c == 'D') {
      //set DC output
      int DCOffsetValue2 = -15000;
      headphoneOut1.SetMagnitude(0);
      headphoneOut1.SetFrequency(440);
      headphoneOut1.SetInversion(false);
      headphoneOut1.SetDCoffset(DCOffsetValue2);
      //calc output in volts
      float outputVolts = 0.000049312088*DCOffsetValue2 + 1.690461538;
      Serial.printf("Output Voltage: %f V", outputVolts);
      Serial.println();
      //read input
      float drainRead = peak1.readLast();
      float drainVolts =  .00004387953486 * drainRead + 1.718627245;
      Serial.printf("Drain Voltage: %f V", drainVolts);
      Serial.println();
      //calc current
      float drainCurrent = 1000*(outputVolts - drainVolts)/10;
      Serial.printf("Drain Current: %f mA", drainCurrent);
      Serial.println();
      
      wm8731.volume(.94);  // 3.4V peak-to-peak is clipping limit of headphone driver
      // 32000 mag and 0.94 volume gives 3.20V peak-peak output;
    }

    if (c == 'e') {
      SERIAL_FOR_BLUETOOTH_MODULE.println("AT+ADDR");
      WaitWhileCheckingForMessages(100);
    }

    if (c == 'f') {
      SERIAL_FOR_BLUETOOTH_MODULE.println("AT+ROLE1");
      WaitWhileCheckingForMessages(1000);
      SERIAL_FOR_BLUETOOTH_MODULE.println("AT+INQ");
      WaitWhileCheckingForMessages(10000);
    }


    if (c == 'g') {
      passThruMode = true;
    }

    if (c == 'h') {
      headphoneOut1.SetMagnitude(20000);
      headphoneOut1.SetFrequency(440);
      headphoneOut1.SetInversion(false);
      headphoneOut1.SetDCoffset(0);


      wm8731.volume(.94);  // 3.4V peak-to-peak is clipping limit of headphone driver
      // 30000 mag and 0.94 volume gives 3.00V peak-peak output;
    }

    if (c == 'i') {
      headphoneOut1.SetMagnitude(20000);
      headphoneOut1.SetFrequency(440);
      headphoneOut1.SetInversion(true);
      headphoneOut1.SetDCoffset(0);


      wm8731.volume(.94);  // 3.4V peak-to-peak is clipping limit of headphone driver
      // 30000 mag and 0.94 volume gives 3.00V peak-peak output;
    }
    if (c == 'j') {
      SERIAL_FOR_BLUETOOTH_MODULE.println("AT+ROLE1");
      WaitWhileCheckingForMessages(1000);
    }

    if (c == 'k') {
      uint32_t uuid = HW_OCOTP_MAC0;
      Serial.printf("%08X\n", uuid);
      uint32_t uuid1 = HW_OCOTP_MAC1;
      Serial.printf("%08X\n", uuid1);
    }

    if (c == 'l') { // serial number
      Serial.println("@l");
      uint32_t gooseNumber = GetNumberForGooseName();
      Serial.println(gooseNumber);
    }

    if (c == 'm') {  // mac address
      GetNumberForGooseName();
      Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X\n", macAddressFromGooseName[0], macAddressFromGooseName[1], macAddressFromGooseName[2], macAddressFromGooseName[3], macAddressFromGooseName[4], macAddressFromGooseName[5]);
    }
    if (c == 'n') {
      SERIAL_FOR_BLUETOOTH_MODULE.println("AT+LADDR");
      WaitWhileCheckingForMessages(100);
    }
    if (c == 'o') {
      SERIAL_FOR_BLUETOOTH_MODULE.println("AT+HELP");
      WaitWhileCheckingForMessages(2000);
    }
    if (c == 'p') {
      SERIAL_FOR_BLUETOOTH_MODULE.println("AT");
      WaitWhileCheckingForMessages(500);
    }
    if (c == 'q') {
      Serial.println("hello");
    }

    if (c == 'v') {  // get bluetooth module version
      SERIAL_FOR_BLUETOOTH_MODULE.println("AT+VERSION");
      WaitWhileCheckingForMessages(1000);
    }

    if (c == 'w') {

      Serial.println(analogRead(0));
    }


    if (c == 'x') digitalWrite(17, LOW);
    if (c == 'X') digitalWrite(17, HIGH);

    if (c == 'y') digitalWrite(16, LOW);
    if (c == 'Y') digitalWrite(16, HIGH);

    if (c == 'z') digitalWrite(15, LOW);
    if (c == 'Z') digitalWrite(15, HIGH);





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
