
/*
* MIDI to EDP-LINK interface
* MIDI-channel 1
*/

# define BAUDRATE 31250
# define NOTE_ON 144
# define NOTE_OFF 128

# define STATUS_WAITING_FOR_NOTE_ON 0
# define STATUS_WAITING_FOR_NOTE 1
# define STATUS_WAITING_FOR_VELOCITY 2
# define STATUS_WAITING_FOR_VELOCITY 2

byte status = STATUS_WAITING_FOR_NOTE_ON;
byte note = 0;
byte velocity = 0;

void setup() {
  Serial.begin(BAUDRATE);

}

void loop() {
  if (Serial.available() > 0) {
    byte incoming = Serial.read();

    if (incoming == NOTE_ON) {
      status = STATUS_WAITING_FOR_NOTE;
    } else (incoming == NOTE_OFF) {

      // close gate

      status = STATUS_WAITING_FOR_NOTE_ON;
    } else if (status == STATUS_WAITING_FOR_NOTE) {
      note = incoming;
      status = STATUS_WAITING_FOR_VELOCITY;
    } else if (status == STATUS_WAITING_FOR_VELOCITY) {
      velocity = incoming;

      // set note and open gate

    }
  }
}
