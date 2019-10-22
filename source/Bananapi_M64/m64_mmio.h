// Copyright (c) 2014 Adafruit Industries
// Author: Tony DiCola
// Based on code from Gert van Loo & Dom: http://elinux.org/RPi_Low-level_peripherals#GPIO_Code_examples

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// Simple fast memory-mapped GPIO library for the Raspberry Pi.
#ifndef M64_MMIO_H
#define M64_MMIO_H

#include <stdint.h>

#define MMIO_SUCCESS 0
#define MMIO_ERROR_ARGUMENT -1
#define MMIO_ERROR_DEVMEM -1
#define MMIO_ERROR_MMAP -2

#define INPUT                   0
#define OUTPUT                  1

#define LOW						0
#define HIGH					1

#define SUNXI_GPIO_AH_BASE    0x01c20800
#define SUNXI_GPIO_LM_BASE    0x01f02c00
#define MAP_SIZE              (4096*2)
#define MAP_MASK              (MAP_SIZE - 1)

extern volatile uint32_t* m64_mmio_gpio_ah;
extern volatile uint32_t* m64_mmio_gpio_lm;

int m64_mmio_init_gpio(int pin);

static uint32_t sunxi_gpio_readl(uint32_t addr, int bank)
{
  uint32_t val = 0;
  uint32_t mmap_base = (addr & ~MAP_MASK);
  uint32_t mmap_seek = ((addr - mmap_base) >> 2);

  /* DK, for PL and PM */
  if(bank == 11)
      val = *(m64_mmio_gpio_lm+ mmap_seek);
  else
      val = *(m64_mmio_gpio_ah + mmap_seek);

  return val;
}

static void sunxi_gpio_writel(uint32_t val, uint32_t addr, int bank)
{
  uint32_t mmap_base = (addr & ~MAP_MASK);
  uint32_t mmap_seek = ((addr - mmap_base) >> 2);

  if(bank == 11)
      *(m64_mmio_gpio_lm+ mmap_seek) = val;
  else
      *(m64_mmio_gpio_ah + mmap_seek) = val;
}

static void sunxi_set_pin_function(const int gpio_number, int mode)
{
  uint32_t regval = 0;
  int bank = gpio_number >> 5;
  int index = gpio_number - (bank << 5);
  int offset = ((index - ((index >> 3) << 3)) << 2);
  uint32_t phyaddr=0;

  if(bank == 11)
    phyaddr = SUNXI_GPIO_LM_BASE + ((bank - 11) * 36) + ((index >> 3) << 2);
  else
    phyaddr = SUNXI_GPIO_AH_BASE + (bank * 36) + ((index >> 3) << 2);

  regval = sunxi_gpio_readl(phyaddr, bank);
  regval &= ~(7 << offset);
  regval |=  ((mode & 0x7) << offset);
  sunxi_gpio_writel(regval, phyaddr, bank);
}

static uint32_t sunxi_get_pin_level(const int gpio_number)
{
  uint32_t regval = 0;
  int bank = gpio_number >> 5;
  int index = gpio_number - (bank << 5);
  uint32_t phyaddr=0;

  if(bank == 11)
    phyaddr = SUNXI_GPIO_LM_BASE + ((bank - 11) * 36) + 0x10;
  else
    phyaddr = SUNXI_GPIO_AH_BASE + (bank * 36) + 0x10;

  regval = sunxi_gpio_readl(phyaddr, bank);
  regval = regval >> index;
  regval &= 1;

  return regval;
}

static void sunxi_set_pin_level(const int gpio_number, int value)
{
  uint32_t regval = 0;
  int bank = gpio_number >> 5;
  int index = gpio_number - (bank << 5);
  uint32_t phyaddr=0;

  if(bank == 11)
    phyaddr = SUNXI_GPIO_LM_BASE + ((bank - 11) * 36) + 0x10;
  else
    phyaddr = SUNXI_GPIO_AH_BASE + (bank * 36) + 0x10;

  regval = sunxi_gpio_readl(phyaddr, bank);
  if(0 == value)
    {
      regval &= ~(1 << index);
      sunxi_gpio_writel(regval, phyaddr, bank);
      regval = sunxi_gpio_readl(phyaddr, bank);}
    else
    {
      regval |= (1 << index);
      sunxi_gpio_writel(regval, phyaddr, bank);
      regval = sunxi_gpio_readl(phyaddr, bank);
    }
}


static inline void m64_mmio_set_input(const int gpio_number) {
  sunxi_set_pin_function(gpio_number, INPUT);
}

static inline void m64_mmio_set_output(const int gpio_number) {
  sunxi_set_pin_function(gpio_number, OUTPUT);
}

static inline void m64_mmio_set_high(const int gpio_number) {
  sunxi_set_pin_level(gpio_number, HIGH);
}

static inline void m64_mmio_set_low(const int gpio_number) {
  sunxi_set_pin_level(gpio_number, LOW);
}

static inline uint32_t m64_mmio_input(const int gpio_number) {
  return sunxi_get_pin_level(gpio_number);
}

#endif
