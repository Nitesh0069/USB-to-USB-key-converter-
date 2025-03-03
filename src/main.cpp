#include <usbhub.h>
#include <hiduniversal.h>
#include <SPI.h>
#include <Keyboard.h>  // Arduino Leonardo acts as a keyboard

#define LED_PIN 12  // LED connected to pin 12

USB Usb;
HIDUniversal Hid(&Usb);
bool numLockActive = false; // Track Num Lock state

class KeyboardHandler : public HIDReportParser {
protected:
    void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) override {
        if (len > 0) {
            uint8_t modifiers = buf[0]; // Modifier keys (Shift, Ctrl, Alt)
            bool shiftPressed = (modifiers & 0x22); // Left Shift (0x02) or Right Shift (0x20)

            for (uint8_t i = 2; i < len; i++) { // Start from index 2 (actual keypresses)
                if (buf[i] > 0) {
                    uint8_t keycode = buf[i]; // Get keycode
                    if (keycode == 0x53) { // Num Lock key
                        numLockActive = !numLockActive; // Toggle Num Lock state
                        digitalWrite(LED_PIN, numLockActive ? HIGH : LOW); // Control LED
                        Serial.print("Num Lock: ");
                        Serial.println(numLockActive ? "ON" : "OFF");
                    }
                    char c = KeycodeToAscii(keycode, shiftPressed);
                    
                    if (c) {
                        Keyboard.press(c);   // Press key
                        delay(10);           // Small delay
                        Keyboard.release(c); // Release key
                    }
                }
            }
        }
    }

    char KeycodeToAscii(uint8_t keycode, bool shift) {
        switch (keycode) {
            // Letter keys (A-Z)
            case 0x04: return shift ? 'A' : 'a';
            case 0x05: return shift ? 'B' : 'b';
            case 0x06: return shift ? 'C' : 'c';
            case 0x07: return shift ? 'D' : 'd';
            case 0x08: return shift ? 'E' : 'e';
            case 0x09: return shift ? 'F' : 'f';
            case 0x0A: return shift ? 'G' : 'g';
            case 0x0B: return shift ? 'H' : 'h';
            case 0x0C: return shift ? 'I' : 'i';
            case 0x0D: return shift ? 'J' : 'j';
            case 0x0E: return shift ? 'K' : 'k';
            case 0x0F: return shift ? 'L' : 'l';
            case 0x10: return shift ? 'M' : 'm';
            case 0x11: return shift ? 'N' : 'n';
            case 0x12: return shift ? 'O' : 'o';
            case 0x13: return shift ? 'P' : 'p';
            case 0x14: return shift ? 'Q' : 'q';
            case 0x15: return shift ? 'R' : 'r';
            case 0x16: return shift ? 'S' : 's';
            case 0x17: return shift ? 'T' : 't';
            case 0x18: return shift ? 'U' : 'u';
            case 0x19: return shift ? 'V' : 'v';
            case 0x1A: return shift ? 'W' : 'w';
            case 0x1B: return shift ? 'X' : 'x';
            case 0x1C: return shift ? 'Y' : 'y';
            case 0x1D: return shift ? 'Z' : 'z';

            // Number keys (0-9, differentiating numpad keys)
            case 0x1E: return shift ? '!' : '1';
            case 0x1F: return shift ? '@' : '2';
            case 0x20: return shift ? '#' : '3';
            case 0x21: return shift ? '$' : '4';
            case 0x22: return shift ? '%' : '5';
            case 0x23: return shift ? '^' : '6';
            case 0x24: return shift ? '&' : '7';
            case 0x25: return shift ? '*' : '8';
            case 0x26: return shift ? '(' : '9';
            case 0x27: return shift ? ')' : '0';
            
            // Numpad keys (now dependent on Num Lock state)
            case 0x62: return numLockActive ? '0' : 0;
            case 0x59: return numLockActive ? '1' : 0;
            case 0x5A: return numLockActive ? '2' : 0;
            case 0x5B: return numLockActive ? '3' : 0;
            case 0x5C: return numLockActive ? '4' : 0;
            case 0x5D: return numLockActive ? '5' : 0;
            case 0x5E: return numLockActive ? '6' : 0;
            case 0x5F: return numLockActive ? '7' : 0;
            case 0x60: return numLockActive ? '8' : 0;
            case 0x61: return numLockActive ? '9' : 0;

            // Special keys
            case 0x2C: return ' '; // Spacebar
            case 0x28: return '\n'; // Enter
            case 0x2A: return '\b'; // Backspace
            case 0x2B: return '\t'; // Tab
            default: return 0;
        }
    }
};

KeyboardHandler MyKeyboard;

void setup() {
    Serial.begin(115200);
    Keyboard.begin();
    pinMode(LED_PIN, OUTPUT); // Set LED pin as output

    if (Usb.Init() == -1) {
        Serial.println("USB Host Shield initialization failed.");
        while (1);
    }
    Hid.SetReportParser(0, &MyKeyboard);
    Serial.println("Ready to read keyboard input.");
}

void loop() {
    Usb.Task(); // Keep checking for updates
}