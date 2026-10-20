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
