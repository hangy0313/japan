#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "bmpHeader.h"

#define bmpWidth 600 
#define bmpHeight 5800

#define newHeight 120
#define newWidth 600

#define heightStartWrite 10

#define FALSE 0
#define TRUE 1

static unsigned char data[bmpHeight][bmpWidth][3];
static unsigned char newData[newHeight][newWidth][3] = {255, 255, 255};

int main(int argc, char *argv[]) {
    struct bmpHeader bmpHeader;
    FILE *bmp, *seg, *horiPlot, *vertPlot;

    unsigned char white[3] = {255, 255, 255};
    unsigned char cmd[120];

    unsigned long height, width, num, color, count, tmp, i, j;
    unsigned long black = 0, vert[bmpWidth], hori[bmpHeight];
    unsigned long heightLocation[80] = {0}, widthLocation[10];
    unsigned long fileNum = 176;

    bool heightStart = TRUE;

    bmp = fopen("originalData/hira_3_24bit.bmp", "rb");
    horiPlot = fopen("hori.txt", "wt");

    fread(&bmpHeader, sizeof(struct bmpHeader), 1, bmp);
    fread(data, sizeof(unsigned char), bmpHeight*bmpWidth*3, bmp);    
    bmpHeader.width = newWidth;
    bmpHeader.height = newHeight;
    bmpHeader.fileSize = bmpHeader.width * bmpHeader.height * 3 + 54;
    bmpHeader.imageSize  =  bmpHeader.width * bmpHeader.height * 3 ;

    for(height = 0;height < bmpHeight;height++){
        hori[height] = 0;
        for(width = 0;width < bmpWidth;width++){
            color = 0;
            for(num = 0;num < 3;num++){
                color += data[height][width][num];
            }
            if(color == black){
                hori[height]++;
            }
        }
        fprintf(horiPlot, "%ld\n", hori[height]);
    }

    num = 0;
    for(height = 0;height < 5200;height++){
        if(heightStart){
            if(hori[height] == 0 && hori[height+1] != 0){
                heightLocation[num] = height;
                num++;
                heightStart = FALSE;
                continue;
            }
        }else{
            if(hori[height] != 0 && hori[height+1] == 0){
                heightLocation[num] = height;
                num++;
                heightStart = TRUE;
                continue;
            }
        }
    }

    count = 0;
    for(num = 0;num < 40;num++){
        sprintf(cmd, "./testSeg/phrase_%03ld.bmp", fileNum);
        seg = fopen(cmd, "wb");
        fwrite(&bmpHeader, sizeof(struct bmpHeader), 1, seg);

        tmp = heightStartWrite + heightLocation[count+1] - heightLocation[count];
        j = heightLocation[count];
        for(height = 0;height < newHeight;height++){
            for(width = 0;width < newWidth;width++){
                if(height >= heightStartWrite && height <= tmp){
                    fwrite(&data[j][width], sizeof(unsigned char), 3, seg);
                    continue;
                }
                fwrite(white, sizeof(unsigned char), 3, seg);
            }
            if(height >= heightStartWrite && height <= tmp){
                j++;
            }
        }
        fclose(seg);
        count += 2;
        fileNum++;
    }
    
    fclose(bmp);
    fclose(horiPlot);

    return 0;
}
