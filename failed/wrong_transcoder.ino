
// Arduino code for transcoding vt100 terminal compatible serial stream to atmega8 tvterm compatible serial stream
// Assume serial input on hardware serial port and serial output using software serial
// Use large output buffer arranged as ring buffer
// Use small, 128byte input buffer
// Arrange code assuming serial output is much slower than input

#include <SoftwareSerial.h>

#define OUTPUT_BUFFER_SIZE 1024 // size of the output buffer
#define INPUT_BUFFER_SIZE 128 // size of the input buffer

SoftwareSerial outputSerial(2, 3); // RX, TX pins for software serial output

uint8_t outputBuffer[OUTPUT_BUFFER_SIZE]; // output buffer array
uint16_t outputHead = 0; // index of the next byte to write to the output buffer
uint16_t outputTail = 0; // index of the next byte to read from the output buffer

uint8_t inputBuffer[INPUT_BUFFER_SIZE]; // input buffer array
uint8_t inputHead = 0; // index of the next byte to write to the input buffer
uint8_t inputTail = 0; // index of the next byte to read from the input buffer

void setup() {
  Serial.begin(9600); // initialize hardware serial input at 9600 baud
  outputSerial.begin(2400); // initialize software serial output at 2400 baud
}

void loop() {
  readInput(); // read bytes from hardware serial input and store them in input buffer
  transcodeInput(); // transcode bytes from input buffer and store them in output buffer
  writeOutput(); // write bytes from output buffer to software serial output
}

void readInput() {
  while (Serial.available() > 0) { // while there are bytes available on hardware serial input
    uint8_t nextByte = Serial.read(); // read the next byte
    uint8_t nextHead = (inputHead + 1) % INPUT_BUFFER_SIZE; // calculate the next index for input head
    if (nextHead != inputTail) { // if the input buffer is not full
      inputBuffer[inputHead] = nextByte; // store the byte in the input buffer
      inputHead = nextHead; // update the input head index
    }
    else {
      // input buffer is full, discard the byte
    }
  }
}

void transcodeInput() {
  while (inputHead != inputTail) { // while there are bytes in the input buffer
    uint8_t nextByte = inputBuffer[inputTail]; // get the next byte from the input buffer
    uint16_t nextTail = (inputTail + 1) % INPUT_BUFFER_SIZE; // calculate the next index for input tail
    switch (nextByte) {
      case '\n': // newline character in vt100 terminal
        writeOutputByte('\r'); // write carriage return character to output buffer
        writeOutputByte('\n'); // write newline character to output buffer
        break;
      case '\b': // backspace character in vt100 terminal
        writeOutputByte(8); // write backspace character to output buffer
        break;
      case '\e': // escape character in vt100 terminal
        writeOutputByte(27); // write escape character to output buffer
        break;
      default: // any other character in vt100 terminal
        writeOutputByte(nextByte); // write the same character to output buffer
        break;
    }
    inputTail = nextTail; // update the input tail index
  }
}

void writeOutputByte(uint8_t byte) {
  uint16_t nextHead = (outputHead + 1) % OUTPUT_BUFFER_SIZE; // calculate the next index for output head
  if (nextHead != outputTail) { // if the output buffer is not full
    outputBuffer[outputHead] = byte; // store the byte in the output buffer
    outputHead = nextHead; // update the output head index
  }
  else {
    // output buffer is full, discard the byte
  }
}

void writeOutput() {
  while (outputSerial.availableForWrite() > 0 && outputHead != outputTail) { 
    // while there are bytes available for writing on software serial output and there are bytes in the output buffer 
    uint8_t nextByte = outputBuffer[outputTail]; // get the next byte from the output buffer 
    uint16_t nextTail = (outputTail + 1) % OUTPUT_BUFFER_SIZE; // calculate the next index for output tail 
    outputSerial.write(nextByte); // write the byte to software serial output 
    outputTail = nextTail; // update the output tail index
  }
}
