#include "SecondsDisplay.h"

void SecondsDisplay::begin() {
  tft.init();
  tft.fillScreen(BACKGROUND_COLOR);
}

void SecondsDisplay::update(int currentMinute, int currentSecond) {
  if (currentMinute != lastMinute && currentSecond > 0) {
    lastMinute = currentMinute;
    tft.drawSmoothArc(arcCenterX, arcCenterY, arcRadius, arcInnerRadius, 0, 360, BACKGROUND_COLOR, BACKGROUND_COLOR, false);
  }

  if (lastSecond != currentSecond) {
    lastSecond = currentSecond;
    tft.drawSmoothArc(arcCenterX, arcCenterY, arcRadius, arcInnerRadius, 180, (currentSecond * 6 + 180) % 360, ARC_COLOR, BACKGROUND_COLOR, true);
    draw7Number(currentSecond, digitX, digitY, digitSize, DIGIT_COLOR, BACKGROUND_COLOR, 2);
  }
}

// by William Zaggle, https://forum.arduino.cc/t/fast-7-segment-number-display-for-tft/296619
void SecondsDisplay::draw7Number(int n, unsigned int xLoc, unsigned int yLoc, char cS, unsigned int fC, unsigned int bC, char nD) {
  unsigned int num = abs(n), i, s, t, w, col, h, a, b, si = 0, j = 1, d = 0;
  unsigned int S2 = 5 * cS;              // width of horizontal segments   5 times the cS
  unsigned int S3 = 2 * cS;              // thickness of a segment 2 times the cs
  unsigned int S4 = 5 * cS;              // height of vertical segments 5 times the cS
  unsigned int x1 = cS + 1;              // starting x location of horizontal segments
  unsigned int x2 = S3 + S2 + 1;         // starting x location of right side segments
  unsigned int y1 = yLoc + x1;           // starting y location of top side segments
  unsigned int y3 = yLoc + S3 + S4 + 1;  // starting y location of bottom side segments

  unsigned int seg[7][3] = { { x1, yLoc, 1 }, { x2, y1, 0 }, { x2, y3 + x1, 0 }, { x1, (2 * y3) - yLoc, 1 }, { 0, y3 + x1, 0 }, { 0, y1, 0 }, { x1, y3, 1 } };  // actual x,y locations of all 7 segments with direction
  unsigned char nums[12] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67, 0x00, 0x40 };                                                          // segment defintions for all 10 numbers plus blank and minus sign

  unsigned char c, cnt;

  c = abs(cS);  // get character size between 1 and 10 ignoring sign
  if (c > 10) c = 10;
  if (c < 1) c = 1;

  cnt = abs(nD);  // get number of digits between 1 and 10 ignoring sign
  if (cnt > 10) cnt = 10;
  if (cnt < 1) cnt = 1;

  xLoc += (cnt - 1) * (d = S2 + (3 * S3) + 2);  // set starting x at last digit location

  while (cnt > 0) {  // for cnt number of places

    --cnt;

    if (num > 9) i = num % 10;        //get the last digit
    else if (!cnt && n < 0) i = 11;   //show minus sign if 1st position and negative number
    else if (nD < 0 && !num) i = 10;  //show blanks if remaining number is zero
    else i = num;

    num = num / 10;  // trim this digit from the number

    for (j = 0; j < 7; ++j) {  // draw all seven segments

      if (nums[i] & (1 << j)) col = fC;  // if segment is On use foreground color
      else col = bC;                     // else use background color

      if (seg[j][2]) {

        w = S2;                     // Starting width of segment (side)
        t = seg[j][1] + S3;         // maximum thickness of segment
        h = seg[j][1] + cS;         // half way point thickness of segment
        a = xLoc + seg[j][0] + cS;  // starting x location
        b = seg[j][1];              // starting y location

        while (b < h) {                     // until x location = half way
          tft.drawFastHLine(a, b, w, col);  //Draw a horizontal segment top
          a--;                              // move the x position by -1
          b++;                              // move the y position by 1
          w += 2;                           // make the line wider by 2
        }

      } else {

        w = S4;                     // Starting height of segment (side)
        t = xLoc + seg[j][0] + S3;  // maximum thickness of segment
        h = xLoc + seg[j][0] + cS;  // half way point thickness of segment
        a = seg[j][1] + cS;         // starting y location
        b = xLoc + seg[j][0];       // starting x location

        while (b < h) {                     // until x location = half way
          tft.drawFastVLine(b, a, w, col);  // Draw a vertical line right side
          a--;                              //  move the y position by -1
          b++;                              //  move teh x position by 1
          w += 2;                           //  make the line wider by 2
        }
      }

      while (b < t) {  //finish drawing horizontal bottom or vertical left side of segment
        if (seg[j][2]) {
          tft.drawFastHLine(a, b, w, col);  // Draw Horizonal line bottom
        } else {
          tft.drawFastVLine(b, a, w, col);  // Draw Vertical line left side
        }
        b++;     // keep moving the x or y draw position until t
        a++;     // move the length or height starting position back the other way.
        w -= 2;  // move the length or height back the other way
      }
    }

    xLoc -= d;  // move to next digit position
  }
}