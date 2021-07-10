# Lullaby
Open source smart music box based on Arduino. It can store a melody inside the EEPROM which can later be played by pressing a button or by sending commands to the serial port. 

## Features
 - programmable, with persistent melody storage
 - supports 4 types of notes (whole, half, quarter, eighth) with relative rests, across 7 octaves. Notes can also be altered and dotted
 - customizable BPM
 - supports any time signature having a denominator of 2, 4 or 8 (e.g. 4/4, 2/2, 6/8...)
 - dedicated (mobile app)[https://github.com/aurasphere/lullaby-mobile]
 - real-time playing (can be used as a piano or while composing the melody to store)
 - melody export (useful to save multiple melodies and restore them anytime)
 - optional led randomization on each note played

## Components
 - HC-05 Bluetooth Module
 - Active Buzzer
 - Arduino Nano
 - Button
 - Resistance /TODO
 
## Schematics

![Schematics](lullaby.png)

## Commands

Communication is performed through serial port. Each command starts with an ASCII lowercase character. Some commands also require a melody argument (the encoding is described in the next paragraph). The available commands are the following:

 - p, plays the melody currently stored in EEPROM
 - s<melody>, saves the given melody into EEPROM
 - d, prints the details of the melody stored
 - e, exports the given melody in the Lullaby protocol
 - n<melody>, plays the given melody
 
## Melody Protocol

Lullaby uses a custom protocol based on hexadecimal bit stuffing. Each melody is represented by appending an header to a sequence of notes:

    <header><note><note><note>...
	
Both the header and the notes have a fixed length of 3 characters, so the length of each encoded melody is guaranteed to be a multiple of 3. The length of any melody is equal to (encodedLength - 3) / 3.

### Header

The melody header contains the BPM and beat unit (e.g. the denominator of the time signature). The BPM is converted to 2 hexadecimal characters (using padding if needed) and then prepended to the beat unit, which is a digit between 1-3:
	  
|     Time Signature     | Beat Unit |
|:----------------------:|:---------:|
| */2 (2/2, 3/2, 4/2...) |     1     |
| */4 (2/4, 4/4, 5/4...) |     2     |
| */8 (3/8, 6/8, 9/8...) |     3     |
	    
For example, given a melody with a time signature of 4/4 and 120 BPM, the resulting header will be 782.
	    
### Notes
	    
