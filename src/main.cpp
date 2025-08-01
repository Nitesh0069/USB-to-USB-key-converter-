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
            case 0x05:   // 'b' key
            Keyboard.press(KEY_LEFT_CTRL);   // Hold Ctrl
            Keyboard.press(KEY_LEFT_ALT);    // Hold Alt
            Keyboard.press('f');             // Press F
            delay(10);                       // Short delay
            Keyboard.releaseAll();          // Release Ctrl + Alt + F
            return 0;                        // Do not send 'b' or 'B'

            case 0x06:  // 'C'
            Keyboard.press(KEY_LEFT_CTRL);  // Hold Ctrl
            Keyboard.press('c');            // Press C
            delay(10);                       // Short delay
            Keyboard.releaseAll();           // Release Ctrl + C
            return 0; // Don't send 'C' again

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
            case 0x19:  // 'V'
            Keyboard.press(KEY_LEFT_CTRL);  // Hold Ctrl
            Keyboard.press('v');            // Press V
            delay(10);                       // Short delay
            Keyboard.releaseAll();           // Release Ctrl + V
            return 0;
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

            // Symbols and special characters
            case 0x2D: return shift ? '_' : '-';
            case 0x2E: return shift ? '+' : '=';
            case 0x2F: return shift ? '{' : '[';
            case 0x30: return shift ? '}' : ']';
            case 0x31: return shift ? '|' : '\\';
            case 0x33: return shift ? ':' : ';';
            case 0x34: return shift ? '"' : '\'';
            case 0x35: return shift ? '~' : '`';
            case 0x36: return shift ? '<' : ',';
            case 0x37: return shift ? '>' : '.';
            case 0x38: return shift ? '?' : '/';

            // Special keys
            case 0x2C: return KEY_F24; // Spacebar
            case 0x28: return '\n'; // Enter
            case 0x2A: return '\b'; // Backspace
            case 0x2B: return '\t'; // Tab
            case 0x29: return KEY_ESC; // ASCII for Escape (ESC)
                
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
         
            // Numpad operators now affected by Num Lock
            case 0x54: return numLockActive ? '/' : 0;
            case 0x55: return numLockActive ? '*' : 0;
            case 0x56: return numLockActive ? '-' : 0;
            case 0x57: return numLockActive ? '+' : 0;
            case 0x58: return numLockActive ? '\n' : 0;
            case 0x63: return numLockActive ? '.' : 0;
            // Function keys (F1-F12)
            case 0x3A: return KEY_F1;
            case 0x3B: return KEY_F2;
            case 0x3C: return KEY_F3;
            case 0x3D: return KEY_F4;
            case 0x3E: return KEY_F5;
            case 0x3F: return KEY_F6;
            case 0x40: return KEY_F7;
            case 0x41: return KEY_F8;
            case 0x42: return KEY_F9;
            case 0x43: return KEY_F10;
            case 0x44: return KEY_F11;
            case 0x45: return KEY_F12;

            // Page up, Page down, Home, End, Insert, Delete
            case 0x4B: return KEY_PAGE_UP;// Page up
            case 0x4E: 
            Keyboard.press(KEY_PAGE_DOWN);// Page down
            Keyboard.press(KEY_F13);
            delay(10);
            Keyboard.releaseAll();
            return 0;
            case 0x4A: return KEY_HOME;// Home
            case 0x4D:
            Keyboard.press(KEY_END);
            Keyboard.press(KEY_F13);
            delay(10);
            Keyboard.releaseAll();
            return 0;
            // End
            case 0x49: return KEY_INSERT;// Insert
            case 0x4C: return KEY_DELETE;// Delete

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