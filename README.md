# Smart Safe Puzzle Box 🔐
Three interlocked Arduino puzzles (RFID → Simon Says → IR Math). Solve in order to unlock a motorized safe.

https://github.com/<your-username>/smart-safe-puzzle-box

## Why it’s cool
- 3 Arduino Uno R4 WiFi boards collaborate via serial messages
- Modular puzzles with LCD feedback, LEDs, buzzer, and a servo/DC lock
- Enforced sequence (can’t skip ahead)

## Demo
- Short video: `media/demo.mp4`
- GIF: `media/demo.gif`

## Hardware
- 3 × Arduino Uno R4 WiFi
- MFRC522 RFID reader (3.3V)
- IR receiver + IR remote
- 1 × Servo or DC motor + driver
- LCD (I2C)
- RGB LED(s), 3 × buttons, buzzer, resistors, wiring

## Libs / Dependencies
- `MFRC522` (RFID)
- `IRremote` (v3 style, uses `#include <IRremote.hpp>`)
- `LiquidCrystal_I2C`
- `SoftwareSerial`
- `SPI`

> Install via Arduino Library Manager where possible.

## Wiring (high level)
- **Puzzle 1 (RFID + Lock)**: MFRC522 on SPI (SS=10, RST=9 by default), motor pins to driver, status LEDs + buzzer.
- **Puzzle 2 (Simon Says)**: RGB LED pins (PWM), 3 buttons (with pull-down or INPUT_PULLUP), LCD via I2C.
- **Puzzle 3 (IR Math)**: IR receiver on a digital pin (e.g., D2), LCD via I2C.

Add detailed pin maps and a Fritzing diagram under `hardware/wiring-diagrams/`.

## Serial protocol between boards
Simple one-byte messages:
- `1` → start next puzzle / success
- `2` → Simon correct (show “GOOD JOB” on shared LCD)
- `3` → Simon wrong (show “TRY AGAIN”)


## How to run
1. Open each folder’s `.ino` in Arduino IDE.
2. Select **Arduino Uno R4 WiFi** as the board.
3. Upload each sketch to its own board.
4. Connect serial jumpers between boards per your wiring diagram.
5. Start with Puzzle 1 (RFID). On success it signals Puzzle 2, then Puzzle 3, then unlocks.

## Notes
- RFID module must be **3.3V** (not 5V).
- If PWM flicker/brightness looks off, ensure your LED pins support PWM on Uno R4.
- If you see IR noise, add a small debounce/cooldown (already included).

## License
MIT – see `LICENSE`.

## Credits
Built by Malak Ramahi & team. Design doc in `docs/`.

