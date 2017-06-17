/*
* MIDI to EDP Link interface
* MIDI channel 1
*
* RX: digital pin 0
* GATE: digital pin 13 (LED)
* PITCH: digital pins 2 through 7
*
* EDP Link uses 6 bits
* The lowest 4 bits contain note information (pins 2 through 5)
* The highest 2 bit contain octave information (pins 6 and 7)
*/

# define BAUDRATE 31250
# define NOTE_ON 144
# define NOTE_OFF 128

# define STATUS_WAITING_FOR_NOTE_ON 0
# define STATUS_WAITING_FOR_NOTE 1
# define STATUS_WAITING_FOR_VELOCITY 2

byte status = STATUS_WAITING_FOR_NOTE_ON;
byte note = 0;
byte velocity = 0;

void openGate() {
  // set the gate pin HIGH
  PORTB = B11111111;
}

void closeGate() {
  // set the gate pin LOW
  PORTB = B00000000;
}

void setNote(byte data) {
  // strangely enough we need to count down ...
  // could be a wrong implementation in my Wasp clone (Jasper)
  byte octave = 2 - (data / 12) << 4;
  byte note = 11 - (data % 12);

  PORTD = (note + octave) << 2;
}

void setup() {
  Serial.begin(BAUDRATE);

  // set digital pins 1 through 13 to WRITE
  DDRD = B11111110;
  DDRB = B11111111;

  // set the initial status for all pins to LOW
  PORTD = B00000000;
  PORTB = B00000000;
}

void loop() {
  // check for incoming byte
  if (Serial.available() > 0) {
    byte incoming = Serial.read();

    if (incoming == NOTE_ON) {
      status = STATUS_WAITING_FOR_NOTE;
    } else if (incoming == NOTE_OFF) {
      closeGate();
      status = STATUS_WAITING_FOR_NOTE_ON;
    } else if (status == STATUS_WAITING_FOR_NOTE) {
      // check if incoming is between C2(MIDI 48) and C5(MIDI 84)
      // if not, revert status to STATUS_WAITING_FOR_NOTE_ON
      if (incoming > 47 && incoming < 85) {
        // compensate: MIDI note 48 is EDP Link note 0
        note = incoming - 48;
        status = STATUS_WAITING_FOR_VELOCITY;
      } else {
        status = STATUS_WAITING_FOR_NOTE_ON;
      }
    } else if (status == STATUS_WAITING_FOR_VELOCITY) {
      // just ignore the velocity byte
      // velocity = incoming;
      setNote(note);
      openGate();
      // wait for the next note to accomodate for MIDI running status
      status = STATUS_WAITING_FOR_NOTE;
    }
  }
}
