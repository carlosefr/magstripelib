/*
 * MagStripe - Read data from a magnetic stripe card.
 *
 * Copyright (c) 2010 Carlos Rodrigues <cefrodrigues@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * Based on this: http://cal.freeshell.org/2009/11/magnetic-stripe-reader/
 *   ...and this: http://www.abacus21.com/Magnetic-Strip-Encoding-1586.html
 */


#ifndef MAGSTRIPE_H
#define MAGSTRIPE_H


#include "WProgram.h"

/*
 * This value is the maximum needed to read any of the three tracks.
 *
 * In the future the code may be changed to reduce memory usage by
 * packing bits, but for now if you have memory constraints and will
 * only be reading track 2, you can reduce this safely to 320 bytes.
 */
#define BIT_BUFFER_LEN 768 

// The pins used by this library...
#define MAGSTRIPE_RDT 2  /* data pin (blue) */
#define MAGSTRIPE_RCL 3  /* clock pin (green) */
#define MAGSTRIPE_CLS 4  /* card present pin (yellow) */


class MagStripe {
    public:
        // Initialize the library (attach interrupts)...
        void begin(unsigned char track);

        // Deinitialize the library (detach interrupts)...
        void stop();

        // Check if there is a card present for reading...
        bool available() { return digitalRead(MAGSTRIPE_CLS) == LOW; };

        // Read the data from the card as ASCII...
        short read(char *data, unsigned char size);

    private:
        unsigned char track;

        void reverse_bits();
        bool verify_lrc(volatile unsigned char *bits, short size, unsigned char parity_bit);
        short find_sentinel_bcd();
        short find_sentinel_sixbit();
        short decode_bits(char *data, unsigned char size);
        short decode_bits_bcd(char *data, unsigned char size);
        short decode_bits_sixbit(char *data, unsigned char size);
        char char_from_bcd(unsigned char bcd);
        char char_from_sixbit(unsigned char sixbit);
};


#endif MAGSTRIPE_H


/* vim: set expandtab ts=4 sw=4: */
