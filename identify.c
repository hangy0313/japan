#include "stdio.h"
#include "math.h"
#include "stdbool.h"
#include "time.h"

#include "bmpHeader.h"

#define trainBmpHeight 120
#define trainBmpWidth 120

#define testBmpHeight 120
#define testBmpWidth 600

#define quarterBmpHeight 60
#define quarterBmpWidth 60

#define trainNum 175

#define frameWidth 15
#define frameHeight 10

#define widthStartWrite 20
#define writeNumPerColumn 15

#define TRUE 1
#define FALSE 0

#define PI 3.141592653589793
static unsigned char trainData[trainBmpHeight][trainBmpWidth][3];
static unsigned char testData[testBmpHeight][testBmpWidth][3];
static unsigned char quarterTestData[4][quarterBmpHeight][quarterBmpWidth][3];

void DFT(unsigned long, unsigned long, unsigned long *, unsigned char *);
void setWidthLocation(unsigned long, unsigned long, unsigned long *, unsigned char *);
void setHeightLocation(unsigned long, unsigned long, unsigned long *, unsigned char *);
long countSegNumber(unsigned long *);
void setSegTestData(unsigned long, unsigned long,unsigned long, unsigned long *, unsigned char *, unsigned char *);
void setQuaterData(unsigned long, unsigned long, unsigned long *, unsigned long *, unsigned char *, unsigned char *);
void main()
{
    struct bmpHeader bmpHeader;
    FILE *trainBmp, *testBmp, *fileList, *japan, *checkBmp, *phraseFile, *checkDft;

    clock_t startTime, endTime;
    float executeTime = 0;

    unsigned char listFileCommand[120], trainFileName[23], testFileName[23], guessFile[23], phrase[50];

    unsigned short trainFileNum[trainNum];
    unsigned short speech1[trainNum] = {
        97, 97, 105, 105, 117,
        117, 101, 101, 111, 111,
        107, 103, 107, 103, 107,
        103, 107, 103, 107, 103,
        115, 122, 115, 106, 115,
        122, 115, 122, 115, 122,
        116, 100, 99, 106, 116,
        115, 122, 116, 100, 116,
        100, 110, 110, 110, 110,
        110, 104, 98, 112, 104,
        98, 112, 102, 98, 112,
        104, 98, 104, 104, 98,
        112, 109, 109, 109, 109,
        109, 121, 121, 121, 121,
        121, 121, 114, 114, 114,
        114, 114, 119, 119, 119,
        119, 119, 110, 32,
        97, 97, 105, 105, 117,
        117, 101, 101, 111, 111,
        107, 103, 107, 103, 107,
        103, 107, 103, 107, 103,
        115, 122, 115, 106, 115,
        122, 115, 122, 115, 122,
        116, 100, 99, 106, 116,
        115, 122, 116, 100, 116,
        100, 110, 110, 110, 110,
        110, 104, 98, 112, 104,
        98, 112, 102, 98, 112,
        104, 98, 104, 104, 98,
        112, 109, 109, 109, 109,
        109, 121, 121, 121, 121,
        121, 121, 114, 114, 114,
        114, 114, 119, 119, 119,
        119, 119, 110, 118, 107,
        107, 32, 32, 32, 32
    };
    unsigned short speech2[trainNum] = {
        32, 32, 32, 32, 32,
        32, 32, 32, 104, 104,
        97, 97, 105, 105, 117,
        117, 101, 101, 111, 111,
        97, 97, 104, 105, 117,
        117, 101, 101, 111, 111,
        97, 97, 104, 105, 117,
        117, 117, 101, 101, 111,
        111, 97, 105, 117, 101,
        111, 97, 97, 97, 105,
        105, 105, 117, 117, 117,
        101, 101, 101, 111, 111,
        111, 97, 105, 117, 101,
        111, 97, 97, 117, 117,
        111, 111, 97, 105, 117,
        101, 111, 97, 97, 105,
        101, 111, 32, 32,
        32, 32, 32, 32, 32,
        32, 32, 32, 104, 104,
        97, 97, 105, 105, 117,
        117, 101, 101, 111, 111,
        97, 97, 104, 105, 117,
        117, 101, 101, 111, 111,
        97, 97, 104, 105, 117,
        117, 117, 101, 101, 111,
        111, 97, 105, 117, 101,
        111, 97, 97, 97, 105,
        105, 105, 117, 117, 117,
        101, 101, 101, 111, 111,
        111, 97, 105, 117, 101,
        111, 97, 97, 117, 117,
        111, 111, 97, 105, 117,
        101, 111, 97, 97, 105,
        101, 111, 32, 117, 97,
        101, 32, 32, 32,32
    };

    unsigned short speechArray[20], speechArrayCount;

    unsigned long trainFileDFT[trainNum][21];
    unsigned long testFileDFT[21];

    unsigned long num, j, i, k;
    unsigned long segWidthLocation[20], segNum;
    unsigned long widthIndex, widthEndWrite;

    unsigned long dis, minDis, guessNum, phraseNum = 0;
    unsigned long disOrder[5], guessOrder[5], order;

    unsigned long segWordWidthLocation[20], segWordHeightLocation[20];
    unsigned long guessQuarterDFT[4][21];
    unsigned long testQuarterDFT[4][21];

    unsigned short bom, newLine = 0x000a, space = 32, printfSpaceNum;

    startTime = clock();
    
    checkDft = fopen("chechDft.txt", "wt");
    phraseFile = fopen("speech.txt", "rb");
    japan = fopen("japan.txt", "wb");
    bom = 0xff;
    fputc(bom, japan);
    bom = 0xfe;
    fputc(bom, japan);

    sprintf(listFileCommand, "ls ./train/word* > trainFileList.txt");
    system(listFileCommand);

    fileList = fopen("trainFileList.txt", "rt");

    num = 0;
    while(!feof(fileList)){
        fscanf(fileList, "%s", trainFileName);
        if(!feof(fileList)){
            trainFileNum[num] = 10000*(trainFileName[13]-'0') + 1000*(trainFileName[14]-'0') + 100*(trainFileName[15]-'0') + 10*(trainFileName[16]-'0') + trainFileName[17]-'0';

            trainBmp = fopen(trainFileName, "rb");
            fread(&bmpHeader, sizeof(struct bmpHeader), 1, trainBmp);
            fread(trainData, sizeof(unsigned char), trainBmpHeight*trainBmpWidth*3, trainBmp);
            
            DFT(trainBmpHeight, trainBmpWidth, &trainFileDFT[num][0], &trainData);
            fclose(trainBmp);
            }
        num++;
    }
    fclose(fileList);
    
    sprintf(listFileCommand, "ls ./test/phrase_* > testFileList.txt");
    system(listFileCommand);
    fileList = fopen("testFileList.txt", "rt");
    while(!feof(fileList) && !feof(phraseFile)){
        fscanf(fileList, "%s", testFileName);
        if(!feof(fileList) && !feof(phraseFile)){
            testBmp = fopen(testFileName, "rb");
            fread(&bmpHeader, sizeof(struct bmpHeader), 1, testBmp);
            fread(testData, sizeof(unsigned char), testBmpHeight*testBmpWidth*3, testBmp);
            printf("%s\n", testFileName);
            
            //將每個詞的每個字分開來判斷
            setWidthLocation(testBmpHeight, testBmpWidth, &segWidthLocation, &testData);
            
//            checkBmp = fopen("check.bmp", "wb");
//            bmpHeader.height = 120;
//            bmpHeader.width = 600;
//            bmpHeader.fileSize = 600*120+54;
//            bmpHeader.imageSize = 600*120;
//            fwrite(&bmpHeader, sizeof(struct bmpHeader), 1, checkBmp);
//            for(height = 0;height < testBmpHeight;height++){
//                for(width = 0;width < testBmpWidth;width++){
//                    for(num = 0;num < 20;num++){
//                        if(width == segWidthLocation[num]){
//                            testData[height][width][0] = 0;
//                            testData[height][width][1] = 0;
//                            testData[height][width][2] = 255;
//                        }
//                    }
//                }
//            }
//            fwrite(testData, sizeof(unsigned char), testBmpHeight*testBmpWidth*3, checkBmp);
//            fclose(checkBmp);
            segNum = countSegNumber(&segWidthLocation);          
            
            for(speechArrayCount = 0;speechArrayCount < 20;speechArrayCount++){
                speechArray[speechArrayCount] = 32;
            }
            speechArrayCount = 0;
            widthIndex = 0;
            for(num = 0;num < segNum;num++){
                setSegTestData(trainBmpHeight, trainBmpWidth, widthIndex, &segWidthLocation, &testData, &trainData);
                DFT(trainBmpHeight, trainBmpWidth, &testFileDFT, &trainData);
                
                //欲猜測的字切成四等份
                setWidthLocation(trainBmpHeight, trainBmpWidth, &segWordWidthLocation, &trainData);
                setHeightLocation(trainBmpHeight, trainBmpWidth, &segWordHeightLocation, &trainData);
                setQuaterData(trainBmpHeight, trainBmpWidth, &segWordHeightLocation, &segWordWidthLocation, &trainData, &quarterTestData);
                for(i = 0;i < 4;i++){
                    DFT(quarterBmpHeight, quarterBmpWidth, &testQuarterDFT[i][0], &quarterTestData[i][0][0][0]);
                }

                for(i = 0;i < 5;i++){
                    guessOrder[i] = -1;
                    disOrder[i] = 10000000;
                }
                guessNum = 0;
                minDis = 100000000;
                for(i = 0;i < trainNum;i++){
                    dis = 0;
                    for(j = 0;j < 21;j++){
                        dis += labs(testFileDFT[j] - trainFileDFT[i][j]);
                    }
                    if(dis < minDis){
                        guessNum = i;
                        minDis = dis;
                    }
                    
                    for(j = 0;j < 5;j++){
                        if(dis < disOrder[j]){
                            k = 4;
                            while(k > j){
                                disOrder[k] = disOrder[k-1];
                                guessOrder[k] = guessOrder[k-1];
                                k--;
                            }
                            disOrder[j] = dis;
                            guessOrder[j] = i;
                            break;
                       }
                    }
                }
                
                minDis = 100000000;
                for(order = 0;order < 3;order++){
                    sprintf(guessFile, "./train/word_%ld.bmp", trainFileNum[guessOrder[order]]);
                    trainBmp = fopen(guessFile, "rb");
                    fseek(trainBmp, 54, SEEK_SET);
                    fread(trainData, sizeof(unsigned char), trainBmpHeight*trainBmpWidth*3, trainBmp);
                    fclose(trainBmp);

                    setWidthLocation(trainBmpHeight, trainBmpWidth, &segWordWidthLocation, &trainData);
                    setHeightLocation(trainBmpHeight, trainBmpWidth, &segWordHeightLocation, &trainData);
                    setQuaterData(trainBmpHeight, trainBmpWidth, &segWordHeightLocation, &segWordWidthLocation, &trainData, &quarterTestData);
                    for(i = 0;i < 4;i++){
                        DFT(quarterBmpHeight, quarterBmpWidth, &guessQuarterDFT[i][0], &quarterTestData[i][0][0][0]);
                    }
                
                    dis = 0;
                    for(i = 0;i < 4;i++){
                        for(j = 0;j < 21;j++){
                            dis += labs(guessQuarterDFT[i][j] - testQuarterDFT[i][j]);
                        }
                    }
                    if(dis < minDis){
                        guessNum = guessOrder[order];
                        minDis = dis;
                    }
                }

                fwrite(&trainFileNum[guessNum], sizeof(unsigned short), 1, japan);

                fwrite(&trainFileNum[guessOrder[0]], sizeof(unsigned short), 1, japan);
                fwrite(&trainFileNum[guessOrder[1]], sizeof(unsigned short), 1, japan);
                fwrite(&trainFileNum[guessOrder[2]], sizeof(unsigned short), 1, japan);
                fwrite(&trainFileNum[guessOrder[3]], sizeof(unsigned short), 1, japan);
                fwrite(&trainFileNum[guessOrder[4]], sizeof(unsigned short), 1, japan);
                
                fwrite(&space, sizeof(unsigned short), 1, japan);
                
                speechArray[speechArrayCount] = speech1[guessNum];
                speechArray[speechArrayCount+1] = speech2[guessNum];
                
                speechArrayCount +=2;
                widthIndex += 2;
            }
            printfSpaceNum = writeNumPerColumn-(segNum*2);
            for(num = 0;num < printfSpaceNum;num++){
                fwrite(&space, sizeof(unsigned short), 1, japan);
            }
            for(speechArrayCount = 0;speechArrayCount < 20;speechArrayCount += 2){
                if(speechArray[speechArrayCount] == 32){
                    fwrite(&newLine, sizeof(unsigned short), 1, japan);
                    break;
                }
                fwrite(&speechArray[speechArrayCount], sizeof(unsigned short), 1, japan);
                fwrite(&speechArray[speechArrayCount+1], sizeof(unsigned short), 1, japan);

                fwrite(&space, sizeof(unsigned short), 1, japan);
            }
            fclose(testBmp);
        }
    }
    fclose(fileList);
    fclose(japan);

    endTime = clock();
    executeTime = (endTime-startTime)/CLOCKS_PER_SEC;
    printf("execute time: %f sec \n", executeTime);
}
void DFT(unsigned long bmpHeight, unsigned long bmpWidth, unsigned long *dft, unsigned char *data)
{
    unsigned long height, width, j, i, arrayNum, count;
    unsigned long theta1, theta2;
    unsigned long dftHeightLocation[21] = {0, 0, 1, 2, 1, 0, 0, 1, 2, 3, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5};
    unsigned long dftWidthLocation[21] = {0, 1, 0, 0, 1, 2, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0};
    
    double Pr = 0.0, Pi = 0.0;
    double cos1cos2 = 0.0, sin1sin2 = 0.0, cos1sin2 = 0.0, cos2sin1 = 0.0;

    for(arrayNum = 0;arrayNum < 21;arrayNum++){
        height = dftHeightLocation[arrayNum];
        width = dftWidthLocation[arrayNum];
        Pr = 0.0;
        Pi = 0.0;
        for(j = 0;j < bmpHeight;j++){
            for(i = 0;i < bmpWidth;i++){
                theta1 = 2 * PI * (double)(height*j) / (double)bmpHeight;
                theta2 = 2 * PI * (double)(width*i) / (double)bmpWidth;
                cos1cos2 = cos(theta1) * cos(theta2);
                sin1sin2 = sin(theta1) * sin(theta2);
                cos1sin2 = cos(theta1) * sin(theta2);
                cos2sin1 = cos(theta2) * sin(theta1);
                for(count = 0;count < 3;count++){
                    Pr += data[j*(bmpWidth*3) + i*3 +count] * (cos1cos2 - sin1sin2);
                    Pi -= data[j*(bmpWidth*3) + i*3 + count] * (cos1sin2 + cos2sin1);
                }
            }
        }
        dft[arrayNum] = sqrt(Pr*Pr + Pi*Pi);
    }
}
void setWidthLocation(unsigned long bmpHeight, unsigned long bmpWidth, unsigned long *widthLocation, unsigned char *data)
{
    unsigned long height, width, j, i, framePoint, vert[600], num, color, black = 0;
   
    bool widthStart = TRUE;

    framePoint = bmpWidth - frameWidth;
    for(width = 0;width <= framePoint;width++){
        j = width + frameWidth;
        vert[width] = 0;
        for(i = width;i < j;i++){
            for(height = 0;height < bmpHeight;height++){
                color = 0;
                for(num = 0;num < 3;num++){
                    color += data[height*(bmpWidth*3) + i*3 + num];
                }
                if(color == black){
                    vert[width]++;
                }
            }
        }
    }
            
    for(num = 0;num < 20;num++){
        widthLocation[num] = 0;
    }
    num = 0;
    for(width = 0;width < framePoint;width++){
        if(widthStart){
            if(vert[width] == 0 && vert[width+1] != 0){
                widthLocation[num] = width +frameWidth;
                num++;
                widthStart = FALSE;
                continue;
            }
        }
        if(!widthStart){
            if(vert[width] != 0 && vert[width+1] == 0){
                widthLocation[num] = width;
                num++;
                widthStart = TRUE;
                continue;
            }
        }
    }
}
void setHeightLocation(unsigned long bmpHeight, unsigned long bmpWidth, unsigned long *heightLocation, unsigned char *data)
{
    unsigned long height, width, j, i, framePoint, hori[600], num, color, black = 0;
   
    bool heightStart = TRUE;
    
    framePoint = bmpHeight - frameHeight;
    for(height = 0;height < framePoint;height++){
        i = height + frameHeight;
        hori[height] = 0;
        for(j = height;j < i;j++){
            for(width = 0;width < bmpWidth;width++){
                color = 0;
                for(num = 0;num < 3;num++){
                    color += data[j*bmpWidth*3 + width*3 + num];
                }
                if(color == black){
                    hori[height]++;
                }
            }
        }
    }
    for(num = 0;num < 20;num++){
        heightLocation[num] = 0;
    }
    num = 0;
    for(height = 0;height < framePoint;height++){
        if(heightStart){
            if(hori[height] == 0 && hori[height+1] != 0){
                heightLocation[num] = height + frameHeight;
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
}
long countSegNumber(unsigned long *segWidthLocation)
{
    unsigned long segNum = 0, num;

    for(num = 0;num < 20;num++){
        if(segWidthLocation[num] == 0){
            segNum = num/2;
            break;
        }
    }

    return segNum;
}
void setSegTestData(unsigned long bmpHeight, unsigned long bmpWidth,unsigned long index, unsigned long *widthLocation, unsigned char *data, unsigned char *segTestData)
{
    unsigned long height, width, i, widthEndWrite;
    
    for(height = 0;height < bmpHeight;height++){
        i = widthLocation[index];
        widthEndWrite = widthStartWrite + widthLocation[index+1] - widthLocation[index];
        for(width = 0;width < bmpWidth;width++){
            if(width >= widthStartWrite && width <= widthEndWrite){
                segTestData[height*bmpWidth*3 + width*3 + 0] = data[height*testBmpWidth*3 + i*3 + 0];
                segTestData[height*bmpWidth*3 + width*3 + 1] = data[height*testBmpWidth*3 + i*3 + 1];
                segTestData[height*bmpWidth*3 + width*3 + 2] = data[height*testBmpWidth*3 + i*3 + 2];
                i++;
                continue;
            }
            segTestData[height*bmpWidth*3 + width*3 + 0] = 255;
            segTestData[height*bmpWidth*3 + width*3 + 1] = 255;
            segTestData[height*bmpWidth*3 + width*3 + 2] = 255;
        }
    }
}
void setQuaterData(unsigned long bmpHeight, unsigned long bmpWidth, unsigned long *heightLocation, unsigned long *widthLocation, unsigned char *data, unsigned char *quarterData)
{
    unsigned long height, height1, height2, height3, height4, halfHeight;
    unsigned long width, width1, width2, width3, width4, halfWidth;
    unsigned long num;
    
    for(num = 0;num < 4;num++){
        for(height = 0;height < quarterBmpHeight;height++){
            for(width = 0;width < quarterBmpWidth;width++){
                quarterData[num*quarterBmpHeight*quarterBmpWidth*3 + height*quarterBmpWidth*3 + width*3] = 255;
                quarterData[num*quarterBmpHeight*quarterBmpWidth*3 + height*quarterBmpWidth*3 + width*3 + 1] = 255;
                quarterData[num*quarterBmpHeight*quarterBmpWidth*3 + height*quarterBmpWidth*3 + width*3 + 2] = 255;
            }
        }
    }
    
    halfHeight = (heightLocation[0]+heightLocation[1]) / 2;
    halfWidth = (widthLocation[0]+widthLocation[1]) / 2;

    height1 = 0;
    height2 = 0;
    height3 = 0;
    height4 = 0;
    for(height = 0;height < bmpHeight;height++){
        width1 = 0;
        width2 = 0;
        width3 = 0;
        width4 = 0;
        for(width = 0;width < bmpWidth;width++){
            if(height >= heightLocation[0] && height <= halfHeight && width >= widthLocation[0] && width <= halfWidth){
                for(num = 0;num < 3;num++){
                    quarterData[height1*quarterBmpWidth*3 + width1*3 + num] = data[height*bmpWidth*3 + width*3 + num];
                }
                width1++;
            }
            if(height >= heightLocation[0] && height <= halfHeight && width >= halfWidth && width <= widthLocation[1]){
                for(num = 0;num < 3;num++){
                    quarterData[1*quarterBmpHeight*quarterBmpWidth*3 + height2*quarterBmpWidth*3 + width2*3 + num] = data[height*bmpWidth*3 + width*3 + num];
                }
                width2++;
            }
            if(height >= halfHeight && height <= heightLocation[1] && width >= widthLocation[0] && width <= halfWidth){
                for(num = 0;num < 3;num++){
                    quarterData[2*quarterBmpHeight*quarterBmpWidth*3 + height3*quarterBmpWidth*3 + width3*3 + num] = data[height*bmpWidth*3 + width*3 + num];
                }
                width3++;
            }
            if(height >= halfHeight && height <= heightLocation[1] && width >= halfWidth && width <= widthLocation[1]){
                for(num = 0;num < 3;num++){
                    quarterData[3*quarterBmpHeight*quarterBmpWidth*3 + height4*quarterBmpWidth*3 + width4*3 + num] = data[height*bmpWidth*3 + width*3 + num];
                }
                width4++;
            }
        }
        if(height >= heightLocation[0] && height <= halfHeight){
            height1++;
            height2++;
        }
        if(height >= halfHeight && height <= heightLocation[1]){
            height3++;
            height4++;
        }
    }
}
