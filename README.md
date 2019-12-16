# nixie_clock

## How to get project

> git submodule update --init --recursive
> git clone --recursive

## Recommended ESP32 IDF version

v3.3

> git clone -b v3.3 --recursive https://github.com/espressif/esp-idf.git esp-idf-v3.3<br>

## How to configure nixie-clock

If nixie clocks device is not yet configured, after power up it runs AP wi-fi mode
(ssid: "NixieClockXXXXXX", where XXXXXX is MAC address of the device; and password: "00000000").
In case if nixie clocks device is already configured, you can always reconfigure device
by pressing and holding Button 4 (_on some devices this button is located on main board in the
left-front corner_).

Connect to AP provided by Nixie Clock from Mobile Phone or PC, and then open web-page
http://nixie-clock.local/ in browser. If your device (PC or Mobile Phone) doesn't support
zeroconf, then use http://192.168.4.1/ url.

If nixie clocks are configured to connect your Home AP, then connect to your Home AP from
Mobile Phone or PC, and then open web-page http://nixie-clock.local/ (or http://ip/) in browser.

Remember, IGMP Snooping must be enabled to allow zeroconf protocol.

## Manual on Keys

There are 2 types of Nixie Clocks available:
*Revision 1*
The device has 4 hardware buttons: 3 buttons on the top (or back) side, and 1 button (Button 4) on
Main board (left side).

*Revision 2*
The device has 6 hardware buttons: 4 buttons on the top (or back) side, and 2 buttons on Main board.
Buttons on Main board are: left is reboot button, and middle-button is factory reset button.

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
   * Short press - disable/enable alarm
   * Long press - setup alarm
 * Button 3
   * Short press - disable/enable highlight
   * Long press - turn off/turn on clock (sleep mode)
 * Button 4
   * Short press - change highlight color
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
