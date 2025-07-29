#include <LiquidCrystal.h>

// The MIDI channel used for program changes is user-configurable:
// - Pressing button 1 + 3 simultaneously enters channel config mode
// - In this mode:
//     - Button 1 increases the MIDI channel (1–16)
//     - Button 2 decreases it
//     - Button 3 exits config mode

// ================= CONFIGURATION ====================

// =============== Button Configuration ==============
// Define the pins for the buttons connected to the Arduino.
// The minimum requirement is 3 buttons for the banks to work;
// beyond that, you can have as many buttons as you want.
// - Any single button press triggers a program change.
// - Simultaneous press of button 1 and 2 switches to the next bank.
// - Simultaneous press of button 2 and 3 switches to the previous bank.
// The program calculates the necessary number of banks and maps each 
// button to the corresponding MIDI program within the current bank,
// ensuring full coverage across all 128 MIDI channels.
int buttonPins[] = {8, 9, 10}; // Minimum 3 buttons required
// int buttonPins[] = {8, 9, 10, 13, 6, 7}; (my controller has 6)
// ===================================================

// ================ LCD Display Configuration =========
// Initializes the library with the numbers of the interface pins
// Pin layout: RS, E, D4, D5, D6, D7
// Adjust these pin numbers to match the connections between
// your Arduino and the LCD display.
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
// ====================================================

// ================= END OF CONFIGURATION =============

const int numButtons = sizeof(buttonPins) / sizeof(buttonPins[0]);
int buttonStates[numButtons];
int previousButtonStates[numButtons];
int currentBank = 1;
int currentProgram = 1;
const int maxMidiChannels = 128;
int totalBanks;

bool inChannelConfigMode = false;
int midiChannel = 1;

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

    if (inChannelConfigMode) {
        handleChannelConfig();
    } else {
        handleProgramChanges();
        handleBankChanges();
        checkEnterChannelConfig();
    }
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
        if (buttonPressed(i)) { 
            currentProgram = i + 1 + (currentBank - 1) * numButtons;
            if (currentProgram <= maxMidiChannels) { 
                sendMidiProgramChange(currentProgram);
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

    static unsigned long lastBankChange = 0;
    if ((bankUpPressed || bankDownPressed) && millis() - lastBankChange > 500) {
        delay(10);
        readButtons();
        if (bankUpPressed) {
            currentBank = min(currentBank + 1, totalBanks);
        } else if (bankDownPressed) {
            currentBank = max(currentBank - 1, 1);
        }
        updateDisplay();
        lastBankChange = millis();
    }
}

// Detect simultaneous btn1 + btn3 to enter config mode
void checkEnterChannelConfig() {
    if (buttonStates[0] == LOW && buttonStates[2] == LOW) {
        delay(10);
        readButtons();
        if (buttonStates[0] == LOW && buttonStates[2] == LOW) {
            inChannelConfigMode = true;
            updateDisplay();
            delay(300);
        }
    }
}

// Handles channel config mode
void handleChannelConfig() {
    if (buttonPressed(0)) {
        midiChannel = min(16, midiChannel + 1);
        updateDisplay();
        delay(200);
    }
    if (buttonPressed(1)) {
        midiChannel = max(1, midiChannel - 1);
        updateDisplay();
        delay(200);
    }
    if (buttonPressed(2)) {
        inChannelConfigMode = false;
        updateDisplay();
        delay(300);
    }
}

// Updates the display with the current bank and program
void updateDisplay() {
    lcd.clear();
    if (inChannelConfigMode) {
        lcd.print("MIDI Channel: ");
        lcd.print(midiChannel);
    } else {
        lcd.print("Bank: ");
        lcd.print(currentBank);
        lcd.setCursor(0, 1);
        lcd.print("Program: ");
        lcd.print(currentProgram);
    }
}

// Sends a MIDI Program Change message
void sendMidiProgramChange(int programNumber) {
    Serial.write(0xC0 | ((midiChannel - 1) & 0x0F));
    Serial.write(programNumber - 1); // 0-indexed for MIDI
}

// Detects rising edge on a button press
bool buttonPressed(int index) {
    return buttonStates[index] == LOW && previousButtonStates[index] == HIGH;
}
