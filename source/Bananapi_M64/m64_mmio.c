// Copyright (c) 2014 Adafruit Industries
// Author: Tony DiCola

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
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "m64_mmio.h"

#define GPIO_LENGTH 4096
#define GPIOAH_ADDR 0x01C20000
#define GPIOLM_ADDR 0x01f02000

// Cache memory-mapped GPIO addresses.
volatile uint32_t* m64_mmio_gpio_ah = NULL;
volatile uint32_t* m64_mmio_gpio_lm = NULL;

int m64_mmio_init_gpio(int pin) {
  // Validate input parameters.
  if (pin < 0 || pin > 364) {
    return MMIO_ERROR_ARGUMENT;
  }

  // On older kernels user readable /dev/gpiomem might not exists.
  // Falls back to root-only /dev/mem.
  int fd;
  if( access( "/dev/gpiomem", F_OK ) != -1 ) {
    fd = open("/dev/gpiomem", O_RDWR | O_SYNC);
  } else {
    fd = open("/dev/mem", O_RDWR | O_SYNC);
  }
  if (fd == -1) {
    // Error opening /dev/gpiomem.
    return MMIO_ERROR_DEVMEM;
  }
  
  // Map GPIO memory if its hasn't been mapped already.
  if (m64_mmio_gpio_ah == NULL) {
    // Map GPIO memory to location in process space.
    m64_mmio_gpio_ah = (uint32_t*)mmap(NULL, GPIO_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIOAH_ADDR);
    
    if (m64_mmio_gpio_ah == MAP_FAILED) {
      // Don't save the result if the memory mapping failed.
      m64_mmio_gpio_ah = NULL;
      return MMIO_ERROR_MMAP;
    }
  }

  if (m64_mmio_gpio_lm == NULL) {
    // Map GPIO memory to location in process space.
    m64_mmio_gpio_lm = (uint32_t*)mmap(NULL, GPIO_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIOLM_ADDR);
    
    if (m64_mmio_gpio_lm == MAP_FAILED) {
      // Don't save the result if the memory mapping failed.
      m64_mmio_gpio_lm = NULL;
      return MMIO_ERROR_MMAP;
    }
  }

  close(fd);
  return MMIO_SUCCESS;
}
