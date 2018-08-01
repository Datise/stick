#ifndef DotSD_h
#define DotSD_h

#include "Arduino.h"
#include <SD.h>
#include <SPI.h>

#define DotSD_DEBUG true

class DotSD {
  public:
    DotSD();
    void init();

    struct BmpImage {
      unsigned int* image_array;
      int w;
      int h;
    };

    void cardInfo();
    void openDirectory(char* dir);
    void printDirectory(char* directory, int numTabs);
    BmpImage readBmp(const char *filename);
    void printFile(const char* fileName);
    void getBMPFilenameByIndex(const char *directoryName, int index, char *pnBuffer);

    File root;
};

#endif