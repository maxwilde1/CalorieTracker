//Arduino uses c++ 


//LIBRARIES
#include "esp_camera.h" //existing library for esp32 cam module 
#include <WiFi.h> //existing library for wifi connection
#include "secrets.h" //include the secrets.h file that contains the wifi credentials

#include "SD_MMC.h" //library interface for communicating with SD cards 

#include "FS.h" //library for file system operations, such as reading and writing files on the SD card

#include "HTTPClient.h" //library for making HTTP requests to a server, such as sending data or receiving data from a web API

#include <WiFiClientSecure.h> // library for establishing secure HTTPS connections over Wi-Fi, allowing the ESP32 to communicate with servers using SSL/TLS encryption





//CAMERA PIN DEFINITIONS
// Camera pin connections for an AI Thinker-style ESP32-CAM
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5

#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22


//GLOBAL VARIABLES 
//This creates a constant named BUTTON_PIN with the value 13.
const int BUTTON_PIN = 13;
//Global variable to keep track of the photo number
int photoNumber = 1; 
// Global variable to keep track of Wi-Fi connection status
bool wasWifiConnected = false;


//FUNCTIONS
//FUNCTION - LIST PHOTOS
//function to list all the photos on the SD card, prints the names of all files in the root directory of the SD card to the serial monitor.
void listPhotos() {
  File root = SD_MMC.open("/");

  File file = root.openNextFile();

  while (file) {
    //only print the name of the file if it is not a directory and has a .jpg extension
    if (!file.isDirectory() && String(file.name()).endsWith(".jpg")) {
      Serial.println(file.name());
    }
    file.close();
    file = root.openNextFile();
  }

  root.close();
}




//FUNCTION - UPLOAD PHOTO
//function which is called after the photo is taken, if no wifi, stays on sd card as already done, else opens file
// creates a fucntion that accepts a filename
void uploadPhoto(String filename) {

  // Only attempt uploads when connected to Wi-Fi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No Wi-Fi. Photo stays on SD card.");
    return;
  }
String wifiName = WiFi.SSID();
String ipName = WiFi.localIP().toString();
Serial.println("Still connected to Wi-Fi: " + wifiName + " with IP: " + ipName);

  // Open an existing photo from the SD card
  // Opens one specific photo from the SD card, using the filename provided as an argument to the function. 
  File file = SD_MMC.open(filename, FILE_READ);

  if (!file) {
    Serial.println("Could not open photo!");
    return;
  }

  Serial.print("Ready to upload: ");
  Serial.println(filename);
  Serial.print("Photo size: ");
  Serial.println(file.size());


  
//Send the photo to the Supabase Edge Function using an HTTP POST request
// Create a secure HTTPS connection
WiFiClientSecure client;
client.setInsecure();  // Testing only: skips certificate verification

HTTPClient http;

// Connect to our Supabase Edge Function
if (!http.begin(client, SUPABASE_UPLOAD_URL)) {
  Serial.println("Could not start HTTP connection.");
  file.close();
  return;
}

// Add the headers expected by our Edge Function
http.addHeader("Content-Type", "image/jpeg");
http.addHeader("x-device-token", DEVICE_UPLOAD_TOKEN);

// Send the JPEG bytes directly from the SD card
int responseCode = http.sendRequest("POST", &file, file.size());

// Check Supabase's response
Serial.print("HTTP response: ");
Serial.println(responseCode);

if (responseCode == 200) {
  Serial.println("Photo uploaded successfully!");
  Serial.println(http.getString());
} else {
  Serial.println("Upload failed. Photo remains on SD card.");
  Serial.println(http.getString());
}

// Close the connections
http.end();
file.close();

// Only delete the local photo if Supabase confirmed success
if (responseCode == 200) {
  if (SD_MMC.remove(filename)) {
    Serial.println("Deleted from SD card: " + filename);
  } else {
    Serial.println("Failed to delete: " + filename);
  }
}

}



//FUNCTION - UPLOAD PENDING PHOTOS
// Upload all JPEG photos currently on the SD card
void uploadPendingPhotos() {

  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  while (true) {

    File root = SD_MMC.open("/");
    File file = root.openNextFile();

    String filename = "";

    // Find the first JPEG
    while (file) {

      if (!file.isDirectory() && String(file.name()).endsWith(".jpg")) {
        filename = "/" + String(file.name());
        file.close();
        break;
      }

      file.close();
      file = root.openNextFile();
    }

    root.close();

    // No more photos to upload
    if (filename == "") {
      break;
    }
Serial.println("PENDING PHOTO UPLOAD: " + filename);
    uploadPhoto(filename);

    // Stop if the photo is still there (upload or deletion failed)
    if (SD_MMC.exists(filename)) {
      Serial.println("Photo still pending. Stopping retries.");
      break;
    }
  }
}




//FUNCTION - TAKE PHOTO
//function to take a photo, save it to the SD card, and then upload it to
void takePhoto(){
  // Discard the first frame, which may be stale
  camera_fb_t *photo = esp_camera_fb_get();

  if (photo != nullptr) {
    esp_camera_fb_return(photo);
  } 

  // Capture a fresh frame
  photo = esp_camera_fb_get();

  if (photo == nullptr) {
    Serial.println("Photo capture failed!");
    return;
  }

  //Create variable of filename to save photo to SD card.
  String filename = "/photo" + String(photoNumber) + ".jpg";
  // Keep increasing the number until we find an unused filename, will not overwrite even after resetting
  while (SD_MMC.exists(filename)) {
    photoNumber++;
    filename = "/photo" + String(photoNumber) + ".jpg";
  }
  // Open a file on the SD carD
  //creates a variable representing a file on the SD card, and opens it for writing. The file is named "photo(number).jpg" and is located in the root directory of the SD card.
  File file = SD_MMC.open(filename, FILE_WRITE);

  if (!file) {
    Serial.println("Failed to open file!");
    esp_camera_fb_return(photo);
    return;
  }

  // Write the photo data into the file
  file.write(photo->buf, photo->len);

  // Close the file
  file.close();

  //increment the photo number for the next photo
  photoNumber++;

  //you're returning the memory buffer to the camera driver, meaning the photograph is no longer available for your program to use.
  //fine for testing, but we'll need to send the photograph somewhere before returning the buffer.
  esp_camera_fb_return(photo);
  
  Serial.println(filename + " saved to SD card!");
Serial.println("NEW PHOTO UPLOAD: " + filename);
  uploadPhoto(filename);

};


//FUNCTION - SETUP
//setup() runs ONCE when the ESP32 is powered on or reset
void setup() {
  Serial.begin(115200);
  Serial.println("Starting camera...");
  delay(1000);


  //INPUT_PULLUP tells the ESP32 to treat GPIO 13 as an input and use its internal pull-up resistor.
  //this means button pressed = low and button released = high. This is because the internal pull-up resistor connects the pin to a high voltage level when the button is not pressed, and when the button is pressed, it connects the pin to ground (low voltage level).
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Try connecting to Wi-Fi for up to 5 seconds, then continue offline
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); //.begin() is a function provided by the WiFi library that intiises the wifi connection.
  Serial.print("Connecting to Wi-Fi");

  //attempts 10 times every 0.5s
  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(500);
    Serial.print(".");
    attempts++;
  }



  Serial.println();


  if (WiFi.status() == WL_CONNECTED) {
   Serial.println("Wi-Fi connected!");
   Serial.println(WiFi.localIP());
  }   
  else {
    Serial.println("No Wi-Fi available. Continuing offline...");
  }


//SD_MMC.begin() = function provided by the SD_MMC library that initializes the SD card interface and mounts the file system on the SD card.
  if (SD_MMC.begin("/sdcard", true)) {
      Serial.println("SD card connected!");
      listPhotos(); //runs function to list all photos on SD, once SD card is connected succefully
  }   
  else {
  Serial.println("SD card failed!");
  }




  // Create a configuration object for the camera
  //camera_config_t is a predefined struct (structure) = data structure by the esp32 camera library, holds many different settings may include some ints some bools etc. 
  //config is the variable name
  //{} is the initilaizer list, everything = 0 or false.
  camera_config_t config = {};

  // Tell the camera driver which pins to use
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;

  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d0 = Y2_GPIO_NUM;

  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;

  // Camera settings
  config.xclk_freq_hz = 20000000;
  config.ledc_timer = LEDC_TIMER_0;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_VGA; // sets the frame size to 640*480 pixels (VGA resolution)
  config.jpeg_quality = 12;
  config.fb_count = 1;

  // Initialise the camera - the driver uses these settings to initialise cam hardware and return a status code 
  // esp_err_t is a data type provided by the lib to store error/status codes = ESP_OK = 0, ESP_FAIL = -1, etc.
  //result = variable
  //esp_camera_init() is a function provided by the library that initaises camera with our redefiend settings = the variable = config
  //comfig is the variable, and &config gives the address of the variable config 

  esp_err_t result = esp_camera_init(&config);

  if (result != ESP_OK) {
    Serial.printf("Camera failed: 0x%x\n", result);
    return;
  }

  Serial.println("Camera initialised successfully!");





}



//LOOP - BUTTON PRESS DETECTION
// loop() runs REPEATEDLY while the ESP32 is powered on
void loop() {


  bool wifiConnected = (WiFi.status() == WL_CONNECTED);

  if (wifiConnected && !wasWifiConnected) {
    Serial.println("Wi-Fi connected! Checking for pending photos...");
    uploadPendingPhotos();
  }

  wasWifiConnected = wifiConnected;


      //digitalRead() = function that detects whether a GPIO pin is high or low
      if (digitalRead(BUTTON_PIN) == LOW) {
        //Serial.println("Button pressed!");
        takePhoto();
        while (digitalRead(BUTTON_PIN) == LOW) { //improved code to prevent multiple photos being taken 
          delay(10);
        }
      }

  }

  



