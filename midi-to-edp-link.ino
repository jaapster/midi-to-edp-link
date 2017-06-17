
/*
* MIDI to EDP-LINK interface
* MIDI-channel 1
*
* RX: digital pin 0
* GATE: digital pin 13
* NOTE: digital pins 3 - 8
*

EDP Link uses 6 bits
The lowest 4 bits contain note information
The highest 2 bit contain octave information

Examples:

B 0001 0001 is note 1 in octave 1
B 0010 0011 is note 3 in octave 2

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
  // set digital pin 13 (LED) HIGH
  PORTB = B11111111;
}

void closeGate() {
  // set digital pin 13 (LED) LOW
  PORTB = B00000000;
}

void setNote(byte data) {
  byte octave = 2 - (data / 12) << 4;
  byte note = 11 - (data % 12);

  PORTD = (note + octave) << 2;
}

void setup() {
  Serial.begin(BAUDRATE);

  // Set digital pins 2 - 13 to WRITE
  // and leave 0 and 1 (rx/tx) alone
  DDRD = B11111110;
  DDRB = B11111111;

  // set the initial status for all pins to LOW
  PORTD = B00000000;
  PORTB = B00000000;
}

void loop() {
  if (Serial.available() > 0) {
    byte incoming = Serial.read();

    if (incoming == NOTE_ON) {
      status = STATUS_WAITING_FOR_NOTE;
    } else if (incoming == NOTE_OFF) {
      closeGate();
      status = STATUS_WAITING_FOR_NOTE_ON;
    } else if (status == STATUS_WAITING_FOR_NOTE) {
      // check if incoming is between C2(48) and C5(84)
      // if not, revert status to STATUS_WAITING_FOR_NOTE_ON
      if (incoming > 47 && incoming < 85) {
        note = incoming - 48;
        status = STATUS_WAITING_FOR_VELOCITY;
      } else {
        status = STATUS_WAITING_FOR_NOTE_ON;
      }
    } else if (status == STATUS_WAITING_FOR_VELOCITY) {
      velocity = incoming;
      setNote(note);
      openGate();
      // accomodate for MIDI running status
      status = STATUS_WAITING_FOR_NOTE;
    }
  }

}
