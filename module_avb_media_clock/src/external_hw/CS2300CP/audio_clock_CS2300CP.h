
#ifndef _audio_clock_CS2300CP_h_
#define _audio_clock_CS2300CP_h_
#include "i2c.h"

void audio_clock_CS2300CP_init(struct r_i2c &r_i2c, unsigned mclks_per_wordclk);

#endif
