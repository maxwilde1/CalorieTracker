# ESP32 camera initialisation and photo capture
## learnings:
- **#include**	Makes library declarations available, in this case esp32 library
- **#define**	Defines constants or macros for the preprocessor, gives names of functions on esp32 the assigned GPIO pin
- **struct**	Groups multiple related data into one data structure e.g. when used camera_config_t config = {};
- **&config**	Gets the memory address of config - config is the variable, and &config gives the address of the variable config 
- ***photo**	Declares a pointer to a frame buffer. **camera_fb_t *photo = esp_camera_fb_get();** camera_fb_t — a structure defined by the camera library. *photo — declares photo as a pointer. esp_camera_fb_get() — returns the memory address of a captured frame buffer. So photo stores the memory address of the frame buffer, rather than the frame buffer itself.
- **esp_camera_fb_return()**	Returns the frame buffer (where image temporarily stored) to the camera driver, config.fb_count = 1; means only one frame buffer and gets overwritten with every photo taken


# Connecting ESP32 to wifi
## Seperating secrets from main program 
- inside .gitignore simply write the name of the file wishing to be ignored by . In this case = **secrets.h** . github wont allow file called secret.h to be pushed. 
- .h stands for header file commonly used for declarations and configuration that other files can include 
- **include guard** - #ifndef SECRETS_H #define SECRETS_H : SECRETS_H isn't a variable. It's a preprocessor macro. like the GPIO names. if SECRETS_H is not defined, define it. This gets done once on compilation but then not again. This is so declaration or definitons doesnt occur twice, potentially casuing errors

**const char* WIFI_SSID = "YOUR_WIFI_NAME";**
- char = data type respresenting single charcter 
- * = declares pointer variable that points to a string of characters (const char* = pointer to constant character array, meaning the string cannot be modified)
- WIFI_SSD = variable name

#include <WiFi.h> //existing library for wifi connection
#include "secrets.h" //include code from another file (the secrets.h file that contains the wifi credentials)


# Send text over WiFi
Creation of web server: which allows another device (your computer, phone or eventually your CalorieTracker app) to request information from the ESP32, over the same wifi network only. 
- `#include <WebServer.h>`

- `WebServer server(80);` - WebServer — A library class for creating a web server on the ESP32. server — The object (variable) created from that class. 80 — The port number the server listens on (default for HTTP).
- `server.on("/", ...)` — Defines what happens when someone visits the ESP32's homepage (`/`).
- `[]() {}` — Lambda function (unnamed function) that runs when the URL is requested.
- `server.send(200, "text/plain", "Hello from CalorieSense!")` — Sends a successful HTTP response (`200`) containing plain text.
- `server.begin()` — Starts the web server, allowing it to accept connections.

# Send image instead of text over wifi
server.on("/capture", []() {
    
  camera_fb_t *photo = esp_camera_fb_get();


  if (photo == nullptr) {
    server.send(500, "text/plain", "Camera capture failed");
    return;
  }

  server.sendHeader("Content-Disposition", "inline; filename=photo.jpg");
  server.setContentLength(photo->len);
  server.send(200, "image/jpeg", "");

  server.client().write(photo->buf, photo->len);

  esp_camera_fb_return(photo);
});

server.begin();


`server.on("/capture", []() {` — Creates the /capture URL route. When visited, the ESP32 runs the lambda function to capture and send a photo.

`camera_fb_t *photo = esp_camera_fb_get();` — Creates a pointer called photo to a camera frame-buffer structure (camera_fb_t). The function obtains a captured image and returns its memory address.

`if (photo == nullptr) {` — Checks whether capturing failed. nullptr means the pointer doesn't point to a valid object.

`server.send(500, "text/plain", "Camera capture failed");` — Sends an HTTP 500 error with a plain-text message if the camera fails.

`return;` — Exits the function early, preventing the remaining code from running.

`server.sendHeader("Content-Disposition", "inline; filename=photo.jpg"); `— Adds an HTTP header suggesting the browser display the image directly (inline) and use photo.jpg as its filename if saved.

`server.setContentLength(photo->len);` — Specifies the image size in bytes (photo->len), telling the browser how much data to expect.

`server.send(200, "image/jpeg", "");` — Sends a successful HTTP response (200) with JPEG content type. The empty string means the image bytes will be sent separately.

`server.client().write(photo->buf, photo->len);` — Sends the actual image bytes to the connected client. photo->buf points to the image data, and photo->len specifies the number of bytes to send.

`esp_camera_fb_return(photo);` — Returns the frame buffer to the camera driver so its memory can be reused.

`});` — Closes the lambda function and the server.on() call.

`server.begin();` — Starts the ESP32 web server, allowing it to accept incoming HTTP connections.















# Pointers explained 
camera_fb_t *photo = esp_camera_fb_get();


1. camera_fb_t — The data type
This is a struct defined by the ESP32 camera library.

It contains information about a photograph, including:
- buf — Pointer to the actual image data.
- len — Size of the image in bytes.
- width — Image width in pixels.
- height — Image height in pixels.
2. *photo — Declares a pointer variable
Normally, you might write:
int age = 22;


Here, age directly stores the number 22.

But with:
camera_fb_t *photo;


You're creating a variable called photo that stores a memory address rather than a camera_fb_t structure itself.

The * tells C++ that photo is a pointer to a camera_fb_t object.
3. esp_camera_fb_get() — The function
This function is provided by the ESP32 camera library.

It obtains a captured image and returns a pointer to the frame-buffer structure containing information about that image.
4. = — Assigns the returned address
camera_fb_t *photo = esp_camera_fb_get();


This means:
1. Call esp_camera_fb_get().
2. The camera driver obtains a photograph.
3. The function returns the memory address of its frame-buffer structure.
4. Store that address in the pointer variable photo.
5. How do we use the pointer?
Imagine the photograph's frame-buffer structure is stored at memory address 0x2000.
Illustrative memory address and image values.
Because photo is a pointer, we access its fields using ->:
Serial.println(photo->width);  // 640
Serial.println(photo->height); // 480
Serial.println(photo->len);    // 45000


The -> operator means follow the pointer to the structure, then access the specified field.
6. Why use a pointer instead of storing the photograph directly?
Because the camera driver already manages the frame buffer in memory.

Rather than copying it, the function gives us a pointer to access it directly.

This is more efficient, particularly on an ESP32 where memory is limited.

Important: The two meanings of *
In C++, the asterisk can mean different things depending on where it's used.
camera_fb_t *photo;   // Declares a pointer


But:
*photo;               // Dereferences the pointer


Dereferencing means accessing the actual object at the memory address stored in the pointer.










#