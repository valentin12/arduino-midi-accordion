/*
 Arduino Accordion Firmware
 Copyright (C) 2015 Valentin Pratz

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along
 with this program; if not, write to the Free Software Foundation, Inc.,
 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <LiquidCrystal.h>
#include <BMP280.h>

LiquidCrystal lcd(22, 24, 34, 32, 30, 28);
BMP280 bmp;

const char PROGRAMS[][27] = {
  "1 Acoustic Grand Piano",
  "2 Bright Acoustic Piano",
  "3 Electric Grand Piano",
  "4 Honky-tonk Piano",
  "5 Electric Piano 1",
  "6 Electric Piano 2",
  "7 Harpsichord",
  "8 Clavinet",
  "9 Celesta",
  "10 Glockenspiel",
  "11 Music Box",
  "12 Vibraphone",
  "13 Marimba",
  "14 Xylophone",
  "15 Tubular Bells",
  "16 Dulcimer",
  "17 Drawbar Organ",
  "18 Percussive Organ",
  "19 Rock Organ",
  "20 Church Organ",
  "21 Reed Organ",
  "22 Accordion",
  "23 Harmonica",
  "24 Tango Accordion",
  "25 Acoustic Guitar (nylon)",
  "26 Acoustic Guitar (steel)",
  "27 Electric Guitar (jazz)",
  "28 Electric Guitar (clean)",
  "29 Electric Guitar (muted)",
  "30 Overdriven Guitar",
  "31 Distortion Guitar",
  "32 Guitar harmonics",
  "33 Acoustic Bass",
  "34 Electric Bass (finger)",
  "35 Electric Bass (pick)",
  "36 Fretless Bass",
  "37 Slap Bass 1",
  "38 Slap Bass 2",
  "39 Synth Bass 1",
  "40 Synth Bass 2",
  "41 Violin",
  "42 Viola",
  "43 Cello",
  "44 Contrabass",
  "45 Tremolo Strings",
  "46 Pizzicato Strings",
  "47 Orchestral Harp",
  "48 Timpani",
  "49 String Ensemble 1",
  "50 String Ensemble 2",
  "51 Synth Strings 1",
  "52 Synth Strings 2",
  "53 Choir Aahs",
  "54 Voice Oohs",
  "55 Synth Voice",
  "56 Orchestra Hit",
  "57 Trumpet",
  "58 Trombone",
  "59 Tuba",
  "60 Muted Trumpet",
  "61 French Horn",
  "62 Brass Section",
  "63 Synth Brass 1",
  "64 Synth Brass 2",
  "65 Soprano Sax",
  "66 Alto Sax",
  "67 Tenor Sax",
  "68 Baritone Sax",
  "69 Oboe",
  "70 English Horn",
  "71 Bassoon",
  "72 Clarinet",
  "73 Piccolo",
  "74 Flute",
  "75 Recorder",
  "76 Pan Flute",
  "77 Blown Bottle",
  "78 Shakuhachi",
  "79 Whistle",
  "80 Ocarina",
  "81 Lead 1 (square)",
  "82 Lead 2 (sawtooth)",
  "83 Lead 3 (calliope)",
  "84 Lead 4 (chiff)",
  "85 Lead 5 (charang)",
  "86 Lead 6 (voice)",
  "87 Lead 7 (fifths)",
  "88 Lead 8 (bass + lead)",
  "89 Pad 1 (new age)",
  "90 Pad 2 (warm)",
  "91 Pad 3 (polysynth)",
  "92 Pad 4 (choir)",
  "93 Pad 5 (bowed)",
  "94 Pad 6 (metallic)",
  "95 Pad 7 (halo)",
  "96 Pad 8 (sweep)",
  "97 FX 1 (rain)",
  "98 FX 2 (soundtrack)",
  "99 FX 3 (crystal)",
  "100 FX 4 (atmosphere)",
  "101 FX 5 (brightness)",
  "102 FX 6 (goblins)",
  "103 FX 7 (echoes)",
  "104 FX 8 (sci-fi)",
  "105 Sitar",
  "106 Banjo",
  "107 Shamisen",
  "108 Koto",
  "109 Kalimba",
  "110 Bag pipe",
  "111 Fiddle",
  "112 Shanai",
  "113 Tinkle Bell",
  "114 Agogo",
  "115 Steel Drums",
  "116 Woodblock",
  "117 Taiko Drum",
  "118 Melodic Tom",
  "119 Synth Drum",
  "120 Reverse Cymbal",
  "121 Guitar Fret Noise",
  "122 Breath Noise",
  "123 Seashore",
  "124 Bird Tweet",
  "125 Telephone Ring",
  "126 Helicopter",
  "127 Applause",
  "128 Gunshot"
};

// MIDI commands
const int NOTE_ON = 0x90;
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
  true, false, false, false, true, false, false, true, false, false, false, true, false};
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
// Bank 1
int bank = 0;

// poti pins
int vol_pin = 0;
// wheel = pitch
int wheel_pin = 1;

int new_wheel_val = 0;
int wheel_val = 0x40;
boolean wheel_dir = false;
int new_vol_wheel = 0;
int new_vol = 0;
int vol_wheel = 0;
int vol = 0x45;
boolean vol_dir = false;

int note_vol = 0x45;

// COMMAND MODE
int command_switch = 46;
int command_led = 13;
int command_switch_val = 0;
int command_switch_new_val = 0;
boolean command_mode = false;
int command_keys[] = {2, 3, 4, 7, 9, 11, 14, 19, 21, 5, 6};
const int command_mode_count = 11;
FunctionArray command_mode_functions[command_mode_count];

// 3 switches for 3 octaves
int switch_count = 3;
int switches[] = {48, 50, 52};
int switch_vals[] = {0, 0, 0};
int switch_new_vals[] = {0, 0, 0};

// base MIDI note b
int base_note = 0x3B;
// octave 0: normal
int octave = 0;
int keys[] = {41, 53, 39, 51, 37, 49, 35, 47, 33, 45, 31, 43,
              29, 27,  2, 25,  4,  3,  6,  5,  8,  7, 10,  9, 12, 11};
boolean last_pressed[key_count];
boolean cur_pressed[key_count];
int playing[0x7f];
boolean changed = false;
boolean is_chord_command = false;

// chord commands
const int command_count = 5;
FunctionArray command_functions[command_count];
boolean commands[command_count][key_count];

// pressure in bellow
const double BMP_MAX_DIFF = 10;
// bmp_center changes daily. commandCallibrate sets it
double bmp_center = 988.3;
double pressure, temperature;
char measure_success;
boolean use_pressure, pressure_available;

// last status byte to implement MIDI running status
int last_status_byte = 0;

int getNote(const int base, const int add_octaves, const int note) {
  return base + (octave + add_octaves) * 12 + note;
};

double getPressure() {
  measure_success = bmp.startMeasurment();
  if (measure_success!=0) {
    measure_success = bmp.getTemperatureAndPressure(temperature, pressure);
    if (measure_success!=0) {
      return pressure;
    }
  }
  return 0;
}

int getVolume(const int vol_wheel, const double pressure) {
  if (use_pressure && pressure_available) {
    int tmp_vol = (abs(bmp_center - pressure) / BMP_MAX_DIFF) * vol_wheel * 1.4;
    if (tmp_vol > 0x7f) {
      return 0x7f;
    }
    return tmp_vol;
  }
  return vol_wheel;
}

void updateDisplay() {
  // clear display
  lcd.clear();
  lcd.setCursor(0, 0);
  // write program name
  lcd.print(PROGRAMS[program]);
  lcd.setCursor(0, 1);
  // write octave difference
  lcd.print(octave);
  lcd.setCursor(4, 1);
  // write bank
  lcd.print(bank);
  lcd.setCursor(8, 1);
  // write pressure use
  if (!pressure_available)
    lcd.print("E");
  else if (use_pressure)
    lcd.print("P");
  else
    lcd.print("S");
  lcd.setCursor(10, 1);
  // write in command mode
  if (command_mode)
    lcd.print("C");
};

void sendMIDI(const int cmd, const int note, const int velocity) {
  if (cmd != last_status_byte)
    Serial1.write(cmd);
  Serial1.write(note);
  Serial1.write(velocity);
  // if (cmd != last_status_byte)
  //  Serial.write(cmd);
  // do not use running status over USB. The serial to MIDI
  // converter has problems with it
  Serial.write(cmd);
  Serial.write(note);
  Serial.write(velocity);
  last_status_byte = cmd;
};

void sendShortMIDI(const int cmd, const int val) {
  if (cmd != last_status_byte)
    Serial1.write(cmd);
  Serial1.write(val);
  // if (cmd != last_status_byte)
  //   Serial.write(cmd);
  // see above
  Serial.write(cmd)
  Serial.write(val);
  last_status_byte = cmd;
}

void commandReset() {
  sendMIDI(CONTROL_CHANGE | channel, 0x7B, 0);
  lcd.clear();
  lcd.setCursor(0, 0);
  delay(1000);
  setup();
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

void commandBankUp() {
  if (bank >= 0x7f)
    bank = 0;
  else
    bank++;
  sendMIDI(CONTROL_CHANGE | channel, 0x00, bank);
};

void commandBankDown() {
  if (bank <= 0)
    bank = 0x7f;
  else
    bank--;
  sendMIDI(CONTROL_CHANGE | channel, 0x00, bank);
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

void commandProgramUp10() {
  if (program + 10 >= 0x7f)
    program = program + 10 - 0x7f;
  else
    program += 10;
  sendShortMIDI(PROGRAM_CHANGE | channel, program);
};

void commandProgramDown10() {
  if (program - 10 <= 0)
    program = program + 0x7f - 10;
  else
    program -= 10;
  sendShortMIDI(PROGRAM_CHANGE | channel, program);
};

void commandCalibrate() {
  if (!pressure_available) return;
  bmp_center = getPressure();
  for (byte i=0;i<50;i++) {
    bmp_center = (bmp_center + getPressure()) / 2;
    delay(10);
  }
}

void commandTogglePressureUse() {
  use_pressure = !use_pressure;
}

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
	  sendMIDI(NOTE_ON | channel, getNote(base_note, l - 1, key), 0);
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
      updateDisplay();
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
	  sendMIDI(NOTE_ON | channel, getNote(base_note, switch_ind - 1, key), 0);
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

void execCommandMode(boolean cur_pressed[]) {
  for (int i=0;i<command_mode_count;i++) {
    if (cur_pressed[command_keys[i]] && !last_pressed[command_keys[i]]) {
      // iteration for octave switches
      command_mode_functions[i]();
      updateDisplay();
    }
  }
};

// the setup routine runs once when you press reset:
void setup() {
  lcd.begin(16, 2);
  lcd.noCursor();
  lcd.print("Setup");
  last_status_byte = 0;
  program = 0;
  vol = 0x45;
  use_pressure = true;
  // command chords
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
  // command mode
  command_mode_functions[0] = commandProgramDown;
  command_mode_functions[1] = commandReset;
  command_mode_functions[2] = commandProgramUp;
  command_mode_functions[3] = commandOctaveDown;
  command_mode_functions[4] = commandOctaveUp;
  command_mode_functions[5] = commandCalibrate;
  command_mode_functions[6] = commandTogglePressureUse;
  command_mode_functions[7] = commandBankDown;
  command_mode_functions[8] = commandBankUp;
  command_mode_functions[9] = commandProgramDown10;
  command_mode_functions[10] = commandProgramUp10;

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
  pinMode(command_switch, INPUT);
  pinMode(command_led, OUTPUT);
  // Setup Serial1 with the standard MIDI baud rate of 31250
  // to get MIDI on TX1 (pin 18)
  Serial1.begin(31250);
  while(!Serial1) ;
  // Setup Serial (TX0 and USB) with the baudrate 9600 to be able to use
  // an Serial to MIDI converter on a PC
  Serial.begin(115200);
  while(!Serial) ;
  pressure_available = bmp.begin() > 0;
  if (pressure_available) {
    commandCalibrate();
    bmp.setOversampling(4);
  }
  updateDisplay();
  sendMIDI(CONTROL_CHANGE | channel, 0x07, vol);
  sendShortMIDI(PROGRAM_CHANGE | channel, program);
};

// the loop routine runs over and over again forever:
void loop() {
  // compute all key input
  changed = false;
  for (int key=0;key<key_count;key++) {
    cur_pressed[key] = digitalRead(keys[key]);
    changed = changed || (cur_pressed[key] != last_pressed[key]);
  }
  command_switch_new_val = digitalRead(command_switch);
  if (command_switch_new_val != command_switch_val) {
    command_switch_val = command_switch_new_val;
    if (command_switch_new_val == HIGH) {
      digitalWrite(command_led, HIGH);
      command_mode = true;
      updateDisplay();
      sendMIDI(CONTROL_CHANGE | channel, 0x7B, 0);
    }
    else {
      digitalWrite(command_led, LOW);
      command_mode = false;
      updateDisplay();
    }
  }
  if (changed) {
    if (command_mode) {
      execCommandMode(cur_pressed);
    }
    else {
      is_chord_command = computeCommands(cur_pressed);
      if (!is_chord_command) {
	for (int key=0;key<key_count;key++) {
	  computeKeyInput(key);
	}
      }
    }
    for (int key=0;key<key_count;key++) {
      last_pressed[key] = cur_pressed[key];
    }
  }
  // read volume, check if diference > 1 if direction changed to prevent
  // switching between two values all the time
  new_vol_wheel = 0x7f - (int) (((analogRead(vol_pin)) / 1023.0) * 0x7f);
  if (new_vol_wheel != vol_wheel) {
    if (new_vol_wheel > vol_wheel) {
      if (vol_dir || new_vol_wheel - vol_wheel > 1) {
	vol_dir = true;
	vol_wheel = new_vol_wheel;
      }
    }
    else {
      if (!vol_dir || vol_wheel - new_vol_wheel > 1) {
	vol_dir = false;
	vol_wheel = new_vol_wheel;
      }
    }
  }
  new_vol = getVolume(vol_wheel, getPressure());
  if (new_vol != vol) {
    vol = new_vol;
    sendMIDI(CONTROL_CHANGE | channel, 0x07, vol);
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
  if (!is_chord_command) {
    for (int i=0;i<switch_count;i++) {
      // read value (convert HIGH to LOW and the other way round)
      computeSwitchInput(i, digitalRead(switches[i]) == LOW ? HIGH : LOW);
    }
  }
};
