# nixie_clock

## How to get project

> git submodule update --init --recursive
> git clone --recursive

## Recommended ESP32 IDF version

v3.2

> git clone -b v3.2 --recursive https://github.com/espressif/esp-idf.git esp-idf-v3.2<br>

## How to configure nixie-clock

If nixie clocks are not yet configured, then connect to AP (ssid: "ncXXXXXX", where XXXXXX - 6 digits
of MAC addr, password: "00000000") provided by Nixie Clock from Mobile Phone or PC,
and then open web-page http://nixie-clock.local/ in browser.

If nixie clocks are configured to connect your Home AP, then connect to your Home AP from
Mobile Phone or PC, and then open web-page http://nixie-clock.local/ in browser.

Remember, IGMP Snooping must be enabled to allow Bonjour protocol.
