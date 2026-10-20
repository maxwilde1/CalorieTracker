//Arduino uses c++ 

#include "esp_camera.h" //existing library for esp32 cam module 
#include <WiFi.h> //existing library for wifi connection
#include "secrets.h" //include the secrets.h file that contains the wifi credentials





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




void setup() {
  Serial.begin(115200);
  Serial.println("Starting camera...");
  delay(1000);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); //.begin() is a function provided by the WiFi library that intiises the wifi connection.
  
  Serial.print("Connecting to Wi-Fi");

  //WiFi.status is function provided by wifi library that returns current status of connection
  //WL_CONNECTED is a constant provided by the WiFi library that represents a successful connection to a Wi-Fi network
    //WL_CONNECTED is a constant provided by the WiFi library that represents a successful connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Wi-Fi connected!");
  Serial.println(WiFi.localIP()); //function that returns the local IP address assigned to the ESP32 by the Wi-Fi network. This is useful for accessing the ESP32 over the network.

  

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

// loop() runs REPEATEDLY while the ESP32 is powered on
void loop() {
    
//camera_fb_t is a data type representing a camera frame buffer
//*photo is a pointer variable (pointer variable = stores memory address of soemthign rather than actual variable) called photo that points to a camera frame buffer object returned by the esp_camera_fb_get() function. This function captures an image from the camera and returns a pointer to the frame buffer containing the image data.
//esp_camera_fb_get() is a function provided by the ESP32 camera library that captures an image from the camera and returns a pointer to the frame buffer containing the image data. 
//The frame buffer is a memory area that holds the raw pixel data of the captured image.
    camera_fb_t *photo = esp_camera_fb_get();


      // Check whether the capture succeeded
    if (photo == nullptr) {
        Serial.println("Photo capture failed!");
     } 
    else {
        Serial.println("Photo captured!");
        Serial.printf("Width: %d pixels\n", photo->width);
        Serial.printf("Height: %d pixels\n", photo->height);
        Serial.printf("Size: %u bytes\n", (unsigned int)photo->len);

    // Return the buffer so the camera can reuse it
    esp_camera_fb_return(photo);
    delay(5000);
  }

  

}

