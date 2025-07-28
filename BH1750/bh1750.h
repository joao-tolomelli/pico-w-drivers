#ifndef BH1750_H
#define BH1750_H

#include "hardware/i2c.h"

#define BH1750_ADDR 0x23

void bh1750_init(i2c_inst_t *i2c);
float bh1750_read_lux(i2c_inst_t *i2c);

#endif
