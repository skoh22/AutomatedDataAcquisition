
#include <Arduino.h>
#include "PeakCapture.h"

void PeakCapture::update(void)
{
  audio_block_t *block;
  const int16_t *p, *end;
  int32_t min, max;

  block = receiveReadOnly();
  if (!block) {
    return;
  }
  p = block->data;
  end = p + AUDIO_BLOCK_SAMPLES;
  min = min_sample;
  max = max_sample;
  do {
    int16_t d=*p++;
    // TODO: can we speed this up with SSUB16 and SEL
    // http://www.m4-unleashed.com/parallel-comparison/
    if (d<min) min=d;
    if (d>max) max=d;
   double pwr = ((double) d ) * ((double) d);
   sum += pwr;
   count ++;
   lastVal = d;
  } while (p < end);
  min_sample = min;
  max_sample = max;
  new_output = true;
  release(block);
}
