#if __has_include("wled.h") && __has_include("Arduino.h")
#include "wled.h"
#else
// Minimal stubs for environments where Arduino/WLED headers are not available
// (e.g. editor IntelliSense). These provide just enough declarations to allow
// this usermod file to be parsed without errors.
#include <stdint.h>

extern unsigned long millis();
inline void pinMode(int /*pin*/, int /*mode*/) {}
inline void digitalWrite(int /*pin*/, int /*val*/) {}
inline void tone(int /*pin*/, unsigned int /*frequency*/, unsigned long /*duration*/) {}
inline void noTone(int /*pin*/) {}

#define OUTPUT 1
#define LOW 0

// Minimal JSONVar stub
class JSONVar {
public:
  bool hasOwnProperty(const char* /*key*/) { return false; }
};

// Minimal Usermod base class stub
class Usermod {
public:
  virtual void setup() {}
  virtual void loop() {}
  virtual bool handleRemote(JSONVar &root) { (void)root; return false; }
  virtual ~Usermod() {}
};

#endif

// ======= CONFIG =======
#define BUZZER_PIN 15  // GPIO pin for passive buzzer
// ======================

class UsermodBuzzerMelody : public Usermod {
private:
  bool playMelody = false;
  unsigned long noteStart = 0;
  int currentNote = 0;

  // Define a simple melody (frequency in Hz, duration in ms)
  struct Note { int freq; int dur; };
  Note melody[8] = {
    { 1047, 150 }, // C6
    { 1319, 150 }, // E6
    { 1568, 150 }, // G6
    { 2093, 300 }, // C7
    { 0,    100 }, // pause
    { 1568, 150 },
    { 1319, 150 },
    { 1047, 200 }
  };

public:
  void setup() {
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
  }

  void loop() {
    if (playMelody) {
      unsigned long now = millis();
      if (currentNote < 8) {
        if (now - noteStart >= melody[currentNote].dur) {
          currentNote++;
          if (currentNote < 8) {
            noteStart = now;
            if (melody[currentNote].freq > 0)
              tone(BUZZER_PIN, melody[currentNote].freq, melody[currentNote].dur);
          } else {
            noTone(BUZZER_PIN);
            playMelody = false;
          }
        }
      } else {
        playMelody = false;
      }
    }
  }

  // Handle custom JSON command: {"melody":1}
  bool handleRemote(JSONVar &root) {
    if (root.hasOwnProperty("melody")) {
      playMelody = true;
      noteStart = millis();
      currentNote = 0;
      tone(BUZZER_PIN, melody[0].freq, melody[0].dur);
      return true;
    }
    return false;
  }
};

UsermodBuzzerMelody usermodBuzzerMelody;