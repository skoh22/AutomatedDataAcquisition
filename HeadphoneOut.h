
#include "Arduino.h"
#include "AudioStream.h"


class TimHeadphoneOut : public AudioStream
{
  public:
    uint32_t   headphoneBlockCount = 0;
    double phaseIncrement = 0.01;
    bool invertSecondChannel = true;

    TimHeadphoneOut() : AudioStream(0, NULL) {}
    virtual void update(void);
    void TimHeadphoneOut::SetMagnitude(double m) {
      __disable_irq();
      philMag = m;
      __enable_irq();
    }
    void TimHeadphoneOut::SetDCoffset(int i) {
      __disable_irq();
      DCoffset = i;
      __enable_irq();
    }
    void TimHeadphoneOut::SetInversion(bool v) {
      __disable_irq();
      invertSecondChannel = v;
      __enable_irq();
    }
    void TimHeadphoneOut::SetFrequency(double f) {
      __disable_irq();
      phaseIncrement = f * PI * 2 / 44100.0;
      __enable_irq();
    }
  private:
    double philMag = 1000.0;
    double phaseVal = 0.0;
    int DCoffset = 0;
};
