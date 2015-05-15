What is it?
===========

**MagStripe** is an [Arduino](http://arduino.cc/) library to interface with TTL (raw) magnetic card readers.
It supports reading any of the cards' possible three tracks, but not simultaneously. Most cards only contain
data on tracks 1 and 2 though.

![MagStripe-KDE.jpg](http://cloud.carlos-rodrigues.com/projects/magstripelib/MagStripe-KDE.jpg)

Card Readers
============

There are many brands of TTL magnetic card readers on the market, some with a fixed reading head and others
with screws allowing the head to be manually positioned to choose one of the tracks. The connector comes with
varying colors for each wire, but usually there are five wires with the following order and function:

  * Ground (black wire)
  * RDT/data
  * RCL/clock
  * CLS/card present
  * Power/+5V (red wire)

For more information, you can check the
[KDE KDR-1000 datasheet](http://cloud.carlos-rodrigues.com/projects/magstripelib/KDR1000.pdf) (pictured above),
which can read all three tracks by repositioning the head, or the
[Panasonic ZU-M1121 datasheet](http://cloud.carlos-rodrigues.com/projects/magstripelib/ZU-M1121S1.pdf) for an
example of another reader which can only read track 2 and has a different pin arrangement.

Installation
============

Download the latest zip file from the [releases](https://github.com/carlosefr/magstripelib/releases) section. Then open it from the `Sketch > Include Library > Add .ZIP Library...` menu inside the Arduino IDE and a new "MagStripe" entry should appear in the `Sketch > Include Library` and `File > Examples` menus.

![Arduino IDE](http://cloud.carlos-rodrigues.com/projects/magstripelib/screenshot-01.png)

How it Works
============

Connect your card reader to the following digital pins of the **Arduino Uno** (or other arduinos based on the
ATmega8/168/328 processors):

Arduino Pin   | Card Reader Pin
--------------|-----------------
Digital Pin 2 | RDT/data
Digital Pin 3 | RCL/clock
Digital Pin 4 | CLS/card present

The **Arduino Leonardo** or **Arduino Micro** (or other arduinos based on the ATmega32U4 processor) have a diferent
interrupt pin assignment so, if you have one of these, connect the wires like this instead (swapped clock/data wires):

Arduino Pin   | Card Reader Pin
--------------|-----------------
Digital Pin 2 | RCL/clock
Digital Pin 3 | RDT/data
Digital Pin 4 | CLS/card present

If your reader can read multiple tracks at a time, and thus has extra sets of data and clock wires, check its
datasheet and use the ones appropriate for the track you want to read.

The library has a thin [interface](MagStripe.h) and is very straightforward to use, just check if there is a
card present before trying to read. The included [`MagStripeReader.ino`](examples/MagStripeReader/MagStripeReader.ino)
example shows how to do it.

The `read()` method does the necessary validation checks and only returns data if it has been read correctly
from the card. The data returned is a string with the (ASCII) full contents of the track, including the control
characters.

To know about the format of data returned for each track, check the
[magnetic card standards](https://github.com/carlosefr/magstripelib/blob/docs/docs/layoutstd.pdf?raw=true)
reference.
