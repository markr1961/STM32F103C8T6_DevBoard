#include "stm32f1xx_hal.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_gpio.h"

#include "stm32_tm1637.h"

#include <stdbool.h>

extern void uSecDelay(unsigned int i);

static void _tm1637Start(void);
static void _tm1637Stop(void);
static bool _tm1637WriteByte(uint8_t byte);
static void _tm1637ClkHigh(void);
static void _tm1637ClkLow(void);
static void _tm1637DioHigh(void);
static void _tm1637DioLow(void);
static bool _tm1637DioRead(void);

// Configuration.

#define CLK_PORT GPIOB
#define DIO_PORT GPIOB
#define CLK_PIN GPIO_PIN_6
#define DIO_PIN GPIO_PIN_7
#define CLK_PORT_CLK_ENABLE   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA)
#define DIO_PORT_CLK_ENABLE   LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOA)


uint8_t brightness = 7;

const char segmentMap[] =
{
  0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, // 0-7
  0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, // 8-9, A-F
  0x00
};

void tm1637Init(void)
{
  CLK_PORT_CLK_ENABLE;
  DIO_PORT_CLK_ENABLE;
  _tm1637ClkHigh();
  _tm1637DioHigh();
  GPIO_InitTypeDef g = {0};
  g.Pull = GPIO_PULLUP;         // F103 doesn't support pull up on outputs.
//    g.Mode = GPIO_MODE_OUTPUT_OD; // OD = open drain
  g.Mode = GPIO_MODE_OUTPUT_PP; // PP = push=pull
  g.Speed = GPIO_SPEED_FREQ_HIGH;
  g.Pin = CLK_PIN;
  HAL_GPIO_Init(CLK_PORT, &g);
  g.Pin = DIO_PIN;
  HAL_GPIO_Init(DIO_PORT, &g);

  tm1637SetBrightness(brightness);
}

void tm1637DisplayDecimal(int v, int displaySeparator)
{
  unsigned char digitArr[4];
  for (int i = 0; i < 4; ++i)
  {
    digitArr[i] = segmentMap[v % 10];
    if (i == 2 && displaySeparator)
    {
      digitArr[i] |= 1 << 7;
    }
    v /= 10;
  }

  _tm1637Start();
  _tm1637WriteByte(0x40);
  _tm1637Stop();

  _tm1637Start();
  _tm1637WriteByte(0xc0);

  for (int i = 0; i < 4; ++i)
  {
    _tm1637WriteByte(digitArr[3 - i]);
  }

  _tm1637Stop();
}

// Valid brightness values: 0 - 8.
// 0 = display off.
void tm1637SetBrightness(uint8_t brightness)
{
  // Brightness command:
  // 1000 0XXX = display off
  // 1000 1BBB = display on, brightness 0-7
  // X = don't care
  // B = brightness
  _tm1637Start();
  _tm1637WriteByte(0x88 + brightness);
  _tm1637Stop();
}

static void _tm1637Start(void)
{
  _tm1637ClkHigh();
  _tm1637DioHigh();
  uSecDelay(5);
  _tm1637DioLow();
  uSecDelay(1);
  _tm1637ClkLow();
}

static void _tm1637Stop(void)
{
  _tm1637ClkHigh();
  uSecDelay(1);
  _tm1637DioHigh();
  uSecDelay(1);
}

static bool _tm1637WriteByte(uint8_t b)
{
  bool ack_result;
  // yes, the TM1637 bytes are really LSB first!
  // for (uint8_t i = 8; i != 0; --i) { // for MSB 1st
  for (int i = 0; i < 8; ++i)
  {
    _tm1637ClkLow();
    uSecDelay(1);
    //  if (b & (1<<(i-1))) { // for MSB 1st
    if (b & 0x01)
    {
      _tm1637DioHigh();
    }
    else
    {
      _tm1637DioLow();
    }
    b >>= 1;  // comment out for MSB first
    uSecDelay(2);
    _tm1637ClkHigh();
    uSecDelay(3);
  }
  // finish with clock low:
  _tm1637ClkLow();
  uSecDelay(2);
  _tm1637DioHigh(); // let the slave drive the data pin:
  LL_GPIO_SetPinMode(DIO_PORT, DIO_PIN, LL_GPIO_MODE_FLOATING);
  _tm1637ClkHigh();
  uSecDelay(3);
  // while (dio); // We're cheating here and not actually reading back the response.
  ack_result = _tm1637DioRead();
  _tm1637ClkLow();
  uSecDelay(2);
  LL_GPIO_SetPinMode(DIO_PORT, DIO_PIN, LL_GPIO_MODE_OUTPUT);
  _tm1637DioLow();
  return (ack_result);
}

static bool _tm1637DioRead(void)
{
  return (bool)HAL_GPIO_ReadPin(DIO_PORT, DIO_PIN);
}

static void _tm1637ClkHigh(void)
{
  HAL_GPIO_WritePin(CLK_PORT, CLK_PIN, GPIO_PIN_SET);
}

static void _tm1637ClkLow(void)
{
  HAL_GPIO_WritePin(CLK_PORT, CLK_PIN, GPIO_PIN_RESET);
}

static void _tm1637DioHigh(void)
{
  HAL_GPIO_WritePin(DIO_PORT, DIO_PIN, GPIO_PIN_SET);
}

static void _tm1637DioLow(void)
{
  HAL_GPIO_WritePin(DIO_PORT, DIO_PIN, GPIO_PIN_RESET);
}
