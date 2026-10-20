# ESP32 camera initialisation and photo capture
## learnings:
- **#include**	Makes library declarations available, in this case esp32 library
- **#define**	Defines constants or macros for the preprocessor, gives names of functions on esp32 the assigned GPIO pin
- **struct**	Groups multiple related data into one data structure e.g. when used camera_config_t config = {};
- **&config**	Gets the memory address of config - config is the variable, and &config gives the address of the variable config 
- ***photo**	Declares a pointer to a frame buffer. **camera_fb_t *photo = esp_camera_fb_get();** camera_fb_t — a structure defined by the camera library. *photo — declares photo as a pointer. esp_camera_fb_get() — returns the memory address of a captured frame buffer. So photo stores the memory address of the frame buffer, rather than the frame buffer itself.
- **esp_camera_fb_return()**	Returns the frame buffer (where image temporarily stored) to the camera driver, config.fb_count = 1; means only one frame buffer and gets overwritten with every photo taken
