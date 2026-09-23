# Kan Weight Scale

An Arduino kitchen scale built around an ATmega328P board, an HX711 load-cell
amplifier, and a 16×2 I²C LCD. The potentiometer provides convenient live
calibration, while three buttons expose calibration, tare, and raw-reading
functions.

> The firmware defaults to an **Arduino Pro Mini 3.3 V / 8 MHz**. Check the
> voltage requirements of every module before wiring or powering the project.

## Hardware

- Arduino Pro Mini 3.3 V / 8 MHz (or a compatible ATmega328P board)
- Load cell and HX711 amplifier
- HD44780-compatible 16×2 LCD with an I²C backpack
- Linear potentiometer
- Three normally-open push buttons
- Battery, power switch, and a suitable voltage divider for battery monitoring
- Optional: two 0.1 µF capacitors for filtering a noisy load-cell signal

## Wiring

| Function | Arduino pin | Notes |
| --- | --- | --- |
| HX711 `DOUT` | D7 | Data output from the HX711 |
| HX711 `CLK` | D6 | HX711 clock input |
| Calibration potentiometer | A2 | Connect the other terminals to VCC and GND |
| Battery monitor | A0 | Connect only through a correctly sized voltage divider |
| Calibration button | D10 | Button connects the pin to GND |
| Tare/zero button | D11 | Button connects the pin to GND |
| Memory/raw button | D12 | Button connects the pin to GND |
| LCD SDA/SCL | SDA/SCL | A4/A5 on an ATmega328P |

Connect the HX711 and load cell according to the labels printed on the modules.
Common load-cell colours are red for `E+`, black for `E-`, white for `A+`, and
green or blue for `A-`, but **wire colours are not standardized**. Consult the
load-cell datasheet before connecting it.

The firmware configures the LCD at address `0x27`. If the display remains blank,
scan the I²C bus and update the address in `src/main.cpp`. Some backpacks use
`0x3F`. The LCD constructor also assumes the common pin mapping used by the
vendored NewLiquidCrystal library.

### Battery monitor

The voltage-monitoring constants in `src/main.cpp` describe the installed ADC
reference and divider resistors. Measure the actual resistor values and board
reference voltage, then update `voltMult`, `resistor1`, and `resistor2`. Never
apply a voltage outside the microcontroller's permitted input range to A0.

## Build and upload

[PlatformIO](https://platformio.org/) is the supported build environment. From
the repository root:

```sh
pio run
pio run --target upload --upload-port /dev/ttyUSB0
pio device monitor --baud 9600
```

Change the upload port for your computer. It is intentionally not committed in
`platformio.ini`, so cloning the project does not target a device-specific
serial port. If you use another board, add a matching PlatformIO environment or
override the board setting.

Project-specific versions of HX711, NewLiquidCrystal, and Low-Power are vendored
under `lib/`; no separate Arduino library installation is required.

Every push and pull request also builds the firmware through the GitHub Actions
workflow in `.github/workflows/build.yml`.

## Calibrate and use

1. Start the scale with an empty platform. Startup automatically tares it.
2. Place an accurately known weight on the platform.
3. Hold the **calibration** button (D10) and turn the potentiometer until the
   displayed reading matches the known weight.
4. Release the button to return to normal weighing.
5. Press **tare/zero** (D11) whenever an empty container should become zero.
6. Press **memory/raw** (D12) to display the current averaged raw ADC reading.

The calibration factor comes directly from the potentiometer and is not saved
across restarts. For repeatable measurements, mark the calibrated position or
adapt the firmware to store a factor in EEPROM.

## Configuration reference

The hardware pins, LCD address, ADC reference, and voltage-divider resistor
values are defined near the top of `src/main.cpp`. Rebuild and upload after
changing them.

## Troubleshooting

- **No display:** verify power, contrast, SDA/SCL wiring, I²C address, and LCD
  backpack pin mapping.
- **Reading never changes:** verify `DOUT`/`CLK`, the HX711 supply, and all four
  load-cell connections.
- **Reading moves in the wrong direction:** swap `A+` and `A-` or account for
  the sign in calibration.
- **Unstable values:** use short wires, a common ground, solid mechanical load
  transfer, supply decoupling, and optional 0.1 µF filtering capacitors.
- **Incorrect battery voltage:** measure the ADC reference and divider
  resistors, then update the three battery-monitor constants.

## License and credits

Created by [Konstantinos Anastasakis](https://github.com/kon-anast) and
documented on [Kostis Lab](https://kostislab.blogspot.com/). The vendored
libraries retain their own metadata and license files. No project-level license
has been declared; contact the author before redistributing the application
code.
