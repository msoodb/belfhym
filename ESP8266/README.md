 
# Install 

```bash
sudo dnf install flatpak
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub cc.arduino.arduinoide
```

## Run
```bash
flatpak run cc.arduino.arduinoide
``` 

# Install ESP8266 Board Support in Arduino IDE
- Open Arduino IDE.
- Go to File > Preferences.
- Find Additional Boards Manager URLs. Paste this:
- https://arduino.esp8266.com/stable/package_esp8266com_index.json
- Click OK.
- Go to Tools > Board > Boards Manager.
- Search for esp8266.
- Click Install on the package named esp8266 by ESP8266 Community.

# Hello world
- Connect your ESP8266 Board
- ls /dev/ttyUSB*
