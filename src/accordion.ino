// MIDI commands
const int NOTE_ON = 0x90;
const int NOTE_OFF = 0x80;
const int CONTROL_CHANGE = 0xB0;

typedef void (*FunctionArray) ();

// commands as chords
const boolean COMMAND_OCTAVE_UP[] = {
  false, false, false, false, false, false, false, false, false, true, false, false, false,
  true, false, false, false, true, false, false, true, false, false, false, true, false};
const boolean COMMAND_OCTAVE_DOWN[] = {
  false, false, false, false, false, false, false, true, false, false, false, false, false,
  true, false, false, false, true, false, false, true, false, false, false, true, false};
const boolean COMMAND_RESET[] = {
  false, false, false, true, false, false, false, false, false, false, false, false, false,
  true, false, false, false, true, false, false, true, false, false, true, false, false};


// 26 keys
const int key_count = 26;
// Send on MIDI channel 1
int channel = 1;

// poti pins
int vol_pin = 0;
int wheel_pin = 1;

int new_wheel_val = 0;
int wheel_val = 0x40;
int new_vol = 0;
int vol = 0x45;

int note_vol = 0x45;

// 3 switches for 3 octaves
int switch_count = 3;
int switches[] = {48, 50, 52};
int switch_vals[] = {0, 0, 0};
int switch_new_vals[] = {0, 0, 0};

// base MIDI note b
int base_note = 0x3B;
int octave = 0;
int keys[] = {41, 53, 39, 51, 37, 49, 35, 47, 33, 45, 31, 43,
              29, 27,  2, 25,  4,  3,  6,  5,  8,  7, 10,  9, 12, 11};
boolean last_pressed[key_count];
boolean cur_pressed[key_count];
int playing[0x7f];
boolean changed = false;
boolean command = false;

const int command_count = 3;
FunctionArray command_functions[command_count];
boolean commands[command_count][key_count];

int getNote(const int base, const int add_octaves, const int note) {
  return base + (octave + add_octaves) * 12 + note;
};

void sendMIDI(const int cmd, const int note, const int velocity) {
  Serial.write(cmd);
  Serial.write(note);
  Serial.write(velocity);
};

void commandReset() {
  sendMIDI(CONTROL_CHANGE | channel, 0x7B, 0);
  delay(5);
  setup();
  delay(1000);
}

void commandOctaveUp() {
  if (octave < 2) {
    octave++;
    sendMIDI(CONTROL_CHANGE | channel, 0x7B, 0);
    for (int note=0;note<0x7f;note++) playing[note] = 0;
  }
};

void commandOctaveDown() {
  if (octave > -2) {
    octave--;
    sendMIDI(CONTROL_CHANGE | channel, 0x7B, 0);
    for (int note=0;note<0x7f;note++) playing[note] = 0;
  }
};

boolean isCommand(const boolean pressed[], const boolean command[]) {
  for (int key=0;key<key_count;key++) {
    if (pressed[key] != command[key]) return false;
  }
  return true;
};

boolean computeKeyInput(int key) {
  if (cur_pressed[key] == HIGH && !last_pressed[key]) {
    // iteration for octave switches
    for (int l=0;l<3;l++) {
      if (switch_vals[l] > 0) {
	playing[getNote(base_note, l - 1, key)] += 1;
	if (playing[getNote(base_note, l - 1, key)] < 2) {
	  sendMIDI(NOTE_ON | channel, getNote(base_note, l - 1, key), note_vol);
	}
      }
    }
    last_pressed[key] = true;
    return true;
  }
  else if (cur_pressed[key] == LOW && last_pressed[key]) {
    for (int l=0;l<3;l++) {
      if (switch_vals[l] && playing[getNote(base_note, l - 1, key)] > 0) {
	playing[getNote(base_note, l - 1, key)] -= 1;
	if (playing[getNote(base_note, l - 1, key)] == 0) {
	  sendMIDI(NOTE_OFF | channel, getNote(base_note, l - 1, key), note_vol);
	}
      }
    }
    last_pressed[key] = false;
    return true;
  }
  return false;
};

boolean computeCommands(const boolean pressed[]) {
  for (int c=0;c<command_count;c++) {
    if (isCommand(pressed, commands[c])) {
      command_functions[c]();
      return true;
    }
  }
  return false;
}

void computeSwitchInput(int switch_ind, int value) {
  if (value != switch_vals[switch_ind]) {
    for (int key=0;key<key_count;key++) {
      if (last_pressed[key] &&
	  value == LOW &&
	  playing[getNote(base_note, switch_ind - 1, key)] > 0) {
	// remove playing from octave
	playing[getNote(base_note, switch_ind - 1, key)] -= 1;
	if (playing[getNote(base_note, switch_ind - 1, key)] <= 0) {
	  sendMIDI(NOTE_OFF | channel, getNote(base_note, switch_ind - 1, key), note_vol);
	}
      }
      else if (last_pressed[key] &&
	       value == HIGH) {
	playing[getNote(base_note, switch_ind - 1, key)] += 1;
	if (playing[getNote(base_note, switch_ind - 1, key)] < 2) {
	  sendMIDI(NOTE_ON | channel, getNote(base_note, switch_ind - 1, key), note_vol);
	}
      }
    }
    switch_vals[switch_ind] = value;
    sendMIDI(CONTROL_CHANGE | channel,
	     0x75 + switch_ind,
	     switch_vals[switch_ind] == HIGH ? 0x7f : 0);
  }
};

// the setup routine runs once when you press reset:
void setup() {
  octave = 0;
  for (int key=0;key<key_count;key++) commands[0][key] = COMMAND_RESET[key];
  for (int key=0;key<key_count;key++) commands[1][key] = COMMAND_OCTAVE_UP[key];
  for (int key=0;key<key_count;key++) commands[2][key] = COMMAND_OCTAVE_DOWN[key];
  command_functions[0] = commandReset;
  command_functions[1] = commandOctaveUp;
  command_functions[2] = commandOctaveDown;
  for (int i=0;i<0x7f;i++) {
    playing[i] = 0;
  }
  for (int key=0;key<key_count;key++) {
    pinMode(keys[key], INPUT);
    cur_pressed[key] = false;
    last_pressed[key] = false;
  }
  for (int i=0;i<switch_count;i++) {
    pinMode(switches[i], INPUT);
  }
  Serial.begin(31250);
};

// the loop routine runs over and over again forever:
void loop() {
  // compute all key input
  changed = false;
  for (int key=0;key<key_count;key++) {
    cur_pressed[key] = digitalRead(keys[key]);
    changed = changed || cur_pressed[key] != last_pressed[key];
  }
  if (changed) {
    command = computeCommands(cur_pressed);
    if (!command) {
      for (int key=0;key<key_count;key++) {
	computeKeyInput(key);
      }
    }
    for (int key=0;key<key_count;key++) {
      last_pressed[key] = cur_pressed[key];
    }
  }
  // read volume
  new_vol = 0x7f - (int) (((analogRead(vol_pin)) / 1023.0) * 0x7f);
  if (new_vol != vol) {
    vol = new_vol;
    sendMIDI(CONTROL_CHANGE | channel, 0x07, vol);
  }
  // read 2nd poti
  new_wheel_val = 0x7f - (int) (((analogRead(wheel_pin)) / 1023.0) * 0x7f);
  if (new_wheel_val != wheel_val) {
    wheel_val = new_wheel_val;
    sendMIDI(CONTROL_CHANGE | channel, 0x09, wheel_val);
  }
  if (!command) {
    for (int i=0;i<switch_count;i++) {
      // read value (convert HIGH to LOW and the other way round)
      computeSwitchInput(i, digitalRead(switches[i]) == LOW ? HIGH : LOW);
    }
  }
};
