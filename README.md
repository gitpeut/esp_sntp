# esp_sntp
simple sntp examples for esp8266 and esp32


This repository contains a simple SNTP example for ES8266 (simplesnmp_esp8266) and one for ESP32 (simplesnmp_esp32)
Also a messy test example (sntp_test) is supplied.

It "proves" timelibs and udp clients are superfluous, and standard c time functions mostly work.
On ESP8266 it has been verified that an SNTP request is done every hour automatically. No such test has been
done for ESP32.

