#include <DotSD.h>

const int chipSelect = BUILTIN_SDCARD;

#define BUFFPIXEL     512
#define BMP_SIGNATURE 0x4D42
int paintSpeed = 15;

DotSD::DotSD() {};

void DotSD::init() {
  if (DotSD_DEBUG) Serial.println("Init SD");

  // if (!card.init(SPI_HALF_SPEED, chipSelect)) {
  //   Serial.println("initialization failed. Things to check:");
  //   Serial.println("* is a card inserted?");
  //   Serial.println("* is your wiring correct?");
  //   Serial.println("* did you change the chipSelect pin to match your shield or module?");
  //   return;
  // } else {
  //  Serial.println("Wiring is correct and a card is present."); 
  // }

  if (!SD.begin(chipSelect)) {
    if (DotSD_DEBUG) Serial.println("initialization failed!");
    return;
  }

  root = SD.open("/meow/");

  if (DotSD_DEBUG) Serial.println("SD Ready");
}

void DotSD::cardInfo() {
  // we'll use the initialization code from the utility libraries
  // since we're just testing if the card is working!
  // if (!card.init(SPI_HALF_SPEED, chipSelect)) {
  //   Serial.println("initialization failed. Things to check:");
  //   Serial.println("* is a card inserted?");
  //   Serial.println("* is your wiring correct?");
  //   Serial.println("* did you change the chipSelect pin to match your shield or module?");
  //   return;
  // } else {
  //  Serial.println("Wiring is correct and a card is present."); 
  // }

  // // print the type of card
  // Serial.print("\nCard type: ");
  // switch(card.type()) {
  //   case SD_CARD_TYPE_SD1:
  //     Serial.println("SD1");
  //     break;
  //   case SD_CARD_TYPE_SD2:
  //     Serial.println("SD2");
  //     break;
  //   case SD_CARD_TYPE_SDHC:
  //     Serial.println("SDHC");
  //     break;
  //   default:
  //     Serial.println("Unknown");
  // }

  // // Now we will try to open the 'volume'/'partition' - it should be FAT16 or FAT32
  // if (!volume.init(card)) {
  //   Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
  //   return;
  // }

  // // print the type and size of the first FAT-type volume
  // uint32_t volumesize;
  // Serial.print("\nVolume type is FAT");
  // Serial.println(volume.fatType(), DEC);
  // Serial.println();
  
  // volumesize = volume.blocksPerCluster();    // clusters are collections of blocks
  // volumesize *= volume.clusterCount();       // we'll have a lot of clusters
  // if (volumesize < 8388608ul) {
  //   Serial.print("Volume size (bytes): ");
  //   Serial.println(volumesize * 512);        // SD card blocks are always 512 bytes
  // }
  // Serial.print("Volume size (Kbytes): ");
  // volumesize /= 2;
  // Serial.println(volumesize);
  // Serial.print("Volume size (Mbytes): ");
  // volumesize /= 1024;
  // Serial.println(volumesize);

  
  // Serial.println("\nFiles found on the card (name, date and size in bytes): ");
  // root.openRoot(volume);
  
  // // list all files in the card with date and size
  // root.ls(LS_R | LS_DATE | LS_SIZE);
}

void DotSD::openDirectory(char* dir){
  Serial.println("openDirectory fired");

  // root = SD.open(dir);

}

void DotSD::printDirectory(File dir, int numTabs) {
  // if (!volume.init(card)) {
  //   Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
  //   return;
  // }

  // root.openRoot(volume);
  // root.ls(LS_R | LS_DATE | LS_SIZE);

  dir.rewindDirectory();

  while(true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      //Serial.println("**nomorefiles**");
      // break;
      return;
    }
    for (uint8_t i=0; i<numTabs; i++) {
      Serial.print('\t');
    }
    Serial.print(entry.name());
    if (entry.isDirectory()) {
      Serial.println("/");
      printDirectory(entry, numTabs+1);
    } else {
      // files have sizes, directories do not
      Serial.print("\t\t");
      Serial.println(entry.size(), DEC);
    }
    entry.close();
  }
  Serial.println();
}

//*************Support Funcitons****************//
// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.
uint16_t read16(File& f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File& f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

bool DotSD::bmpDrawScale(const char *filename)
{
  File bmpFile;
  int bmpWidth, bmpHeight;             // W+H in pixels
  uint8_t bmpDepth;                    // Bit depth (currently must report 24)
  uint32_t bmpImageoffset;             // Start of image data in file
  uint32_t rowSize;                    // Not always = bmpWidth; may have padding
  uint8_t sdbuffer[3 * BUFFPIXEL];     // pixel in buffer (R+G+B per pixel)
  uint32_t povbuffer[BUFFPIXEL];       // pixel out buffer (16-bit per pixel)//////mg/////this needs to be 24bit per pixel////////
  uint32_t buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean goodBmp = false;             // Set to true on valid header parse
  boolean flip = true;                 // BMP is stored bottom-to-top
  int w, h, row, col;
  int r, g, b;
  uint32_t pos = 0, startTime = millis();
  uint8_t povidx = 0;
  boolean first = true;

  // Open requested file on SD card
  bmpFile = SD.open(filename);
  Serial.println(filename);
  // Parse BMP header
  uint16_t signature = read16(bmpFile);
  Serial.print("File signature: ");
  Serial.println(signature, HEX);

  if (signature == BMP_SIGNATURE) { // BMP signature
    Serial.print("File size: ");
    Serial.println(read32(bmpFile));
    // (void)read32(bmpFile);            // Read & ignore creator bytes
    // bmpImageoffset = read32(bmpFile); // Start of image data
  //   Serial.print("Image Offset: ");
  //   Serial.println(bmpImageoffset, DEC);
  //   // Read DIB header
  //   Serial.print("Header size: ");
  //   Serial.println(read32(bmpFile));
  //   bmpWidth = read32(bmpFile);
  //   bmpHeight = read32(bmpFile);
  //   if (read16(bmpFile) == 1)
  //   {                             // # planes — must be ‘1’
  //     bmpDepth = read16(bmpFile); // bits per pixel
  //     Serial.print("Bit Depth: ");
  //     Serial.println(bmpDepth);
  //     if ((bmpDepth == 24) && (read32(bmpFile) == 0))
  //     { // 0 = uncompressed

  //       goodBmp = true; // Supported BMP format — proceed!
  //       Serial.print("Image size: ");
  //       Serial.print(bmpWidth);
  //       Serial.print('x');
  //       Serial.println(bmpHeight);

  //       // BMP rows are padded (if needed) to 4-byte boundary
  //       rowSize = (bmpWidth * 3 + 3) & ~3;

  //       // If bmpHeight is negative, image is in top-down order.
  //       // This is not canon but has been observed in the wild.
  //       if (bmpHeight < 0)
  //       {
  //         bmpHeight = -bmpHeight;
  //         flip = false;
  //       }

  //       w = bmpWidth;
  //       h = bmpHeight;

  //       for (row = 0; row < h; row++)
  //       {
  //         if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
  //           pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
  //         else // Bitmap is stored top-to-bottom
  //           pos = bmpImageoffset + row * rowSize;
  //         if (bmpFile.position() != pos)
  //         { // Need seek?
  //           bmpFile.seek(pos);
  //           buffidx = sizeof(sdbuffer); // Force buffer reload
  //         }

  //         for (col = 0; col < w; col++)
  //         { // For each column…
  //           // read more pixel data
  //           if (buffidx >= sizeof(sdbuffer))
  //           {
  //             povidx = 0;
  //             bmpFile.read(sdbuffer, sizeof(sdbuffer));
  //             buffidx = 0; // Set index to beginning
  //           }
  //           // set pixel
  //           r = sdbuffer[buffidx++];
  //           g = sdbuffer[buffidx++];
  //           b = sdbuffer[buffidx++];
  //           Serial.print(r);
  //           Serial.print(" ");
  //           Serial.print(g);
  //           Serial.print(" ");
  //           Serial.println(b);
  //           //we need to output BRG 24bit colour//
  //           povbuffer[povidx++] = (b << 16) + (g << 8) + r;
  //         }

  //         // for (int i = 0; i < NUM_LEDS; i++)
  //         // {
  //         //   leds[i] = povbuffer[i];
  //         // }
  //         // FastLED.show();
  //         delay(paintSpeed); // change the delay time depending effect required
  //       }                    // end scanline

  //     } // end goodBmp
  //   }
  } //end of IF BMP
  // Serial.println();

  bmpFile.close();
}

void DotSD::printFile(const char* fileName) {
  File file = SD.open(fileName);
  // Serial.print("File size: ");
  // Serial.println(read32(file));
  // (void)read32(file);

  unsigned long size = file.size();

  Serial.print("File size is: "); Serial.println(size);
  file.close();
}


bool isBMP(const char filename []) {
    if (filename[0] == '_')
        return false;

    if (filename[0] == '~')
        return false;

    if (filename[0] == '.')
        return false;

    String filenameString = String(filename).toUpperCase();
    if (filenameString.endsWith(".BMP") != 1)
        return false;

    return true;
}

void DotSD::getBMPFilenameByIndex(const char *directoryName, int index, char *pnBuffer) {

    char* filename;

    // Make sure index is in range
    if ((index < 0) || (index >= 10))
        return;

    File directory = SD.open(directoryName);
    if (!directory)
        return;

    File file = directory.openNextFile();
    while (file && (index >= 0)) {
        filename = file.name();

        Serial.println(filename);

        if (isBMP(file.name())) {
            index--;

            // Copy the directory name into the pathname buffer
            strcpy(pnBuffer, directoryName);

            // Append the filename to the pathname
            strcat(pnBuffer, filename);
        }

        file.close();
        file = directory.openNextFile();
    }

    file.close();
    directory.close();
}