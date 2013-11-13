#include <stdio.h>
#include <stdlib.h>

#include "bmpHeader.h"

int main(int argc, char *argv[]) {
    FILE *bmp;            //為了開檔產生一個讀取用檔案變數
    struct bmpHeader bmpHeader;
    
    bmp = fopen(argv[1], "r+b");
    
    fread(&bmpHeader,sizeof(struct bmpHeader),1,bmp);
    
    if(bmpHeader.id[0] != 'B' || bmpHeader.id[1]!='M') { 
        printf("\n\n該檔案不是BMP檔案!\n");
        system("PAUSE"); 
    
        return -1;  
    } else {
        printf("\n檔案驗證完成!\n");
    }

    printf("TEST:%ld", sizeof(unsigned long));
    //列印檔案前兩字 
    printf("\n\n檔案開頭前兩個字元為：%c%c\n", bmpHeader.id[0], bmpHeader.id[1]);
    
    printf("檔案大小為：%ld byte(s)\n", bmpHeader.fileSize); 
//    printf("保留區域內容為：%ld\n", bmpHeader.reserved);
//    printf("繪圖資料起始位址 %ld\n", bmpHeader.offset);
    
    printf("\n\n資訊檔頭大小為 %ld byte(s)\n", bmpHeader.headerSize);
    printf("圖寬為 %ld(pixel)\n", bmpHeader.width);
    printf("圖高為 %ld(pixel)\n", bmpHeader.height);
    printf("Planes為 %d (未知用途)\n", bmpHeader.planes); 
    printf("像素位元數為 %d bit(s) \n", bmpHeader.bitsPerPixel);
   
    printf("壓縮模式為 %ld \n", bmpHeader.compression);
    printf("圖檔資料的大小為 %ld byte(s)\n", bmpHeader.imageSize); 
    printf("水平解析度為 %ld (pixel/m)\n", bmpHeader.xPixelPerMeter);
    printf("垂直解析度為 %ld (pixel/m)\n", bmpHeader.yPixelPerMeter);
    printf("點陣圖使用的調色盤顏色數為 %ld \n", bmpHeader.colorUsed);    
    printf("重要的顏色數為 %ld \n\n", bmpHeader.importantColor);
    
    if(bmp!=NULL) fclose(bmp);  //檢查若檔案並不是NULL則關閉檔案   

    system("pause");

    return 0;
}
