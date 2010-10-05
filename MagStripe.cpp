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
 */


#include "WProgram.h"
#include "MagStripe.h"


// This is enough for now (BCD on track 2 only)...
#define BIT_BUFFER_LEN 320


// Variables used by the interrupt handlers...
static volatile unsigned char next_bit = 0;          // next bit to read
static volatile unsigned char bits[BIT_BUFFER_LEN];  // buffer for bits being read
static volatile short num_bits = 0;                  // number of bits already read

// The interrupt handlers...
static void handle_data(void);
static void handle_clock(void);


void MagStripe::begin(unsigned char format)
{
    this->format = format;

    pinMode(MAGSTRIPE_RDT, INPUT);
    pinMode(MAGSTRIPE_RCL, INPUT);
    pinMode(MAGSTRIPE_CLS, INPUT);

    // Reading is more reliable when using interrupts...
    attachInterrupt(0, handle_data, CHANGE);    // data on digital pin 2
    attachInterrupt(1, handle_clock, FALLING);  // clock on digital pin 3
}


void MagStripe::stop()
{
    detachInterrupt(0);
    detachInterrupt(1);
}


short MagStripe::read(char *data, unsigned char size)
{
    // Currently only the BCD format (track 2) is supported...
    if (this->format != MAGSTRIPE_FMT_BCD) {
        return -1;
    }

    // Fail if no card present...
    if (!this->available()) {
        return -1;
    }
 
    // Wait while the data is being read by the interrupt routines...
    while (this->available()) {}

    // TODO: support other formats besides BCD on track 2...
    short chars = this->decode_bits_bcd(data, size);

    // If the data looks bad, reverse and try again...
    if (chars < 0) {
        this->reverse_bits();
        chars = this->decode_bits_bcd(data, size);
    }

    // Reset the bit buffer...
    num_bits = 0;

    return chars;
}


void MagStripe::reverse_bits()
{
    for (short i = 0; i < num_bits / 2; i++) {
        unsigned char b = bits[i];

        bits[i] = bits[num_bits - i - 1];
        bits[num_bits - i - 1] = b;
    }
}


short MagStripe::find_start_bcd()
{
    unsigned char bcd_accum = 0x00;
  
    for (short i = 0; i < num_bits; i++) {
        bcd_accum = (bcd_accum << 1) & 0x1f;  // rotate the 5 bits to the left...
        bcd_accum |= bits[i];                 // ...and add the current bit
    
        // Stop when the start sentinel (';') is found...
        if (bcd_accum == 0x1a) {
            return i - 4;
        }
    }

    // Error, no start sentinel was found...
    return -1;
}


short MagStripe::decode_bits_bcd(char *data, unsigned char size) {
    short counter = 0;
    unsigned char chars = 0;
    unsigned char bcd_accum = 0x00;
  
    short start = this->find_start_bcd();
    if (start < 0) {  // error, start sentinel not found
        return -1;
    }
 
    for (short i = start; i < num_bits; i++) {
        bcd_accum = (bcd_accum << 1) & 0x1f;  // rotate the 5 bits to the left...
        bcd_accum |= bits[i];                 // ...and add the current bit
    
        counter++;
    
        if (counter % 5 == 0) {
            if (chars >= size) {
                return -1;
            }

            // A null means we reached the end of the data...
            if (bcd_accum == 0x00) {
                break;
            }
            
            // This checks the parity implicitly...
            char c = this->bcd_to_char(bcd_accum);
      
            if (c == '\0') { // error, invalid character (bad parity)
                return -1;
            }

            data[chars] = c;
            chars++;
     
            // Reset the bit accumulator...
            bcd_accum = 0x00;
        }
    }
  
    // Turn the data into a null-terminated string...
    data[chars] = '\0';
  
    if (data[chars-2] != '?') {  // error, the end sentinel is not in the right place
        return -1;
    }

    // Verify the LRC (even parity across columns)...
    if (!check_lrc(&bits[start], chars*5, 5)) {
        return -1;
    }

    return chars;
}


bool MagStripe::check_lrc(volatile unsigned char *bits, short size, unsigned char parity_bit)
{
    // Count the number of ones per column (ignoring parity bits)...
    for (short i = 0; i < (parity_bit-1); i++) {
        short counter = 0;

        for (short j = i; j < size; j += parity_bit) {
            counter += bits[j];
        }

        // Even parity is what we want...
        if (counter % 2 != 0) {
            return false;
        }
    }

    return true;
}


char MagStripe::bcd_to_char(unsigned char bcd)
{
    // This decodes and checks the (odd) parity in one go...
    switch (bcd) {
        case 0x01: return '0';
        case 0x10: return '1';
        case 0x08: return '2';
        case 0x19: return '3';
        case 0x04: return '4';
        case 0x15: return '5';
        case 0x0d: return '6';
        case 0x1c: return '7';
        case 0x02: return '8';
        case 0x13: return '9';
        case 0x0b: return ':';  // control
        case 0x1a: return ';';  // start sentinel
        case 0x07: return '<';  // control
        case 0x16: return '=';  // field separator
        case 0x0e: return '>';  // control
        case 0x1f: return '?';  // end sentinel
    }

    // Error, invalid character (bad parity)...
    return '\0';
}


static void handle_data()
{
    next_bit = (next_bit == 0) ? 1 : 0;
}
 

static void handle_clock()
{
    // Avoid a crash in case there are too many bits (garbage)...
    if (num_bits >= BIT_BUFFER_LEN) {
        return;
    }

    bits[num_bits] = next_bit;
    num_bits++;
}


/* vim: set expandtab ts=4 sw=4: */
