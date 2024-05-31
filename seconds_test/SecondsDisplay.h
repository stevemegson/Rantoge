#ifndef SECONDSDISPLAY_H
#define SECONDSDISPLAY_H

#include "tft_settings.h"
#include <TFT_eSPI.h>

#define ARC_THICKNESS 10
#define BACKGROUND_COLOR TFT_BLACK
#define DIGIT_COLOR TFT_WHITE
#define ARC_COLOR TFT_GOLD

class SecondsDisplay {
public:
  void begin();
  void update(int currentMinute, int currentSecond);

private:
  TFT_eSPI tft = TFT_eSPI();
  int lastMinute = 0;
  int lastSecond = 0;

  void draw7Number(int n, unsigned int xLoc, unsigned int yLoc, char cS, unsigned int fC, unsigned int bC, char nD);
};

constexpr int draw7Width(int cS, int nD) {
  return (nD * (11 * cS + 2)) - (2 * cS);
}

constexpr int draw7Height(int cS) {
  return 16 * cS + 4;
}

constexpr int digitSize = 7;
constexpr int digitX = (TFT_WIDTH / 2) - (draw7Width(digitSize, 2) / 2);
constexpr int digitY = (TFT_HEIGHT / 2) - (draw7Height(digitSize) / 2);

constexpr int arcCenterX = TFT_WIDTH / 2;
constexpr int arcCenterY = TFT_HEIGHT / 2;
constexpr int arcRadius = (TFT_WIDTH > TFT_HEIGHT ? TFT_HEIGHT / 2 : TFT_WIDTH / 2) - (ARC_THICKNESS / 2);
constexpr int arcInnerRadius = arcRadius - (ARC_THICKNESS / 2);

#endif