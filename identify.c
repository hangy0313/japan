#include "stdio.h"
#include "math.h"
#include "stdbool.h"
#include "time.h"

#include "bmpHeader.h"

#define trainBmpHeight 120
#define trainBmpWidth 120

#define testBmpHeight 120
#define testBmpWidth 600

#define trainNum 175

#define widthStartWrite 20
#define writeNumPerColumn 15

#define TRUE 1
#define FALSE 0

#define PI 3.141592653589793
static unsigned char trainData[trainBmpHeight][trainBmpWidth][3];
static unsigned char testData[testBmpHeight][testBmpWidth][3];
void main()
{
    struct bmpHeader bmpHeader;
    FILE *trainBmp, *testBmp, *fileList, *japan, *checkBmp;

    clock_t startTime, endTime;
    float executeTime = 0;

    unsigned char listFileCommand[120], trainFileName[23], testFileName[23], rmCommand[120];

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

    unsigned long dftHeightLocation[21] = {0, 0, 1, 2, 1, 0, 0, 1, 2, 3, 4, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 6};
    unsigned long dftWidthLocation[21] = {0, 1, 0, 0, 1, 2, 3, 2, 1, 0, 0, 1, 2, 3, 4, 5, 4, 3, 2, 1, 0, 0};
    unsigned long trainFileDFT[trainNum][21];
    double trainPr[trainNum][21], trainPi[trainNum][21];
    unsigned long testFileDFT[21];
    double testPr[21], testPi[21];
    unsigned long arrayNum;
    unsigned long theta1, theta2;

    unsigned long num, height, width, j, i, count;
    unsigned long color, black = 0;
    unsigned long vert[testBmpWidth], segWidthLocation[20], segNum;
    unsigned long widthIndex, widthEndWrite;

    unsigned long dis, minDis, guessNum;

    unsigned short bom, newLine = 0x000a, space = 32, printfSpaceNum;

    bool widthStart = TRUE;

    startTime = clock();
    
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

            for(arrayNum = 0;arrayNum < 21;arrayNum++){
                height = dftHeightLocation[arrayNum];
                width = dftWidthLocation[arrayNum];
                for(j = 0;j < trainBmpHeight;j++){
                    for(i = 0;i < trainBmpWidth;i++){
                        theta1 = 2 * PI * (double)(height*j) / (double)trainBmpHeight;
                        theta2 = 2 * PI * (double)(width*i) / (double)trainBmpWidth;
                        for(count = 0;count < 3;count++){
                            trainPr[num][arrayNum] += trainData[j][i][count] * (cos(theta1)*cos(theta2) - sin(theta1)*sin(theta2));
                            trainPi[num][arrayNum] -= trainData[j][i][count] * (cos(theta1)*sin(theta2) + cos(theta2)*sin(theta1));
                        }
                    }
                }
                        trainFileDFT[num][arrayNum] = sqrt(trainPr[num][arrayNum]*trainPr[num][arrayNum] + trainPi[num][arrayNum]*trainPi[num][arrayNum]);
            }

            fclose(trainBmp);
            }
        num++;
    }
    fclose(fileList);

    sprintf(listFileCommand, "ls ./test/phrase_* > testFileList.txt");
    system(listFileCommand);
    fileList = fopen("testFileList.txt", "rt");
    while(!feof(fileList)){
        fscanf(fileList, "%s", testFileName);
        if(!feof(fileList)){
        
            testBmp = fopen(testFileName, "rb");
            fread(&bmpHeader, sizeof(struct bmpHeader), 1, testBmp);
            fread(testData, sizeof(unsigned char), testBmpHeight*testBmpWidth*3, testBmp);
            
            for(width = 0;width < testBmpWidth;width++){
                vert[width] = 0;
                for(height = 0;height < testBmpHeight;height++){
                    color = 0;
                    for(num = 0;num < 3;num++){
                        color += testData[height][width][num];
                    }
                    if(color == black){
                        vert[width]++;
                    }
                }
            }
            
            for(num = 0;num < 20;num++){
                segWidthLocation[num] = 0;
            }

            num = 0;
            for(width = 0;width < testBmpWidth;width++){
                if(widthStart){
                    if(vert[width] == 0 && vert[width+1] != 0){
                        segWidthLocation[num] = width;
                        num++;
                        widthStart = FALSE;
                        continue;
                    }
                }
                if(!widthStart){
                    if(vert[width] != 0 && vert[width+1] == 0){
                        segWidthLocation[num] = width;
                        num++;
                        widthStart = TRUE;
                        continue;
                    }
                }
            }
           
            segNum = 0;
            for(num = 0;num < 20;num++){
                if(segWidthLocation[num] == 0){
                    segNum = num/2;
                    break;
                }
            }

            for(speechArrayCount = 0;speechArrayCount < 20;speechArrayCount++){
                speechArray[speechArrayCount] = 32;
            }
            speechArrayCount = 0;
            widthIndex = 0;
            for(num = 0;num < segNum;num++){
                for(height = 0;height < trainBmpHeight;height++){
                    i = segWidthLocation[widthIndex];
                    widthEndWrite = widthStartWrite + segWidthLocation[widthIndex+1] - segWidthLocation[widthIndex];
                    for(width = 0;width < trainBmpWidth;width++){
                        if(width >= widthStartWrite && width <= widthEndWrite){
                            trainData[height][width][0] = testData[height][i][0];
                            trainData[height][width][1] = testData[height][i][1];
                            trainData[height][width][2] = testData[height][i][2];
                            i++;
                            continue;
                        }
                        trainData[height][width][0] = 255;
                        trainData[height][width][1] = 255;
                        trainData[height][width][2] = 255;
                    }
                }
//                checkBmp = fopen("check.bmp", "wb");
//                bmpHeader.height = 120;
//                bmpHeader.width = 120;
//                bmpHeader.fileSize = 120*120+54;
//                bmpHeader.imageSize = 120*120;
//                fwrite(&bmpHeader, sizeof(struct bmpHeader), 1, checkBmp);
//                fwrite(trainData, sizeof(unsigned char), trainBmpHeight*trainBmpWidth*3, checkBmp);
//                fclose(checkBmp);

                for(arrayNum = 0;arrayNum < 21;arrayNum++){
                    height = dftHeightLocation[arrayNum];
                    width = dftWidthLocation[arrayNum];
                    testPr[arrayNum] = 0.0;
                    testPi[arrayNum] = 0.0;
                    for(j = 0;j < trainBmpHeight;j++){
                        for(i = 0;i < trainBmpWidth;i++){
                            theta1 = 2 * PI * (double)(height*j) / (double)trainBmpHeight;
                            theta2 = 2 * PI * (double)(width*i) / (double)trainBmpWidth;
                            for(count = 0;count < 3;count++){
                                testPr[arrayNum] += trainData[j][i][count] * (cos(theta1)*cos(theta2) - sin(theta1)*sin(theta2));
                                testPi[arrayNum] -= trainData[j][i][count] * (cos(theta1)*sin(theta2) + cos(theta2)*sin(theta1));
                            }
                        }
                    }
                    testFileDFT[arrayNum] = sqrt(testPr[arrayNum]*testPr[arrayNum] + testPi[arrayNum]*testPi[arrayNum]);
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
                }
                fwrite(&trainFileNum[guessNum], sizeof(unsigned short), 1, japan);
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
//            fwrite(&newLine, sizeof(unsigned short), 1, japan);
            fclose(testBmp);
        }
    }
    fclose(fileList);
    fclose(japan);

    endTime = clock();
    executeTime = (endTime-startTime)/CLOCKS_PER_SEC;
    printf("execute time: %f sec \n", executeTime);
}
