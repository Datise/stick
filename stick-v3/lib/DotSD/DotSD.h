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

    void cardInfo();
    void openDirectory(char* dir);
    void printDirectory(File dir, int numTabs);
    bool bmpDrawScale(const char *filename);
    void printFile(const char* fileName);

    File root;
};

#endif