#pragma once

void tm1637Init(void);
void tm1637DisplayDecimal(int v, int displaySeparator);
void tm1637SetBrightness(uint8_t brightness);

extern uint8_t brightness;