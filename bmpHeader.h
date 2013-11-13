struct bmpHeader {
    unsigned char id[2];     //BM

    unsigned long fileSize;
    unsigned long reserved;
    unsigned long offset;

    unsigned long headerSize;
    unsigned long width;
    unsigned long height;
    short planes;
    short bitsPerPixel;
    
    unsigned long compression;
    unsigned long imageSize;
    unsigned long xPixelPerMeter;
    unsigned long yPixelPerMeter;
    unsigned long colorUsed;
    unsigned long importantColor;
}__attribute__((__packed__));
