#include <LiquidCrystal.h>

// ================= CONFIGURATION ====================

// =============== Button Configuration ==============
// Define the pins for the buttons connected to the Arduino.
// The minimum requirement is 3 buttons for the banks to work;
// beyond that, you can have as many buttons as you want.
// - Any single button press triggers a program change.
// - Simultaneous press of button 1 and 2 switches to the next bank.
// - Simultaneous press of button 2 and 3 switches to the previous bank.
// The program calculates the necessary number of banks and maps each 
// button to the corresponding MIDI channel within the current bank,
// ensuring full coverage across all 128 MIDI channels.
int buttonPins[] = {8, 9, 10, 13, 6, 7};
// ===========================================================

// ================ LCD Display Configuration ================
// Initializes the library with the numbers of the interface pins
// Pin layout: RS, E, D4, D5, D6, D7
// Adjust these pin numbers to match the connections between
// your Arduino and the LCD display.
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
// ===========================================================

// ================= END OF CONFIGURATION ====================


const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);
int buttonStates[numButtons];
int previousButtonStates[numButtons];
int currentBank = 1;
int currentProgram = 1;
const int maxMidiChannels = 128;
int totalBanks;

// Setup function: Initializes the Arduino setup
void setup() {
    Serial.begin(115200);
    lcd.begin(16, 2);
    for (int i = 0; i < numButtons; i++) {
        pinMode(buttonPins[i], INPUT_PULLUP); 
        buttonStates[i] = HIGH; 
        previousButtonStates[i] = HIGH;
    }
    totalBanks = (maxMidiChannels + numButtons - 1) / numButtons;
    updateDisplay();
}

// Main loop function: Repeatedly executes the main logic
void loop() {
    readButtons();
    handleProgramChanges();
    handleBankChanges();
}

// Reads the state of all configured buttons
void readButtons() {
    for (int i = 0; i < numButtons; i++) {
        previousButtonStates[i] = buttonStates[i];
        buttonStates[i] = digitalRead(buttonPins[i]);
    }
}

// Handles program changes based on button presses
void handleProgramChanges() {
    for (int i = 0; i < numButtons; i++) { 
        if (buttonStates[i] == LOW && previousButtonStates[i] == HIGH) { 
            currentProgram = i + 1 + (currentBank - 1) * numButtons;
            if (currentProgram <= maxMidiChannels) { 
                sendMidiProgramChange(baseProgramChangeCommand, currentProgram);
                updateDisplay();
            }
            break; 
        }
    }
}

// Handles bank changes based on specific button combinations
void handleBankChanges() {
    bool bankUpPressed = (buttonStates[0] == LOW && buttonStates[1] == LOW);
    bool bankDownPressed = (buttonStates[1] == LOW && buttonStates[2] == LOW);

    if (bankUpPressed || bankDownPressed) {
        delay(10); // Debounce delay
        readButtons(); // Confirm button states
        if (bankUpPressed) {
            currentBank = min(currentBank + 1, totalBanks);
            updateDisplay();
        } else if (bankDownPressed) {
            currentBank = max(currentBank - 1, 1);
            updateDisplay();
        }
    }
}

// Updates the display with the current bank and program
void updateDisplay() {
    lcd.clear();
    lcd.print("Bank: ");
    lcd.print(currentBank);
    lcd.setCursor(0, 1);
    lcd.print("Program: ");
    lcd.print(currentProgram);
}

// Sends a MIDI Program Change message
void sendMidiProgramChange(int commandType, int programNumber) {
    Serial.write(commandType);
    Serial.write(programNumber - 1); // MIDI programs are 0-indexed
}
