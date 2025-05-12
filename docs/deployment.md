
# Here’s the minimal list of what you need
To deploy and test your Belfhym project on the STM32F103C8T6 (Blue Pill) board, 
a Blue Pill alone is not enough — you’ll also need tools to power it, program it, and potentially debug it.

1. STM32F103C8T6 Blue Pill Board
   This is your actual microcontroller board.
   !!!Make sure it's a good-quality one (some clones have bad USB connectors or faulty crystals).

2. ST-Link V2 Programmer/Debugger
   You must have this to flash your firmware (unless you want to use the built-in bootloader via UART, which is less convenient).
   ST-Link also allows debugging via SWD, which is critical for FreeRTOS and embedded debugging.
   !!!Beware of super-cheap ST-Link clones — they often work but might have limitations or be unstable.

3. Dupont Cables (Male-to-Female or Male-to-Male)
    You’ll need 4 wires to connect ST-Link to the Blue Pill:
    - SWDIO
	- SWCLK
	- GND
	- 3.3V (optional; ST-Link can power the board if connected)
	
4. USB to Serial Adapter (Optional but Useful)
   For debugging via UART (e.g., printf() debugging).
   Useful for serial console output or bootloader flashing if needed.

5. USB Mini-B Cable
   To power the Blue Pill through its USB port.
   Also needed if you ever want to use USB (e.g., CDC/Serial over USB) in your project.

6. Breadboard and Power Supply (Optional)
   Handy for experimenting, connecting sensors, or prototyping.
   
   
Item	                Required	Notes
---------------------------------------------------------
STM32F103C8T6 Blue Pill	    R   Main board
ST-Link V2	                R	Programming and SWD debugging
Dupont wires	            R	For connecting ST-Link
USB to Serial Adapter	    - 	Optional, for UART debugging
USB Mini-B cable	        R	Power and USB features
Breadboard + PSU	        -	Optional for prototyping

---

# Step-by-Step Deployment Process

1. Connect ST-Link V2 to Blue Pill
   - Use jumper wires to connect the ST-Link to the Blue Pill:
   - ST-Link → Blue Pill
	 -------------------
	 SWDIO   → SWIO (PA13)
	 SWCLK   → SWCLK (PA14)
	 GND     → GND
	 3.3V    → 3.3V

2. Power Source: The ST-Link supplies power (3.3V), no need for USB or other power unless required by peripherals.
3. Software Setup on Linux
   1. Install toolchain and flashing tools
   ```bash
   sudo dnf install arm-none-eabi-gcc arm-none-eabi-newlib openocd
   ```
   
   2. Install stlink-tools for direct flashing
   ```bash
   sudo dnf install stlink
   ```
   3. Plug it into USB and verify ST-Link is connected
   ```bash
   lsusb | grep ST-LINK
   ```
4. Build Firmware (belfhym)
```bash
make clean
make
```
5. Flash Firmware to Blue Pill
	- Option 1:
	 1. Start OpenOCD server:
	 ```bash
	 openocd -f interface/stlink.cfg -f target/stm32f1x.cfg
	 ```
	 2. In a separate terminal:
	 ```bash
	 telnet localhost 4444
	 ```
	 3. Then
	 ```bash
	 reset halt
	 flash write_image erase bin/belfhym 0x08000000
	 reset run
	 exit
	 ```
	 - Option 2: 
	 1. Flash with st-flash
	 ```bash
	 st-flash write build/belfhym.bin 0x8000000
	 ```
    - Option 3:
	```bash
	make deploy
	```

## Debug or Monitor Output
1. If your firmware sends serial data via USART1 (PA9/PA10), use your USB-to-Serial adapter:
    PA9 → TX
    PA10 → RX
	GND → GND
	Then use a tool like minicom:
	```bash
	sudo dnf install minicom
	minicom -D /dev/ttyUSB0 -b 115200
	```
