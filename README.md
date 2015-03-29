Today, we will work on something pretty basic. Trying to get a simple 7 segment display working. Too make our lives easier we are going to use a 74HC595 Shift Register to control our display, one of these IC's can control a 4-digit display(depends on the display though!).

##Things Needed

* A 7 Segment Display(Ideally multiple)
* 1 74HC595 Shift Register IC
* MCU (PI/Arduino)

I ordered a 8 Digit 7 segment display that already had 2 74HC595 IC's on the board from [DX](http://dx.com), you can get your's [here](http://www.dx.com/p/bonatech-8-digit-led-seven-segment-display-module-deep-blue-297686#.VRe5VJOUeQw).

##Background

###7 Segment Display
(Excerpts from Wikipedia, read the [article](http://en.wikipedia.org/wiki/Seven-segment_display) for more info)
A simple 7 segment display is made up of 7 LED segments. The seven segments are arranged as a rectangle of two vertical segments on each side with one horizontal segment on the top, middle, and bottom. Additionally, the seventh segment bisects the rectangle horizontally.  Typically all of the cathodes (negative terminals) or all of the anodes (positive terminals) of the segment LEDs are connected and brought out to a common pin; this is referred to as a "common cathode" or "common anode" device. Hence a 7 segment plus decimal point package will only require nine pins, though commercial products typically contain more pins, and/or spaces where pins would go, in order to match standard IC sockets. Integrated displays also exist, with single or multiple digits. Some of these integrated displays incorporate their own internal decoder, though most do not: each individual LED is brought out to a connecting pin as described.

###Shift Register
(Excerpts from Wikipedia, read the [article](http://en.wikipedia.org/wiki/Shift_register) for more info)
A shift register is a cascade of flip flops, sharing the same clock, in which the output of each flip-flop is connected to the "data" input of the next flip-flop in the chain, resulting in a circuit that shifts by one position the "bit array" stored in it, shifting in the data present at its input and shifting out the last bit in the array, at each transition of the clock input.

###Using them together
One thing you may have noticed, is that using a shift register can only display 1 digit at time, this is not a problem when you are using a single digit display with the 74HC595, however this is an issue when we use multiple digits. To remedy this, we take advantage of the fact the average human eye cannot process more than 10-12 images per second. What we end up doing is actually showing the digits one by one in some order, but they change so fast that to the human eye, all the digits appear static.

##Wiring it up
The setup is pretty easy, because all we need is 5 wires, a VCC, a ground, and the clock, latch and data from the 74HC595 IC.

The raspberry pi wiring looks like:
<img src="/content/images/2015/03/RPi--595.png" width=400 />

The arduino wiring looks like:
<img src="/content/images/2015/03/arduino--595.png" width=400 />

Where the pin definitions are as follows:
DIO -> Data
SCLK -> Shift Register Clock
RCLK -> Latch/Storage Register Clock

##Code
###Raspberry Pi
To keepo things simple I'll by using a [library](https://pypi.python.org/pypi/Pi7SegPy) that I created for this.

So all you need to do is:

	pip install Pi7SegPy

And the code to run is:

	import Pi7SegPy
	Pi7SegPy.init(18,23,24,1,4)
	while True:
		Pi7SegPy.show([5,'P','I','O'], [3])


All this code does is initialize the library with respective pins and a single 4 digit 7 segment display on 1 74HC595 IC. Then its creates an inifite loop which displays the string "5P.IO".

The real magic is what is happening in the library, which I will try and outline in the following steps:

1. The library gets initialized with respective pins.
2. The library reads the string and one by one it writes each digit of the display by reading a map of which segments need to be turned on for each digit.
3. To do this it calls into another library, [PiShiftPy](https://pypi.python.org/pypi/Pi7ShiftPy), which just takes in series of bytes and writes it out to the shift register and flips the latch.

For more details, you should check out the documentation of the respective libraries.

Thats basically all that needs to be done, the end result should look something like this:
<img src="/content/images/2015/03/IMG_0018.JPG" width=400 />

###Arduino

As the Arduino is already pretty good at using shift registers, I decided to write this one completely from scratch. Now, this got a little complicated as the Arduino does not have any native support for Hash Tables. To remedy this I use 2 different arrays, one to store the characters that I can show, and one more to store the respective bits to send the display.

They look something like this:

    const byte ARRAY_SIZE = 27;
	byte segment_codes[ARRAY_SIZE];
	String available_chars[ARRAY_SIZE];
    available_chars[0] = "0";
    available_chars[1] = "1";
    available_chars[2] = "2";
    .
    .
    .
    segment_codes[0]=0b11000000;
    segment_codes[1]=0b11111001;
    segment_codes[2]=0b10100100;
    .
    .
    .

The function I use to actually show the string is:

    void show(String value){
  		int skip = 0;
  		for(int i=0; i<value.length(); i++) {
    		if(!((String)value[i]).equals(".")) {
      			int indexOfCharToWrite = findIndexOfChar((String)value[i]);
      			if(indexOfCharToWrite > -1){
        			byte bitToWrite = 0;
        			digitalWrite(latchPin, LOW);
        			byte code = segment_codes[indexOfCharToWrite];
        			boolean skipNext = false;
        			if(((String)value[i+1]).equals(".")) {
          				bitWrite(code, 7, 0);
          				skipNext = true;
        			}
        			shiftOut(dataPin, clockPin, MSBFIRST, code);
        			bitWrite(bitToWrite, i-skip, 1);
        			shiftOut(dataPin, clockPin, LSBFIRST, bitToWrite);
        			digitalWrite(latchPin, HIGH);
        			if(skipNext){
          				skip++;
        			}
      			}
    		}
    	}
	}


    int findIndexOfChar(String character){
  		int wantedpos = -1;
  		for(int i=0; i< ARRAY_SIZE; i++){
    		if(character.equals(available_chars[i])){
      			wantedpos = i;
      			break;
    		}
  		}
  	return wantedpos;
	}

Here's what is actually happening:

1. We first iterate through the string and process each character.
2. We then check if that character is dot, if it is, we just skip it.
3. We then find the index of character in our available_chars array defined earlier.
4. If it does exist, we start writing to the shift register.
5. We first set the latch to LOW so that the register knows that it is being written too.
6. We then get the bits to send to the display from the segment_codes array.
7. We then check if the next character is dot, we have to this because the dot as a next character will actually have to applied to this digit.
8. If there is a dot, we alter the bit for the dot, and tell the program we will skip the next character(So that it is not blank!).
9. We then shift out the final bits to the register.
10. We then shift out the bit telling the display which digit the previous shift should go to.
11. Finally, we pull the Latch pin to HIGH to tell the register we are done writing.

The end result should look something like this:
<img src="/content/images/2015/03/IMG_0020-1.JPG" width=400 />

The full code for this is available on the [repository](https://github.com/5pIO/7Segment).
