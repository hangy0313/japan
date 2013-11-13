#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "bmpHeader.h"

#define bmpWidth 600 
#define bmpHeight 5800

#define newHeight 120
#define newWidth 120

#define heightStartWrite 20
#define widthStartWrite 20

#define FALSE 0
#define TRUE 1

static unsigned char data[bmpHeight][bmpWidth][3];
static unsigned char newData[newHeight][newWidth][3] = {255, 255, 255};

int main(int argc, char *argv[]) {
    struct bmpHeader bmpHeader;
    FILE *bmp, *seg, *horiPlot, *vertPlot;

    unsigned char white[3] = {255, 255, 255};
    unsigned char cmd[120];

    unsigned long height, width, num, color, count, tmpHeight, tmpWidth, i, j;
    unsigned long black = 0, vert[bmpWidth], hori[bmpHeight];
    unsigned long heightLocation[72] = {0}, widthLocation[10] = {0};
    unsigned long fileNum = 12534;
    unsigned long heightArrayNum, widthArrayNum;

    long check = 0;

    bool heightStart = TRUE, widthStart = TRUE;

    bmp = fopen("originalData/word_24bit.bmp", "rb");
    horiPlot = fopen("hori.txt", "wt");
    vertPlot = fopen("vert.txt", "wt");
//    seg = fopen("seg.bmp", "wb");

    fread(&bmpHeader, sizeof(struct bmpHeader), 1, bmp);
    fread(data, sizeof(unsigned char), bmpHeight*bmpWidth*3, bmp);
//    fwrite(&bmpHeader, sizeof(struct bmpHeader), 1, seg);
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
    for(height = 400;height < 5000;height++){
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

    for(width = 0;width < bmpWidth;width++){
        vert[width] = 0;
        for(height = 0;height < bmpHeight;height++){
            color = 0;
            for(num = 0;num < 3;num++){
                color += data[height][width][num];
            }
            if(color == black){
                vert[width]++;
            }
        }
        vert[width] -= 600;
        fprintf(vertPlot, "%ld\n", vert[width]);
    }

    num = 0;
    for(width = 0;width < bmpWidth;width++){
        if(widthStart){
            if(vert[width] == 0 && vert[width+1] != 0){
                widthLocation[num] = width;
                num++;
                widthStart = FALSE;
                continue;
            }
        }else{
            if(vert[width] != 0 && vert[width+1] == 0){
                widthLocation[num] = width;
                num++;
                widthStart = TRUE;
                continue;
            }
        }
    }

//    for(height = 0;height < bmpHeight;height++){
//        for(width = 0;width < bmpWidth;width++){
//            for(num = 0;num < sizeof(heightLocation)/4;num++){
//                if(height == heightLocation[num]){
//                    data[height][width][0] = 0;
//                    data[height][width][1] = 0;
//                    data[height][width][2] = 255;
//                }
//            }
//        }
//    }
//    for(height = 0;height < bmpHeight;height++){
//        for(width = 0;width < bmpWidth;width++){
//            for(num = 0;num < sizeof(widthLocation)/4;num++){
//                if(width == widthLocation[num]){
//                    data[height][width][0] = 0;
//                    data[height][width][1] = 0;
//                    data[height][width][2] = 255;
//                }
//            }
//        }
//    }

heightArrayNum = 0;
for(count = 0;count < 35;count++){
    widthArrayNum = 0;
    for(num = 0;num < 5;num++){
        sprintf(cmd, "./testSeg/word_%ld.bmp", fileNum);
        seg = fopen(cmd, "wb");
        fwrite(&bmpHeader, sizeof(struct bmpHeader), 1, seg);

        tmpHeight = heightStartWrite + heightLocation[heightArrayNum+1] - heightLocation[heightArrayNum];
        j = heightLocation[heightArrayNum];
        for(height = 0;height < newHeight;height++){
            tmpWidth = widthStartWrite + widthLocation[widthArrayNum+1] - widthLocation[widthArrayNum];
            i = widthLocation[widthArrayNum];
            for(width = 0;width < newWidth;width++){
                if(height >= heightStartWrite && height <= tmpHeight && width >= widthStartWrite && width <= tmpWidth){
                    fwrite(&data[j][i], sizeof(unsigned char), 3, seg);
                    i++;
                    continue;
                }
                fwrite(white, sizeof(unsigned char), 3, seg);
            }
            if(height >= heightStartWrite && height <= tmpHeight){
                j++;
            }
        }
        fclose(seg);
        widthArrayNum += 2;
        fileNum++;
    }
    heightArrayNum += 2;
    fileNum -=10;
    if(count == 17){
        fileNum = 12433;
    }
}
//    fwrite(data, sizeof(unsigned char), bmpHeight*bmpWidth*3, seg);

    fclose(bmp);
    fclose(horiPlot);
    fclose(vertPlot);
//    fclose(seg);

    return 0;
}
