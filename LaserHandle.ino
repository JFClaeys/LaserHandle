#include <SPI.h>
#include <SD.h>
#include "alphas.h"

/*following are pin defitnition for the 74HC595 drive
 * running ShiftRegister 74HC595 
 * each pin has a 47k resistor connected to the base of a 2N2222 transistor
 * 2N2222 collector is connected to the negative leg of a laser diod
 * 2N2222 emitter is connected to ground
 * Following pinout would eventually change shen the CD card reader (on SPI) is mastered, since the current pinout will be used for SPI
   SR_LATCH_PIN : ST_CP - pin 12  store register clock pin  -  PB3 = STM32-pin 19  /  uno-pin 3
   SR_CLOCK_PIN : SH_CP - pin 11  shift register clock pin  -  PB4 = STM32-pin 20  /  uno-pin 4
   SR_DATA_PIN  : DS    - pin 14  serial data input         -  PB5 = STM32-pin 21  /  uno-pin 5
                  OE    - pin 13  Output enabled  is connected to ground 
*/

#define FILE_NAME "sequence.bin"
#define SR_LATCH_PIN PB3
#define SR_CLOCK_PIN PB4
#define SR_DATA_PIN  PB5 
// #define LED_PIN PB5(13)  one cannot use a LED while doing SPI, at least on arduino uno, as SCK is the same as LED

File myFile;
byte SequenceSize = 0;
byte SequenceRepeat = 0;
int SequenceDelay = 0;
byte Sequence[10];  // might need to change to dynamic array...
byte looping = 0;

/*
 * record_type registers [5] = { { 1, 2, 3 }, { 1, 2, 3 }, { 1, 2, 3 }, { 1, 2, 3 }, { 1, 2, 3 } };
 * 
 * or
 * typedef struct
  {
      int one;
      int two;
      int three;
  }  record_type;

  record_type record[8];
  record[0].one = 1;
 * 
*/

/*typedef struct {
  byte aData;
  byte aDelay;
} TSeqElem;
*/

void WriteShiftRegisterValue( byte value ) {
  digitalWrite(SR_LATCH_PIN, LOW);
  shiftOut(SR_DATA_PIN, SR_CLOCK_PIN, MSBFIRST, value);
  digitalWrite(SR_LATCH_PIN, HIGH);
}

/* the following functions are helpers to allow using Serial functions in an easy manner
   BUT also to allow usage of my stm32 via ST-link which do not provide serial communications.
   Given the USB stm32 seems problematic with my units, ST-LINK is MY prefered way*/

/* define prototype for using default parameter LineFeed*/   
void PrintSerialLog( int aText, bool LineFeed = false );
void PrintSerialLog( String aText, bool LineFeed = false);
  
void PrintSerialLog( String aText, bool LineFeed ) {
  #ifndef  STM32_CORE_VERSION  
  if (LineFeed) {
    Serial.println(aText);
  } else {
    Serial.print(aText);
  }
  #endif
}

void PrintSerialLog( int aText, bool LineFeed ) {
  #ifndef  STM32_CORE_VERSION  
  if (LineFeed) {
    Serial.println(aText);
  } else {
    Serial.print(aText);
  }
  #endif
}

/* Function in regards to reading the SD card*/
void ReadVersion() {
  /* as we are not yet interesting in doing changes based on different versions of the bin,
     we just read it over */
  PrintSerialLog("Version : ");
  PrintSerialLog(myFile.read());  // major version
  PrintSerialLog(".");
  PrintSerialLog(myFile.read());// minor version  
}


void setup() {

  /* initialization of the 74HC595 */
  pinMode(SR_LATCH_PIN, OUTPUT);
  pinMode(SR_CLOCK_PIN, OUTPUT);
  pinMode(SR_DATA_PIN, OUTPUT);
  
  // Open serial communications and wait for port to open:
  #ifndef  STM32_CORE_VERSION  
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  #endif

  PrintSerialLog("Init SD card", true);
  
  if (!SD.begin(PIN_SPI_SS)) {
    PrintSerialLog("init failed!", true);
    while (1);
  }
  PrintSerialLog("init done.", true); 

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open(FILE_NAME, FILE_READ);

  // if the file opened okay, write to it:
  if (myFile) {
    PrintSerialLog("Reading file ");
    PrintSerialLog(FILE_NAME, true);      

  /*
     00 01 05 00 02 04 08 10 03 E8 04
     -- -- -- -------------- ----- --
      |  | |   |              |     +- repetition
      |  | |   |              +------- delai (1000 ms)
      |  | |   +---------------------- sequence 
      |  | +-------------------------- nombre de bytes de sequence
      |  +---------------------------- version mineure
      +------------------------------- version majeure
  */
    
    while (myFile.available()) {            
      ReadVersion();

      SequenceSize = myFile.read(); // nb bytes to read      
      PrintSerialLog("Nb Sequences : ");
      PrintSerialLog(SequenceSize, true);

      PrintSerialLog("Bytes read : ");
      PrintSerialLog( myFile.readBytes(Sequence, SequenceSize));
      PrintSerialLog( " -> " );
      
      for (int i = 0; i < SequenceSize; i++ ) {
        PrintSerialLog(Sequence[i]);
        PrintSerialLog(" ");
      }
      PrintSerialLog("", true);
      
      SequenceDelay = (myFile.read() << 8) + myFile.read();  // shifted by 8, with the low part added to it
      
      PrintSerialLog("delay (ms) : ");
      PrintSerialLog(SequenceDelay, true);

      SequenceRepeat = myFile.read();
      
      PrintSerialLog("repeats : ");
      PrintSerialLog(SequenceRepeat, true);
    }

    // close the file:
    myFile.close();
    
    PrintSerialLog("reading done.", true);
    PrintSerialLog("-----", true);
  } else {
    // if the file didn't open, print an error:
    PrintSerialLog("error opening file", true);
  }
  
  for (int i = 1; i <= SequenceRepeat; i++) {
    PrintSerialLog("Sequence ");
    PrintSerialLog(i);
    PrintSerialLog( " = "); 
    
    for (int j = 0; j < SequenceSize; j++) {
      PrintSerialLog(Sequence[j]);
      PrintSerialLog(" ");
    }
    PrintSerialLog("", true);
  }

  PrintSerialLog("-----", true); 
}
  
void loop() {
  PrintSerialLog("current loop : ");
  PrintSerialLog(looping, true);
  PrintSerialLog("writing sending pulse", true);
  WriteShiftRegisterValue( Sequence[looping] );  // just shift one laser after the other

  delay(SequenceDelay);  
  PrintSerialLog("delay sent", true);  

  looping ++;
  if (looping >= SequenceSize) {
    looping = 0;
    PrintSerialLog("resetting loop", true);
  }  
}
