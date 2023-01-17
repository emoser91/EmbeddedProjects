/* This Project gets an Atmega328p chip working on a breadboard

  The Atmega328p chip in the Arduino Uno already has a bootloader so we do not have to worry about that.
  If you had a bare chip there is documentation in the folder on how to flash the bootloader to it with
  the use of a second Arduino Uno as a programmer.

  You can just program the Atmega328p chip sitting in the Arduino Uno board with this program and then
  remove it and place it on the breadboard and wire it up as shown in the wiring digram pictures
  in the documents folder of this project. Alternatively, you can also program the chip already wired
  up on the breadboard with a second Arduino Uno which is shown in the Arduino Document in the folder.

  It is imporatant to use the right load capacitors with you osillator crystal and there are documents
  included that show how to go about that calculation with a formula.

  As for powering the Atmega328p chip, you can either use a second Arduino Uno and run power and Ground
  wires from it or you can use the Power Supply breakout board that comes with the Sensor kit that can
  accept multiple different power connectors. The barrel DC connector from the Arduino power adaptor
  might work well.

  It is also a good idea to use a voltage regulator circuit to ensure safe voltage reaches the
  Atmega328p chip.

  The Atmega328p chip in the Arduino Uno also has modified FUSE BITS to set it up to run an external
  clock source as well as a few other settings. The Atmega328p has an internal 8Mhz clock that can be
  set by setting the FUSE BITS. I guess the internal 8Mhz clock has a horrible rate of error of 10%
  so it isnt really useful for anything that needs accurate timing. You will need to use the Arduino IDE
  to set the fuse bits as that what all the examples use.

  Steps to gettting Arduino IDE Setup programming Atmega328p chip to use 8Mhz Internal Clock:
  1. Go to File/Examples/11.ArduinoISP and click to open the example. Upload the program an Arduino Uno with the example code
     and it is now an ISP programmer. We are using SPI serial protocol to program.
  2. Open a new Sketch
  3. Go to File/Preferences and in window paste the following into the "Additional Board Manager URL" section and click OK
     https://mcudude.github.io/MiniCore/package_MCUdude_MiniCore_index.json
  4. Go to Tool/Board:/Board Manager search for Minicore and install
  5. Go to Tools/Board and select Minicore atmega328
     You will now see all the options to change Fuse Bits to change the clock and other settings
  7. Go to Tools/Clock and select INTERNAL 8Mhz (MAKE SURE IT IS INTERNAL!!!)
     If you were to select an external by accident, you would have to set up an external clock circuit to even program it again to change it back
  8. Go to Tools/COM Port and select the port for the Arduino Uno
  9. Go to Tools/Programmer and select Arduino as ISP
  10. Wire project up and then click to BURN BOOTLOADER to change the Fuse bits
  11. You should be able to click Upload to upload your code to the atmega328p now running at 8Mhz
  Note: If the bootload burning fails then you will most likely need to have an 16Mhz external clock setup for your breadboard arduino
        or you could just leave the chip in the Arduino so it gets its 16Mhz clock still use the second arduino as the ISP Programmer
        and then pull the chip out once bootloader is burned.
  Note: There is an old wiring option and new wiring option to choose from in the Arduino ISP code for how you want to connect two arduinos
  Note: You can look at the Arduino Document link below for this process. I used the Minicore but Arduino uses a different file. Its the
        same procedure pretty much with more setting in the Minicore.
  
  Arduino as ISP Wiring:
  See Arduino Document Link below and follow.

  Atmega328p Wiring:
  See Arduino and Wiring documents listed below.

  Link to Arduino Document: https://docs.arduino.cc/built-in-examples/arduino-isp/ArduinoToBreadboard
  Link to Wiring Setup: https://www.instructables.com/How-to-Build-an-Arduino-Uno-on-a-BreadBoard/

  Original Comments:

  Blink
  
  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}
