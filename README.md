# nixie_clock

## How to get project

> git submodule update --init --recursive
> git clone --recursive

## Recommended ESP32 IDF version

v3.3

> git clone -b v3.3 --recursive https://github.com/espressif/esp-idf.git esp-idf-v3.3<br>

## How to configure nixie-clock

If nixie clocks are not yet configured, then connect to AP (ssid: "ncXXXXXX", where XXXXXX - 6 digits
of MAC addr, password: "00000000") provided by Nixie Clock from Mobile Phone or PC,
and then open web-page http://nixie-clock.local/ in browser.

If nixie clocks are configured to connect your Home AP, then connect to your Home AP from
Mobile Phone or PC, and then open web-page http://nixie-clock.local/ in browser.

Remember, IGMP Snooping must be enabled to allow Bonjour protocol.

## Manual on Keys

Nixie Clock has 6 hardware buttons: 4 buttons on the top/back side, and 2 buttons on Main board.

### Buttons on Main board

 * Left-front button on Main board is reset button. If clock hangs up, reset button short press will
reboot clocks. The same can be done via power cut.
 * Middle (second) button on Main board is Factory reset button. Hold it during power cycle to reset
clock to factory settings and firmware

### Buttons on back side

#### Normal clock operating mode

 * Button 1
   * Short press - change display mode
   * Long press - enter clock setup mode
 * Button 2
   * Short press - change highlight color
   * Long press - setup alarm
 * Button 3
   * Short press - disable/enable highlight
   * Long press - turn off/turn on clock (sleep mode)
 * Button 4
   * Short press - display IP address
   * Long press - start AP mode for setup

#### Sleep mode

 * Button 1
   * Short press - wake up to normal clock operating mode
 * Button 2
   * Short press - wake up to normal clock operating mode
 * Button 3
   * Short press - wake up to normal clock operating mode
 * Button 4
   * Short press - wake up to normal clock operating mode

#### Clock setup mode

 * Button 1
   * Short press - switch between hours, minutes, seconds, year, month, day
   * Long press - save setup
 * Button 2
   * Short press - increase value
 * Button 3
   * Short press - decrease value
 * Button 4
 * Timeout - return to normal operating mode without clock change
