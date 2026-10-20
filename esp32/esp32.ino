//Arduino uses c++ 

#include "esp_camera.h" //existing library for esp32 cam module 
#include <WiFi.h> //existing library for wifi connection
#include "secrets.h" //include the secrets.h file that contains the wifi credentials

#include <WebServer.h> //existing library for web server functionality




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

//webserver = a class provided by the library that allows you to create a web server on the ESP32. The server listens for incoming HTTP requests on port 80 (the default port for HTTP) and responds with the appropriate content.
//server = name of object we are creating, also a variable of type WebServer
// 80 = port number on which the server will listen for incoming requests. Port 80 is the default port for HTTP traffic.
WebServer server(80); 

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


  //This tells the ESP32 what to do when someone visits its IP address.
  //server.on() is a function that tells your esp32 what to do when someone visits its IP address. The first argument is the URL path ("/" means the root of the server), and the second argument is a lambda function that defines the response to send back to the client.
  //[]() is a lambda function, which is an anonymous function that can be defined inline. In this case, it defines the response to send back to the client when they visit the root URL of the server.
  //server.send() is a function that sends an HTTP response back to the client. The first argument is the HTTP status code (200 means OK), the second argument is the content type ("text/plain" means plain text), and the third argument is the actual content to send back
  server.on("/", []() {
    server.send(200, "text/plain", "Hello from CalorieSense!");
  });



  //capture is at the end of the IP link telling the esp to take a photo and send it to that web page
  server.on("/capture", []() {
    //camera_fb_t = predefined data structure (like before) representing camera frame buffer
    //*photo = declares pointer variable called photo
    //esp_camera_fb_get() = obtains captured frame from cam driver
    camera_fb_t *photo = esp_camera_fb_get();


    //nullptr = a special C++ value meaning poinrer doesnt point to a valid object
    if (photo == nullptr) {
      server.send(500, "text/plain", "Camera capture failed");
      return;
    }
    //.sendHeader() = function that adds an HTTP response header
    //"Content-Disposition" = name of the HTTP header that controls how the broswer handles the file
    //inline = tells the browser to display the img directly rather than download as attachment
    //suggest filename if img is saved
    server.sendHeader("Content-Disposition", "inline; filename=photo.jpg");
    //Specifies image size
    //photo->len = size of image data in bytes
    //setContentLength() = tells server how much data will be sent in response - helps browser know when it has received the complete image
    server.setContentLength(photo->len);
    //Sends response headers
    //image/jpeg = tells browser response contains a jpeg image
    //"" = empty string because noy sending actual img data through this method
    server.send(200, "image/jpeg", "");
    //Sends the photograph - important line
    //server.client() = returns the client connection associated with the current HTTP request
    //.write() = writes bytes to that connection
    //photo->buf = a pointer to the beginning of the img data in memory
    //photo->len = the no. bytes to send 
    server.client().write(photo->buf, photo->len);
    //Returns the buffer - need to tell camera driver we're finished using its buffer. returns the buffer to the driver.
    esp_camera_fb_return(photo);
  });

  server.begin();


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


    //This checks for incoming browser requests and processes them.
    server.handleClient();

  }

  



