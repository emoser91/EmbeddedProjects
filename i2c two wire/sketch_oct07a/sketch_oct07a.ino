#include <SPI.h>
#define SS 10           // The Slave/Chip select pin
#define CHIP 0x40       // The chip's address (set by pins 4 & 5)
#define IO_DIR_REG 0x00 // The Input/Output Register
#define GPIO_REG 0x09   // The GPIO Register

#define IODIR 0x00           // MCP23008 I/O Direction Register
#define GPIO  0x09           // MCP23008 General Purpose I/O Register
#define OLAT  0x0A           // MCP23008 Output Latch Register

#define LCD_CLR             0x01  // clear display
#define LCD_HOME      0x02  // return to home position

#define LCD_CTRL_RS    1 // bit 1 of data word
#define LCD_CTRL_E    2 // bit 2 of data word

void SPIWrite(byte spiRegister, byte value){
  // This function sends data to the chip
  // It's a 5 step process
  // 1) Pull the Slave/Chip select LOW
  digitalWrite(SS,LOW);
  // 2) Send the chip's address to the chip
  SPI.transfer(CHIP);
  // 3) Send the register to the chip
  SPI.transfer(spiRegister);
  // 4) Send the value to the chip
  SPI.transfer(value);
  //5) Pull the Slave/Chip select HIGH
  digitalWrite(SS,HIGH);
}

void LcdCommandWrite(uint8_t cmd)
{
  char temp_reg = 0x00;
  /* Read MCP23008 Output Latch Register. The OLAT register provides access to the output latches. 
  A read from this register results in a read of the OLAT and not the port itself. 
  A write to this register modifies the output latches that modifies the pins configured as outputs. */
  // temp_reg = Read_MCP23008(OLAT);                //Read MCP23008 Output Latch Register

  /* Lower RS to send commands */
  temp_reg &= ~(1 << (LCD_CTRL_RS));          //Raise RS bit for Data
  SPIWrite(GPIO,temp_reg);
  _delay_ms(1);

  /* Write Enable High, Write Data first half of 8bit data, Write Enable Low */
  temp_reg |= (1 << (LCD_CTRL_E));          //Raise Enable bit
  SPIWrite(GPIO,temp_reg);

  temp_reg=(temp_reg&0x87)|((cmd>>1)&0x78);     //Mask for the Data bits and set to upper nibble (4bits)
  SPIWrite(GPIO,temp_reg);

  temp_reg &= ~(1 << (LCD_CTRL_E));         //Lower Enable bit
  SPIWrite(GPIO,temp_reg);
  _delay_ms(2);                    //Wait 2ms so display uploads data

  /* Write Enable High, Write Data second half of 8bit data, Write Enable Low */
  temp_reg |= (1 << (LCD_CTRL_E));          //Raise Enable bit
  SPIWrite(GPIO,temp_reg);

  temp_reg=(temp_reg & 0x87) | ((cmd<<3) & 0x78);   //Mask for the Data bits and set to lower nibble (4bits)
  SPIWrite(GPIO,temp_reg);

  temp_reg &= ~(1 << (LCD_CTRL_E));         //Lower Enable bit
  SPIWrite(GPIO,temp_reg);
  _delay_ms(2);                    //Wait 2ms so display uploads data
}

void LcdDataWrite(uint8_t data)
{
  char temp_reg = 0x00;

  // temp_reg=Read_MCP23008(OLAT);            //Read MCP23008 Output Latch Register

  /* Raise RS bit to send Data */
  temp_reg |= (1 << (LCD_CTRL_RS));         //Raise RS bit for Data
  SPIWrite(GPIO,temp_reg);
  _delay_ms(1);

  /* Write Enable High, Write Data first half of 8bit data, Write Enable Low */
  temp_reg |= (1 << (LCD_CTRL_E));          //Raise Enable bit
  SPIWrite(GPIO,temp_reg);
  temp_reg=(temp_reg&0x87)|((data>>1)&0x78);      //Mask for the Data bits and set to upper nibble (4bits)
  SPIWrite(GPIO,temp_reg);
  temp_reg &= ~(1 << (LCD_CTRL_E));         //Lower Enable bit
  SPIWrite(GPIO,temp_reg);
  _delay_ms(1);                    //Wait 1ms so display uploads data

  /* Write Enable High, Write Data second half of 8bit data, Write Enable Low */
  temp_reg |= (1 << (LCD_CTRL_E));          //Raise Enable bit
  SPIWrite(GPIO,temp_reg);
  temp_reg=(temp_reg & 0x87) | ((data<<3) & 0x78);  // send lower nibble
  SPIWrite(GPIO,temp_reg);
  temp_reg &= ~(1 << (LCD_CTRL_E));         //Lower Enable bit
  SPIWrite(GPIO,temp_reg);
  _delay_ms(1);                    //Wait 1ms so display uploads data
}

void LcdPortInit(void)
{
  delay(100);
  SPI.begin();
  SPIWrite(IO_DIR_REG,0x00); // Set all pins to OUTPUT
  SPIWrite(GPIO_REG,0x00);   // Set all pins LOW
}

void LcdInit(void)
{
  _delay_ms(100);             // wait 100ms for the LCD to come out of reset
  LcdPortInit();             // initalize the port pins and init SPI subsystem
  LcdCommandWrite(0x33);     // set DDRAM address: init LCD to 4bit interface
  LcdCommandWrite(0x32);     // set DDRAM address: init LCD to 4bit interface
  LcdCommandWrite(0x28);     // set DDRAM address: set two-line display
  LcdCommandWrite(0x0C);     // Display On/Off: display on, curser off, curser blink off
  LcdCommandWrite(0x06);     // Entry Mode Set: increment DD RAM after each write/read, increment curser right after each write/read
  LcdCommandWrite(LCD_CLR);  // Clear Display: 0x01 clear display and returns curser to the home position
  LcdCommandWrite(LCD_HOME); // Return Home: 0x02 return home
}



void setup() {
  pinMode(SS,OUTPUT);
  digitalWrite(SS,HIGH);
  

  LcdInit();
  LcdDataWrite(0x48); //0x48=Ascii letter "H"
  LcdDataWrite(0x49); //0x48=Ascii letter "I"/
 }

void loop() {
  //do nothing
}
