/*  Copyright (c) 2021, Collab
 *  All rights reserved
*/
#ifndef Utils_h
#define Utils_h

#include <Arduino.h>

class Utils {
  public:
    static long splitHourString(String data, char separator, int index) {
      int found = 0;
      int strIndex[] = {0, -1};
      int maxIndex = data.length() - 1;

      for (int i = 0; i <= maxIndex && found <= index; i++) {
          if (data.charAt(i) == separator || i == maxIndex) {
              found++;
              strIndex[0] = strIndex[1] + 1;
              strIndex[1] = (i == maxIndex) ? i + 1 : i;
          }
      }
      String result = found > index ? data.substring(strIndex[0], strIndex[1]) : "";
      return result.toInt();
    }

    static const char * const BoolToString(bool b) {
      return b ? "Yes" : "No";
    }
};

#endif
