scrolltext
==========
Uses a modified version of HT1632 libraries (https://github.com/adafruit/HT1632) to smoothly scroll text across LED panels. All of this library code except for the left-shifting routines was created by the good folks at Adafruit (http://www.adafruit.com), not by me.

scrolltext.ino is an Arduino sketch that listens on the serial port for any incoming text and performs the scrolling routine.

Assumes four chained HT1632C panels, but the number can be changed at the top of scrolltext.ino.

See here for tutorial on setting up panels: http://learn.adafruit.com/16x24-led-matrix/
