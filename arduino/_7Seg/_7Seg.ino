int dataPin = 2;
int latchPin = 3;
int clockPin = 4;
const byte ARRAY_SIZE = 27;
byte segment_codes[ARRAY_SIZE];
String available_chars[ARRAY_SIZE];

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

void setup() {
  available_chars[0] = "0";
  available_chars[1] = "1";
  available_chars[2] = "2";
  available_chars[3] = "3";
  available_chars[4] = "4";
  available_chars[5] = "5";
  available_chars[6] = "6";
  available_chars[7] = "7";
  available_chars[8] = "8";
  available_chars[9] = "9";
  available_chars[10] = "A";
  available_chars[11] = "b";
  available_chars[12] = "D";
  available_chars[13] = "c";
  available_chars[14] = "d";
  available_chars[15] = "E";
  available_chars[16] = "F";
  available_chars[17] = "H";
  available_chars[18] = "h";
  available_chars[19] = "L";
  available_chars[20] = "n";
  available_chars[21] = "I";
  available_chars[22] = "O";
  available_chars[23] = "o";
  available_chars[24] = "P";
  available_chars[25] = "S";
  available_chars[26] = " ";

  segment_codes[0]=0b11000000;
  segment_codes[1]=0b11111001;
  segment_codes[2]=0b10100100;
  segment_codes[3]=0b10110000;
  segment_codes[4]=0b10011001;
  segment_codes[5]=0b10010010;
  segment_codes[6]=0b10000011;
  segment_codes[7]=0b11111000;
  segment_codes[8]=0b10000000;
  segment_codes[9]=0b10011000;
  segment_codes[10]=0b10001000;
  segment_codes[11]=0b10000011;
  segment_codes[12]=0b11000110;
  segment_codes[13]=0b10100111;
  segment_codes[14]=0b10100001;
  segment_codes[15]=0b10000110;
  segment_codes[16]=0b10001110;
  segment_codes[17]=0b10001001;
  segment_codes[18]=0b10001011;
  segment_codes[19]=0b11000111;
  segment_codes[20]=0b10101011;
  segment_codes[21]=0b11111001;
  segment_codes[22]=0b11000000;
  segment_codes[23]=0b10100011;
  segment_codes[24]=0b10001100;
  segment_codes[25]=0b10010010;
  segment_codes[26]=0b11111111;


  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void loop() {
  show("    5P.IO");
}



