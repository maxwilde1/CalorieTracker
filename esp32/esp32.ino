//esp32.ino .ino means that the file is an Arduino sketch file. The .ino extension is 
//used for files that contain code written for the Arduino platform, which can be 
//compiled and uploaded to Arduino-compatible boards, including the ESP32.

//Arduino uses c++ 


 // setup() runs ONCE when the ESP32 starts
void setup() {

  // Start communication between the ESP32 and computer via usb
  // 115200 is the communication speed (baud rate)
  //serial communication lets the esp32 send messages to my computer 
  //serial is an object provided by arduino, 
  // .begin() is a method belonging to an object that initialises the serial communication 
  Serial.begin(115200); //115200 bits transmitted per second, common for esp32 debugging

}

// loop() runs REPEATEDLY while the ESP32 is powered on
void loop() {

  // Send this message to the computer
  Serial.println("Hello from CalorieSense!");

  // Wait 1000 milliseconds (1 second)
  delay(1000);

}

