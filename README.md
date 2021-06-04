# ESP32cam_secure_server

Multi-cam streaming server using node and websockets.
Based on ThatProject repo: 
https://github.com/0015/ThatProject/tree/master/ESP32CAM_Projects/ESP32_CAM_MULTICAM/Part.10-Multiple_CAM_Dashboard_with_HTTPS_WSS/NodeServer_v3

Uses Arduino board as a replacement for FDTI to program the ESP32 board 

##Installation Instructions

! Ensure node is installed before proceeding

### Node server/Dashboard
* Copy/Clone git Repo
* From terminal/command line enter
`node ci`
  * This will install required node packages, respecting contents of package-lock.json. 
  * `node install` will install using package.json only, ignoring package-lock.js
* Generate Self-Signed Certificate using OpenSSL using following file names, and place in root
  * server.key
  * server.cert
* Run server by entering 'node server.js' from terminal/commandline

### Arduino/ESP32CAM
* Edit file "secureESP32CamStream.ino"
 * Enter wifi credentials at following lines:
``` 
const char* ssid = "SSID";
const char* password = "PASSWORD";
const char* websocket_server_host = "wss://192.168.1.80";
```
  * Set ESP32 cam ID number at following line
  ```
  fb->buf[12] = 0x01; //FIRST CAM
  //fb->buf[12] = 0x02; //SECOND CAM
  ```
  * Validate code in Arduino IDE, then push to board
  * Repeat previous two steps for any additional ESP32Cams

## Notes

* Have made some changes to the server and HTML client code to make variables, IDs more meaningful
* Have improved the getFormattedTime() function
* Toggling streams on/off is now based on element bool value instead of InnerHTML contents

---
&nbsp;
## TODO
* Separate client scripts out from HTML file
* Improve dashboard HTML/CSS 
  * Use flexbox to structure dashboard
  * Use CSS to hide/display elements
* Refactor code to use dynamic arrays to hold stream object list instead of hardcoding handling of incoming streams
* 
