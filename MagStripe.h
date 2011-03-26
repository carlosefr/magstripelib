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


#include <WProgram.h>


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
        bool available();

        // Read the data from the card as ASCII...
        short read(char *data, unsigned char size);

    private:
        unsigned char track;

        void reverse_bits();
        bool verify_parity(unsigned char c);
        bool verify_lrc(short start, short length);
        short find_sentinel(unsigned char pattern);
        short decode_bits(char *data, unsigned char size);
};


#endif  /* MAGSTRIPE_H */


/* vim: set expandtab ts=4 sw=4: */
