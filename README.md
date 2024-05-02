# Rantoge ESP32 

A replacement for the Rantoge motherboard using an ESP32 and DRV8825 stepper drivers.

## Pin assignments

The code expects these GPIO pins to be used:

| Pin| Connection |
| --- | --- |
| 16 | Left button  |
| 17 | Right button |
| 18 | Minutes STEP |
| 19 | Minutes SLEEP |
| 21 | Hours STEP |
| 22 | Hours SLEEP |

Buttons should be active low (connected to ground when pressed).

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
	- Long press to calibrate. The motor will step slowly until the button is released.

**TO DO: Configuring the current time and displayed time using only buttons.**