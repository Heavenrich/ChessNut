#include "Clock.h"

Clock::Clock(Lcd *l) :
  lcd(l)
{
}

void Clock::set(short white, short black) {
  if (white > 0 && black > 0) {
    enabled = true;
    whiteTime = long(white) * 60 * 1000;
    blackTime = long(black) * 60 * 1000;
    lcd->clearLine(1);
    lcd->writeClock(1, white * 60);
    lcd->writeClock(-1, black * 60);
  } else {
    enabled = false;
  }
  started = false;
}

void Clock::writeTimes() {
  lcd->clearLine(1);
  lcd->writeClock(1, short(whiteTime / 1000));
  lcd->writeClock(-1, short(blackTime / 1000));
}

void Clock::start() {
  time = millis();
  started = true;
}

boolean Clock::loop(short whosTurn) {
  if (!enabled || !started) {
    return true;
  }
  boolean timeRanOut = false;
  if (whiteTime > 0 && blackTime > 0) {
    long newTime = millis();
    if (whosTurn == 1) {
      long newWhiteTime = whiteTime - (newTime - time);
      if (newWhiteTime / 1000 != whiteTime / 1000) {
        lcd->writeClock(whosTurn, short(newWhiteTime / 1000));
      }
      whiteTime = newWhiteTime;
      if (whiteTime <= 0) {
        timeRanOut = true;
      }
    } else if (whosTurn == -1) {
      long newBlackTime = blackTime - (newTime - time);
      if (newBlackTime / 1000 != blackTime / 1000) {
        lcd->writeClock(whosTurn, short(newBlackTime / 1000));
      }
      blackTime = newBlackTime;
      if (blackTime <= 0) {
        timeRanOut = true;
      }
    }
    time = newTime;
  } else {
    timeRanOut = true;
  }

  return !timeRanOut;
}