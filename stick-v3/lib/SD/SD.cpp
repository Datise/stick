// bool bmpDrawScale(const char *filename, uint8_t x, uint16_t y, int scale, int maxheight)
// {

//   File bmpFile;
//   int bmpWidth, bmpHeight;                       // W+H in pixels
//   uint8_t bmpDepth;                              // Bit depth (currently must be 24)
//   uint32_t bmpImageoffset;                       // Start of image data in file
//   uint32_t rowSize;                              // Not always = bmpWidth; may have padding
//   uint8_t sdbuffer[3 * BUFFPIXEL * ROWSPERDRAW]; // pixel buffer (R+G+B per pixel)
//   uint32_t buffidx = sizeof(sdbuffer);           // Current position in sdbuffer
//   boolean goodBmp = false;                       // Set to true on valid header parse
//   boolean flip = true;                           // BMP is stored bottom-to-top
//   int row, col;
//   uint8_t r, g, b;
//   uint32_t pos = 0, startTime = millis();

//   if ((x >= tft.width()) || (y >= tft.height()))
//     return false;

//   Serial.println();
//   Serial.print("Filename: ");
//   Serial.println(filename);

//   // Open requested file on SD card
//   if (!(bmpFile.open(filename)))
//   {
//     Serial.print(F("File not found"));
//     return false;
//   }

//   // Parse BMP header
//   if (read16(bmpFile) == 0x4D42)
//   { // BMP signature
//     Serial.print(F("File size: "));
//     Serial.println(read32(bmpFile));
//     (void)read32(bmpFile);            // Read & ignore creator bytes
//     bmpImageoffset = read32(bmpFile); // Start of image data
//     Serial.print(F("Image Offset: "));
//     Serial.println(bmpImageoffset, DEC);
//     // Read DIB header
//     Serial.print(F("Header size: "));
//     Serial.println(read32(bmpFile));
//     bmpWidth = read32(bmpFile);
//     bmpHeight = read32(bmpFile);
//     if (bmpHeight < maxheight)
//     {
//       maxheight = bmpHeight;
//     }
//     if (read16(bmpFile) == 1)
//     {                             // # planes -- must be '1'
//       bmpDepth = read16(bmpFile); // bits per pixel
//       //Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
//       if ((bmpDepth == 24) && (read32(bmpFile) == 0))
//       { // 0 = uncompressed

//         goodBmp = true; // Supported BMP format -- proceed!
//         //Serial.print(F("Image size: "));
//         //Serial.print(bmpWidth);
//         //Serial.print('x');
//         //Serial.println(bmpHeight);

//         // BMP rows are padded (if needed) to 4-byte boundary
//         rowSize = (bmpWidth * 3 + 3) & ~3;

//         // If bmpHeight is negative, image is in top-down order.
//         // This is not canon but has been observed in the wild.
//         if (bmpHeight < 0)
//         {
//           bmpHeight = -bmpHeight;
//           flip = false;
//         }

//         for (row = 0; row < maxheight; row += ROWSPERDRAW)
//         { // For each scanline...

//           // Seek to start of scan line. It might seem labor-
//           // intensive to be doing this on every line, but this
//           // method covers a lot of gritty details like cropping
//           // and scanline padding. Also, the seek only takes
//           // place if the file position actually needs to change
//           // (avoids a lot of cluster math in SD library).
//           if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
//             pos = bmpImageoffset + (bmpHeight - ROWSPERDRAW - row) * rowSize;
//           else // Bitmap is stored top-to-bottom
//             pos = bmpImageoffset + row * rowSize;
//           if (bmpFile.position() != pos)
//           { // Need seek?
//             bmpFile.seek(pos);
//             buffidx = sizeof(sdbuffer); // Force buffer reload
//           }
//           for (int i = ROWSPERDRAW; i > 0; i--)
//           {
//             int index = 0;

//             for (col = 0; col < (bmpWidth); col++)
//             { // For each pixel...
//               // Time to read more pixel data?
//               if (buffidx >= sizeof(sdbuffer))
//               { // Indeed
//                 bmpFile.read(sdbuffer, sizeof(sdbuffer));
//                 buffidx = 0; // Set index to beginning
//               }

//               b = sdbuffer[buffidx++];
//               g = sdbuffer[buffidx++];
//               r = sdbuffer[buffidx++];
//               // Convert pixel from BMP to TFT format, push to display
//               if (i % scale == 0)
//               {
//                 if (index++ % scale == 0)
//                 {
//                   awColors[(index / scale) + (((i / scale) - 1) * bmpWidth / scale)] = tft.color565(r, g, b);
//                 }
//               } // end pixel
//             }
//           }
//           tft.writeRect(x, y + (row / scale), bmpWidth / scale, ROWSPERDRAW / scale, awColors);
//         } // end scanline
//         Serial.print(F("Loaded in "));
//         Serial.print(millis() - startTime);
//         Serial.println(" ms");
//       } // end goodBmp
//     }
//   }
//   else
//   {
//     return false;
//   }
//   bmpFile.close();
//   if (!goodBmp)
//     Serial.println(F("BMP format not recognized."));
//   return true;
// }