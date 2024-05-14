# Rantoge ESP32 

A replacement for the Rantoge motherboard using an ESP32 and DRV8825 stepper drivers.

## Pin assignments

The code is intended for use with [this PCB](pcb/README.md).

It expects these GPIO pins to be used:

| Pin| Connection |
| --- | --- |
| 16 | Left button  |
| 17 | Right button |
| 18 | Minutes STEP |
| 19 | Minutes SLEEP |
| 21 | Hours STEP |
| 22 | Hours SLEEP |

Buttons are active low (connected to ground when pressed).

It expects the DRV8825s to be configured for 1/32 microstepping (otherwise, change `MICROSTEPPING_MULTIPLIER`).

## Configuration

### Internet-connected
- If no wi-fi credentials have been set, the clock will create its own `clock` network. Connect to it and go to http://clock.local/ to see the config page.
- Enter your network's SSID and password, and the clock will connect to it. The credentials are stored, and the clock will reconnect after resetting.
- The clock will request the current time from `pool.ntp.org`, and try to determine your time zone from its IP address using [ip-api.com](https://ip-api.com/).
- After a reset, the clock assumes that it is displaying the correct time. If it is not, you can enter the displayed time on the config page to correct it.

### Wi-fi but no internet
- If you don't have a wi-fi network available for the clock to connect to the internet, you can set the current time and time zone though the config page.
- The ESP32 uses [POSIX time zone strings](https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv). The firmware also understands tzdata time zone names, which it converts to a POSIX equivalent. 

### Buttons
- The left and right buttons control hours and minutes:
	- Single click to increment, double click to decrement (minutes will skip the next advance, rather than advancing 59 times).
	- Long press to calibrate. The motor will step slowly until the button is released.
- Triple click the left button to toggle demo mode. The clock will cycle through advancing hour, then both, then minute.
- Triple click the right button to set the displayed and current minute.
	- The clock will advance through the minutes automatically.
	- Single click the right button when it is advancing towards 00.
	- Double click the right button when it is advancing towards the current minute (as it was when the process started).
	- If the current time has been set from another source (SNTP or the config page), you only need to identify either the current minute or 00. The clock assumes that the current time is set correctly, and only needs to determine the displayed minute.
	- If the current time has not been set, or was set using this process, then you need to identify both the current minute and 00 to allow the clock to idenify the current and displayed minutes.
- (There is no need for an equivalent process for hours - the clock doesn't need to know what hour it is displaying.)


![config page](./config-page.png)
