# Arduino MIDI Controller

This project allows you to build a flexible MIDI controller for switching programs and banks.

## Features
- **Program Change**: Press any configured button to switch programs within the current bank.
- **Bank Navigation**: Press buttons 1 and 2 simultaneously for "bank up", or buttons 2 and 3 for "bank down".
- **Adaptable Button Configuration**: You need >=3 buttons, according to the number of buttons connected, the programm will figure out the number of necessary banks for a full coverage across the 128 MIDI channels
- **LCD Feedback**: Provides visual feedback on the current bank and program selection via an LCD display.

## Hardware Requirements

- Arduino Uno or compatible board
- Push buttons (minimum 3 required for basic functionality)
- LiquidCrystal LCD display (16x2 or compatible)
- Jumper wires and breadboard for connections
- Optional: resistors for buttons if not using `INPUT_PULLUP` mode

## Software Requirements

- Arduino IDE for uploading the code to the Arduino board

## Setup and Configuration

1. **Wiring**: Connect the buttons and LCD display to your Arduino, adapt the pin configuration to match your setup at the top of the sketch.
3. **Upload**: Use the Arduino IDE to compile and upload the sketch to your Arduino board.

## License

This project is open source and available under the [MIT License](LICENSE).
