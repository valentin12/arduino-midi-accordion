// MIDI commands
const int NOTE_ON = 0x90;
const int NOTE_OFF = 0x80;
const int CONTROL_CHANGE = 0xB0;
const int PROGRAM_CHANGE = 0xC0;
const int PITCH_BEND_CHANGE = 0xE0;

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
const boolean COMMAND_PROGRAM_UP[] = {
  false, false, false, false, true, false, false, false, false, false, false, false, false,
  true, false, false, false, true, false, false, true, false, false, false, true, false};
const boolean COMMAND_PROGRAM_DOWN[] = {
  false, false, true, false, false, false, false, false, false, false, false, false, false,
  true, false, false, false, true, false, false, true, false, false, false, true, false};

// 26 keys
const int key_count = 26;
// Send on MIDI channel 1
int channel = 1;
// Program 1: Piano
int program = 0;

// poti pins
int vol_pin = 0;
int wheel_pin = 1;

int new_wheel_val = 0;
int wheel_val = 0x40;
boolean wheel_dir = false;
int new_vol = 0;
int vol = 0x45;
boolean vol_dir = false;

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

const int command_count = 5;
FunctionArray command_functions[command_count];
boolean commands[command_count][key_count];

int getNote(const int base, const int add_octaves, const int note) {
  return base + (octave + add_octaves) * 12 + note;
};

void sendMIDI(const int cmd, const int note, const int velocity) {
  Serial1.write(cmd);
  Serial1.write(note);
  Serial1.write(velocity);
  Serial.write(cmd);
  Serial.write(note);
  Serial.write(velocity);
};

void sendShortMIDI(const int cmd, const int val) {
  Serial1.write(cmd);
  Serial1.write(val);
  Serial.write(cmd);
  Serial.write(val);
}

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

void commandProgramUp() {
  if (program >= 0x7f)
    program = 0;
  else
    program++;
  sendShortMIDI(PROGRAM_CHANGE | channel, program);
};

void commandProgramDown() {
  if (program <= 0)
    program = 0x7f;
  else
    program--;
  sendShortMIDI(PROGRAM_CHANGE | channel, program);
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
  for (int key=0;key<key_count;key++) commands[3][key] = COMMAND_PROGRAM_UP[key];
  for (int key=0;key<key_count;key++) commands[4][key] = COMMAND_PROGRAM_DOWN[key];
  command_functions[0] = commandReset;
  command_functions[1] = commandOctaveUp;
  command_functions[2] = commandOctaveDown;
  command_functions[3] = commandProgramUp;
  command_functions[4] = commandProgramDown;
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
  // Setup Serial1 with the standard MIDI baud rate of 31250
  // to get MIDI on TX1 (pin 18)
  Serial1.begin(31250);
  // Setup Serial (TX0 and USB) with the baudrate 9600 to be able to use
  // an Serial to MIDI converter on a PC
  Serial.begin(9600);
  sendMIDI(CONTROL_CHANGE | channel, 0x07, vol);
  sendShortMIDI(PROGRAM_CHANGE, program);
};

// the loop routine runs over and over again forever:
void loop() {
  // compute all key input
  changed = false;
  for (int key=0;key<key_count;key++) {
    cur_pressed[key] = digitalRead(keys[key]);
    changed = changed || (cur_pressed[key] != last_pressed[key]);
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
  // read volume, check if diference > 1 if direction changed to prevent
  // switching between two values all the time
  new_vol = 0x7f - (int) (((analogRead(vol_pin)) / 1023.0) * 0x7f);
  if (new_vol != vol) {
    if (new_vol > vol) {
      if (vol_dir || new_vol - vol > 1) {
	vol_dir = true;
	vol = new_vol;
	sendMIDI(CONTROL_CHANGE | channel, 0x07, vol);
      }
    }
    else {
      if (!vol_dir || vol - new_vol > 1) {
	vol_dir = false;
	vol = new_vol;
	sendMIDI(CONTROL_CHANGE | channel, 0x07, vol);
      }
    }
  }
  // read 2nd poti
  new_wheel_val = 0x7f - (int) (((analogRead(wheel_pin)) / 1023.0) * 0x7f);
  if (new_wheel_val != wheel_val) {
    if (new_wheel_val > wheel_val) {
      if (wheel_dir || new_wheel_val - wheel_val > 1) {
	wheel_dir = true;
	wheel_val = new_wheel_val;
	sendMIDI(PITCH_BEND_CHANGE | channel, 0, wheel_val);
      }
    }
    else {
      if (!wheel_dir || wheel_val - new_wheel_val > 1) {
	wheel_dir = false;
	wheel_val = new_wheel_val;
	sendMIDI(PITCH_BEND_CHANGE | channel, 0, wheel_val);
      }
    }
  }
  if (!command) {
    for (int i=0;i<switch_count;i++) {
      // read value (convert HIGH to LOW and the other way round)
      computeSwitchInput(i, digitalRead(switches[i]) == LOW ? HIGH : LOW);
    }
  }
};
