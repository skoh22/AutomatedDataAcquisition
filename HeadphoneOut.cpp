#include <Arduino.h>
#include "HeadphoneOut.h"


void ArrayLimitCheck(int index, int arraySize, int whichOne) ;
void MemcpyLimitCheck(int bytesCopied, int thisBufferSize, int whichOne);



void TimHeadphoneOut::update(void)
{
  audio_block_t *block;
  audio_block_t *blockInverted;
  uint32_t i;
  //  int32_t val1, val2;

  block = allocate();
  blockInverted = allocate();
  if (block && blockInverted) {
    for (i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {
  //    int16_t val = 10 * (i - AUDIO_BLOCK_SAMPLES / 2);
      double dval = sin(phaseVal) * philMag;
      phaseVal += phaseIncrement;
      if (phaseVal > PI) phaseVal -= 2.0 * PI;

      if (dval > 32767) dval = 32767;
      if (dval < -32768) dval = -32768;
      int16_t val = dval + DCoffsetLeft;
      
      
      
      block->data[i] = val;
      if (val == -32768) val = -32767;
      if (invertSecondChannel) val = - val;
      blockInverted->data[i] =  DCoffsetRight;
  
    }
    transmit(block, 0);
    transmit(blockInverted, 1);
    release(block);
    release(blockInverted);
  }



}
