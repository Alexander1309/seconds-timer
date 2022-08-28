#include <Keypad.h>
#include <Adafruit_LEDBackpack.h>

const int led = 5;
int timer = 0;
int setTimerIndex = -1;
char keyFunction;
char timerData[4];
bool stateAnimateDraw = 0;
unsigned long animate_draw, timer_start = 0;

Adafruit_7segment display = Adafruit_7segment();

const byte numRows = 4;
const byte numColums = 4;

char keymap[numRows][numColums] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[numRows] = {9, 8, 7, 6};
byte colPins[numColums] = {13, 12, 11, 10};

Keypad myKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, numRows, numColums);

void setup() {
  Serial.begin(9600);
  display.begin(119);
  pinMode(led, OUTPUT);
  reset();
  render();
  animate_draw, timer_start = millis();
}

void writeNumber(int number) {
  char showNumber[4];
  int thousands = number / 1000;
  int hundreds = (number - (thousands * 1000)) / 100;
  int tens = (number - (thousands * 1000 + hundreds * 100)) / 10;
  int units = number - (thousands * 1000 + hundreds * 100 + tens * 10);

  sprintf(showNumber, "%d%d%d%d", thousands, hundreds, tens, units);
  display.println(showNumber);
  display.drawColon(true);
  display.writeDisplay();
}

void start() {
  digitalWrite(led, 0);
}

void end() {
  digitalWrite(led, 1);
}

void render() {
  writeNumber(timer);
}

void reset() {
  timer = 0;
  setTimerIndex = -1;
  for (int i = 0; i <= 4; i++) timerData[i] = {};
  display.drawColon(true);
  display.writeDisplay();
  render();
}

void animateDraw() {
  if ((millis() - animate_draw) > 100) {
    display.drawColon(stateAnimateDraw);
    display.writeDisplay();
    stateAnimateDraw = !stateAnimateDraw;
    animate_draw = millis();
  }
}

void setTimer(char keypressed) {
  if (setTimerIndex >= 0) timerData[setTimerIndex] = keypressed;
  else reset();
  timer = atoi(timerData);
  if (setTimerIndex >= 0 && timer == 0) reset();
  render();
  setTimerIndex++;
  if (setTimerIndex == 4) {
    keyFunction = 'B';
    setTimerIndex = -1;
  }
}

void controllerKeyPad() {
  char keypressed = myKeypad.getKey();
  if (keypressed != NO_KEY) {
    if (keyFunction != 'D') {
      if (keypressed == 'A' || keyFunction == 'A') {
        keyFunction = 'A';
        start();
        setTimer(keypressed);
      }
    }
    if (keypressed == 'B' && keyFunction != 'D') keyFunction = 'B';
    if (keypressed == 'C' && keyFunction != 'D') keyFunction = 'C';
    if (keypressed == 'D' && timer != 0) keyFunction = 'D';
  }
}

void controller() {
  if (keyFunction == 'A') animateDraw();
  if (keyFunction == 'B') {
    display.drawColon(true);
    display.writeDisplay();
  }
  if (keyFunction == 'C') reset();
  if (keyFunction == 'D') {
    if ((millis() - timer_start) > 1000) {
      timer--;
      render();
      if (timer == 0) {
        keyFunction = 'C';
        end();
        return;
      }
    }
  }
}

void loop() {
  controllerKeyPad();
  controller();
}
