// Badge code for the osquery-sao

// Receives control codes over i2c
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

#include <TinyWireS.h>
// The default buffer size, Can't recall the scope of defines right now
#ifndef TWI_RX_BUFFER_SIZE
#define TWI_RX_BUFFER_SIZE (16)
#endif

// We want to remember the last state of our config, so we use the EEPROM
// library to store data between startups
//  Used the examples available in the Arduino IDE for the EEPROM segments
#include <EEPROM.h>
#define EEPROM_REVISION                                                        \
  0x0001 // used to version the data so we can change the array below and have
         // it override whats in EEPROM
#define EEPROM_OFFSET 0x00 // if we want to move the storage around, change this

#define I2C_SLAVE_ADDRESS 0x42

volatile byte mode = 0;

// This is purple by default
// Change me to change the color
volatile int curr_color = 3;

#define LED_PIN PB1
#define NUMPIXELS 8
#define DELAYVAL 4000

const int num_colors = 7;
int colors[num_colors][3] = {{1, 0, 0}, // Red
                             {0, 1, 0}, // Green
                             {0, 0, 1}, // Blue
                             {1, 0, 1}, // Purple?
                             {1, 1, 1}, // White?
                             {0, 1, 1}, // ... Yellow?
                             {1, 1, 0}};

Adafruit_NeoPixel pixels(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

// volatile uint8_t i2c_regs[] = {
//     0x00, // 0: Mode: 0=off, 1=solid, 2=fade, 3=rainbow loop, etc...
//     0x00,
//     0x00,
//     0x00,
//     0x17, // 5: Should be 0x17 most of the time, changing it triggers special
//     //      actions and returns to 0x17(arbitrary choice - used to help see
//     //      the end of the options list before looping)
// };

// Tracks the current register pointer position
// volatile byte reg_position = 0;
// const byte reg_size = sizeof(i2c_regs);

/**
   The I2C data received handler

   This needs to complete before the next incoming transaction (start, data,
   restart/stop) on the bus does so be quick, set flags for long running tasks
   to be called from the mainloop instead of running them directly,
*/
void receiveEvent(uint8_t howMany) {
  if (howMany < 1) {
    // Sanity-check
    return;
  }
  if (howMany > TWI_RX_BUFFER_SIZE) {
    // Also insane number
    return;
  }

  // reg_position = TinyWireS.receive();
  mode = TinyWireS.receive();
  howMany--;
  if (!howMany) {
    // This write was only to set the buffer for next read
    return;
  }
  while (howMany--) {
    // i2c_regs[reg_position] = TinyWireS.receive();
    mode = TinyWireS.receive();
    // reg_position++;
    // if (reg_position >= reg_size) {
    //   reg_position = 0;
    // }
  }
}

// Forwad decl :P
void color_all_pixels(int r, int g, int b);
void solid(int brightness, int cind);
void all_off();
void breath(int speed);
void rainbow();
void alternating_blink(size_t speed);
void run(size_t speed);

void setup() {
  pixels.begin();
  pixels.setBrightness(50);
  pixels.show();

  // Start us off with solid purple
  solid(75, 3);

  // i2c_regs[0] = 0x00;
  mode = 0;

  // Disable global interrupts before initialization of the Timer
  // noInterrupts(); // cli();

  TinyWireS.begin(I2C_SLAVE_ADDRESS);
  TinyWireS.onReceive(receiveEvent);
  // TinyWireS.onRequest(requestEvent);

  // Initialize Timer1
  //   We use Timer1 because Timer0 is setup in the arduino libraries to run the
  //   millis() function Below we set timer1 to call an ISR(interrupt service
  //   routine) whenever it overflows value.
  //    This occurs every 256 timer ticks, because this is an 8 bit timer(see
  //    datasheet). Every cpu cycle at 8MHz is 125ns, times 256 to overflow has
  //    us running the ISR every 32us,
  //     or 31.25KHz. We use 256 cycles of the ISR to do one PWM cycle, so the
  //     LEDs flicker around every 8.2ms, or 122Hz, enough to keep your eyes
  //     fooled, but probably not for PoV use.
  // TCCR1 = 0; // set entire TCCR1 register to 0, removes any existing values
  // TIMSK = (1 << TOIE1); // enable Timer1 overflow interrupt, calls
  // ISR(TIMER1_OVF_vect) below
  // TCCR1 |= (1 << CS10); // Set CS10 bit so timer runs at the cpu clock speed

  // Read the EEPROM values where appropriate
  // unsigned int ver;
  // EEPROM.get(
  //     EEPROM_OFFSET,
  //     ver); // read version stored at the beginning of the EEPROM(w/ offset)
  // if (ver == EEPROM_REVISION) {
  //   EEPROM.get(EEPROM_OFFSET + 2,
  //              i2c_regs); // read the settings if the version has not changed
  //   // offset by +2 to give room for REVISION
  // } else {
  //   EEPROM.put(EEPROM_OFFSET, EEPROM_REVISION); // store the current version
  //   EEPROM.put(EEPROM_OFFSET + 2,
  //              i2c_regs); // and store the array as it currently exists in
  //              code
  // }

  // Re-enable interrupts before we enter the main loop.
  // interrupts();
}

void loop() {
  TinyWireS_stop_check();

  // switch (i2c_regs[0]) {
  switch (mode) {
  case 0x00: {
    // Breath Purple by default
    breath(10);
    break;
  }
  case 0x01: {
    // Solid Red
    solid(75, 0);
    break;
  }
  case 0x02: {
    // Solid Green
    solid(75, 1);
    break;
  }
  case 0x03: {
    // Solid Blue
    solid(75, 2);
    break;
  }
  case 0x04: {
    run(200);
    break;
  }
  default: {
    breath(20);
  }
  }

  tws_delay(DELAYVAL);
}

/****** Start Coloring Functions *******/
void color_all_pixels(int r, int g, int b) {
  for (int j = 0; j < NUMPIXELS; j++) {
    pixels.setPixelColor(j, pixels.Color(r, g, b));
  }
  pixels.show();
}

void all_off() {
  pixels.clear();
  pixels.show();
  tws_delay(DELAYVAL);
}

void breath(int speed) {
  pixels.clear();
  pixels.show();

  int curr_mode = mode;

  // Fade up
  for (int i = 0; i < 200; i++) {
    color_all_pixels(i * colors[curr_color][0], i * colors[curr_color][1],
                     i * colors[curr_color][2]);
    tws_delay(speed);
    // Mode changed during breath, break out
    if (curr_mode != mode) {
      return;
    }
  }

  // Fade down
  for (int i = 200; i > 0; i--) {
    color_all_pixels(i * colors[curr_color][0], i * colors[curr_color][1],
                     i * colors[curr_color][2]);
    tws_delay(speed);
    // Mode changed during breath, break out
    if (curr_mode != mode) {
      return;
    }
  }
}

void rainbow() {
  // pixels.clear();
  pixels.setBrightness(75);
  pixels.rainbow();
  pixels.show();
}

void alternating_blink(size_t speed) {

  pixels.clear();

  int curr_mode = mode;

  int bright = 75;
  size_t numsets = 2;

  for (size_t j = 0; j < numsets; j++) {
    for (size_t i = 0; i < NUMPIXELS; i++) {
      if (i == j % 2) {
        pixels.setPixelColor(i, pixels.Color(255 * colors[curr_color][0],
                                             255 * colors[curr_color][1],
                                             255 * colors[curr_color][2]));
      } else {
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
      }
      // Mode changed during breath, break out
      if (curr_mode != mode) {
        return;
      }
    }
    pixels.setBrightness(bright);
    pixels.show();
    tws_delay(speed);
  }
}

void solid(int brightness, int cind) {
  color_all_pixels(brightness * colors[cind][0], brightness * colors[cind][1],
                   brightness * colors[cind][2]);
  tws_delay(DELAYVAL);
}

// Speed in this case is the number of ms to wait between
// light shows, so lower = faster runs
void run(size_t speed) {
  pixels.clear();
  pixels.show();

  int curr_mode = mode;

  pixels.setBrightness(75);

  for (int j = 0; j < NUMPIXELS; j++) {
    pixels.setPixelColor(j, pixels.Color(255 * colors[curr_color][0],
                                         255 * colors[curr_color][1],
                                         255 * colors[curr_color][2]));
    pixels.show();
    tws_delay(speed);
    pixels.clear();

    // Mode changed during breath, break out
    if (curr_mode != mode) {
      return;
    }
  }
}
/******* END COLOR FUNCTIONS ********/