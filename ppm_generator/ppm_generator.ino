/*
 * PPM generator originally written by David Hasko
 * on https://code.google.com/p/generate-ppm-signal/ 
 */

//////////////////////CONFIGURATION///////////////////////////////
#define CHANNEL_NUMBER 9  //set the number of chanels
#define CHANNEL_DEFAULT_VALUE 1500  //set the default servo value
#define FRAME_LENGTH 22500  //set the PPM frame length in microseconds (1ms = 1000µs)
#define PULSE_LENGTH 300  //set the pulse length
#define onState 1  //set polarity of the pulses: 1 is positive, 0 is negative
#define sigPin 10  //set PPM signal output pin on the arduino

#define CMD_START_BYTE 255
#define USED_CHANNELS 4
#define ACCURACY_OFFSET 13

/*this array holds the servo values for the ppm signal
 change theese values in your code (usually servo values move between 1000 and 2000)*/
int ppm[CHANNEL_NUMBER];
int curChannel;
int readByte;
bool readingCmd;

void printValues() {
  Serial.print("Read byte:");
  Serial.print(readByte);
  Serial.print(", Cur channel: ");
  Serial.print(curChannel);
  Serial.print(", reading Cmd: ");
  Serial.print(readingCmd);
  Serial.print(", ppm[curChannel]: ");
  Serial.println(ppm[curChannel]);
}

void setup(){  
  curChannel = 0;
  readByte = 0;
  readingCmd = false;

  //initiallize default ppm values
  for(int i=0; i<CHANNEL_NUMBER; i++){
      ppm[i]= CHANNEL_DEFAULT_VALUE;
  Serial.begin(9600);
  }

  pinMode(sigPin, OUTPUT);
  digitalWrite(sigPin, !onState);  //set the PPM signal pin to the default state (off)
  
  cli();
  TCCR1A = 0; // set entire TCCR1 register to 0
  TCCR1B = 0;
  
  OCR1A = 100;  // compare match register, change this
  TCCR1B |= (1 << WGM12);  // turn on CTC mode
  TCCR1B |= (1 << CS11);  // 8 prescaler: 0,5 microseconds at 16mhz
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  sei();

}

void loop(){
  if(readingCmd) {
    if(Serial.available()){
      readByte = Serial.read();
      if(readByte == CMD_START_BYTE) {
        readingCmd = true;
        curChannel = 0;
      } else {
        ppm[curChannel] = int((readByte * 3.92) + 1000);
        curChannel++;
        if(curChannel == USED_CHANNELS) {
          curChannel = 0;
          readingCmd = false; 
        }
      }
    }
  } else {
    if(Serial.available()) {
      readByte = Serial.read();
      if(readByte == CMD_START_BYTE) {
        readingCmd = true;
      } 
    }
  }
  //printValues();
}

ISR(TIMER1_COMPA_vect){  //leave this alone
  static boolean state = true;
  
  TCNT1 = 0;
  
  if (state) {  //start pulse
    digitalWrite(sigPin, onState);
    OCR1A = PULSE_LENGTH * 2;
    state = false;
  } else{  //end pulse and calculate when to start the next pulse
    static byte cur_chan_numb;
    static unsigned int calc_rest;
  
    digitalWrite(sigPin, !onState);
    state = true;

    if(cur_chan_numb >= CHANNEL_NUMBER){
      cur_chan_numb = 0;
      calc_rest = calc_rest + PULSE_LENGTH;// 
      OCR1A = (FRAME_LENGTH - calc_rest) * 2;
      calc_rest = 0;
    }
    else{
      OCR1A = (ppm[cur_chan_numb] - ACCURACY_OFFSET - PULSE_LENGTH) * 2;
      calc_rest = calc_rest + ppm[cur_chan_numb] - ACCURACY_OFFSET;
      cur_chan_numb++;
    }     
  }
}
