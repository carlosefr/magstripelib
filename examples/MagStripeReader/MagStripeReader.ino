/*
 * MagStripeReader - Read data from a magnetic stripe card (track 1, 2 or 3).
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


#include <MagStripe.h>


// Visual feedback when the card is being read...
static const byte READ_LED = 13;
static const byte ERROR_LED = 12;

MagStripe card;

/*
 * Track 3 is the one that can contain the most characters (107).
 * We add one more to accomodate the final '\0', as the data is a C string...
 */
static const byte DATA_BUFFER_LEN = 108;
static char data[DATA_BUFFER_LEN];


void setup()
{
  pinMode(READ_LED, OUTPUT);
  pinMode(ERROR_LED, OUTPUT);
  
  // The card data will be sent over serial...
  Serial.begin(9600);
  
  // Initialize the library for reading track 2...
  card.begin(2);

  // Start with the feedback LEDs off...
  digitalWrite(READ_LED, LOW);
  digitalWrite(ERROR_LED, LOW);
}

 
void loop()
{
  // Don't do anything if there isn't a card present...
  if (!card.available()) {
    return;
  }
  
  // Show that a card is being read...
  digitalWrite(READ_LED, HIGH);
  
  // Read the card into the buffer "data" (as a null-terminated string)...
  short chars = card.read(data, DATA_BUFFER_LEN);
  
  // Show that the card has finished reading...
  digitalWrite(READ_LED, LOW);
  
  // If there was an error reading the card, blink the error LED...
  if (chars < 0) {
    digitalWrite(ERROR_LED, HIGH);
    delay(250);
    digitalWrite(ERROR_LED, LOW);

    return;
  }

  // Send the data to the computer...
  Serial.println(data);
}


/* EOF - MagStripeReader.ino */
