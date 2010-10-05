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


// Currently the only supported card format is BCD (track 2)...
#define MAGSTRIPE_FMT_BCD 1
#define MAGSTRIPE_FMT_SIXBIT 2

// The pins used by this library...
#define MAGSTRIPE_RDT 2  /* data pin (blue) */
#define MAGSTRIPE_RCL 3  /* clock pin (green) */
#define MAGSTRIPE_CLS 4  /* card present pin (yellow) */


class MagStripe {
    public:
        void begin(unsigned char format=MAGSTRIPE_FMT_BCD);
        void stop();
        bool available() { return digitalRead(MAGSTRIPE_CLS) == LOW; };
        short read(char *data, unsigned char size);

    private:
        unsigned char format;

        void reverse_bits();
        short find_start_bcd();
        short decode_bits_bcd(char *data, unsigned char size);
        bool check_lrc(volatile unsigned char *bits, short size, unsigned char parity_bit);
        char bcd_to_char(unsigned char bcd);
};


#endif MAGSTRIPE_H


/* vim: set expandtab ts=4 sw=4: */
