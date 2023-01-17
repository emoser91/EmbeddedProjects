/* ESP32BlinkLED Arduino Sketch File
 * 
 * This program will blink the Blue LED on the ESP32 Dev Module Board
 * 
 * To Get Arduino IDE Setup and connected to ESP32 board follow the below link
 * https://m.media-amazon.com/images/I/91bM3wW2OTL.pdf
 * Note: I skipped the driver install and it worked just fine. 
 * 
 * Steps to program ESP32 Board
 * 1. Conenct ESP32 Via USB to the computer
 * 2. Open Device Manager App to check what COM port the ESP32 is connected to
 * 3. Go under Tools/Select Board/ESP32 Arduino/ESP Dev Module
 * 4. Change upload rate to 115200 just to be a little safer while uploading
 * 5. Change PORT to the COM port you found on device manager
 * 6. Dont worry about Programmer saying it is missing, it is not needed for this
 * 7. Click to upload Sketch to the ESP32
 * 8. Keep an eye on the output text and you will see it say Connecting....._____
 * 9. When you see it say connecting, you will hold down the Boot Button (button at botton of board near 100 silk screening, NOT THE EN BUTTON)
 * 10. The upload will continue and you can let go of the Boot Button
 * 11. You should now see the Blue LED blinking on the ESP32 Board if everything worked correctly
 */


int LED_BUILTIN = 2;

void setup() 
{
  pinMode (LED_BUILTIN, OUTPUT);
}
void loop() 
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
