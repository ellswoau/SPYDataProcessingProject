#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pbPlots.h"
#include "supportLib.h"

int numberLinesInFile;

// returns allocated token for field on success, NULL otherwise
char* getfield (char *buf, size_t field)
{
    size_t len = strlen(buf);       // size of input string
    char *cpy = malloc (len + 1),   // allocate for copy
    *p,                         // pointer to use with strsep
    *tok = NULL;                // token for requested field

    if (!cpy)                       // validate allocation
        return NULL;

    memcpy (cpy, buf, len + 1);     // copy buf to cpy
    p = cpy;                        // pointer to cpy, preserves cpy address

    while (field-- && (tok = strsep (&p, ","))) {}  // get field field

    // copy tok to cpy and return cpy on success or NULL on failure
    return tok ? memmove (cpy, tok, strlen(tok) + 1) : NULL;
}

//function counts the number of lines in the .txt file for array initialization
int getLineCount() {
    FILE *fp;
    char ch;
    int linesCount=0;

    //open file in read mode
    fp=fopen("SPY241Project.txt","r");
    if(fp==NULL) {
        printf("File \"%s\" does not exist!!!\n","SPY241Project.txt");
        return -1;
    }

    //read character by character and check for new line
    while((ch=fgetc(fp))!=EOF) {
        if(ch=='\n')
            linesCount++;
    }

    //close the file
    fclose(fp);
    return linesCount;
}

//function to calculate the average for an array of integers
float calculateAverage(int numberLines, double array[]) {
    double tempTotal;
    for (int i=1; i <= numberLines; i++) {
        tempTotal += array[i];
    }
    tempTotal = tempTotal / numberLines;
    return tempTotal;
}
//function that returns array position for minumum value
int returnMinimumIndex(int numberLines, double array[]) {
    double currentNum;
    int j = 1;
    for (int i=1; i <= numberLines; i++) {
        if (array[j] > array[i]) {
            j = i;
        }
    }
    return j;
}
//function that returns array position for maximum value
int returnMaximumIndex(int numberLines, double array[]) {
    double currentNum;
    int j = 1;
    //currentNum = array[i];
    for (int i=1; i <= numberLines; i++) {
        if (array[j] < array[i]) {
            j = i;
        }
    }
    return j;
}

//main function
int main()
{
    FILE* stream = fopen("SPY241Project.txt", "r");

    //get number of lines in int variable to initialize arrays
    numberLinesInFile = getLineCount();
    //create char array of dates
    char dates[numberLinesInFile][8];
    //create float array of put call ratio
    double putCallRatio[numberLinesInFile];
    //create int array of put volume
    double putVolume[numberLinesInFile];
    //create int array of call volume
    double callVolume[numberLinesInFile];
    //create int array of total options volume
    int totalVolume[numberLinesInFile];
    //create int array for index numbers to use on X-axis of plot
    double indexArray[numberLinesInFile];
    for (int i=0; i <= numberLinesInFile; i++) {
        indexArray[i] = i;
    }

    //allocate memory for 128 char line
    char line[128];

    //set row count to 0 and loop through input stream
    int rowCount = 0;
    while (fgets(line, 128, stream))
    {
        char* tmp = strdup(line);
        //copy dates to array
        strcpy(dates[rowCount], getfield(tmp, 1));
        //skip column names (row 0) for this one to avoid copy to int/float issues
       if (rowCount > 0) {
           putCallRatio[rowCount] = atof(getfield(tmp, 2));
           putVolume[rowCount] = atoi(getfield(tmp, 3));
           callVolume[rowCount] = atoi(getfield(tmp, 4));
           totalVolume[rowCount] = atoi(getfield(tmp, 5));
       }
        rowCount++;
        free(tmp);
    }

    //calculate and display statistics from txt data
    printf("Statistics from %s to %s:\n", dates[1], dates[numberLinesInFile]);
    printf("\tAverage put/call ratio: %.2f\n", calculateAverage(numberLinesInFile, putCallRatio));
    printf("\tMinimum put/call ratio: %.2f on %.8s\n", putCallRatio[(returnMinimumIndex(numberLinesInFile, putCallRatio))], dates[(returnMinimumIndex(numberLinesInFile, putCallRatio))]);
    printf("\tMaximum put/call ratio: %.2f on %.8s\n\n", putCallRatio[(returnMaximumIndex(numberLinesInFile, putCallRatio))], dates[(returnMaximumIndex(numberLinesInFile, putCallRatio))]);
    printf("\tAverage put volume: %.2f\n", calculateAverage(numberLinesInFile, putVolume));
    printf("\tMinimum put volume: %.2f on %.8s\n", putVolume[(returnMinimumIndex(numberLinesInFile, putVolume))], dates[(returnMinimumIndex(numberLinesInFile, putVolume))]);
    printf("\tMaximum put volume: %.2f on %.8s\n\n", putVolume[(returnMaximumIndex(numberLinesInFile, putVolume))], dates[(returnMaximumIndex(numberLinesInFile, putVolume))]);
    printf("\tAverage call volume: %.2f\n", calculateAverage(numberLinesInFile, callVolume));
    printf("\tMinimum call volume: %.2f on %.8s\n", callVolume[(returnMinimumIndex(numberLinesInFile, callVolume))], dates[(returnMinimumIndex(numberLinesInFile, callVolume))]);
    printf("\tMaximum call volume: %.2f on %.8s\n\n", callVolume[(returnMaximumIndex(numberLinesInFile, callVolume))], dates[(returnMaximumIndex(numberLinesInFile, callVolume))]);
    printf("\tAverage total volume: %.2f\n", calculateAverage(numberLinesInFile, totalVolume));

    //create put volume graph
    ScatterPlotSeries *series = GetDefaultScatterPlotSeriesSettings();
    series->xs = indexArray;
    series->xsLength = numberLinesInFile;
    series->ys = putVolume;
    series->ysLength = numberLinesInFile;
    series->linearInterpolation = true;
    series->lineType = L"solid";
    series->lineTypeLength = wcslen(series->lineType);
    series->lineThickness = 2;
    series->color = GetGray(0.3);

    ScatterPlotSettings *settings = GetDefaultScatterPlotSettings();
    settings->width = 1920;
    settings->height = 1080;
    settings->autoBoundaries = true;
    settings->autoPadding = true;
    settings->title = L"SPY Put Volume Chart";
    settings->titleLength = wcslen(settings->title);
    settings->xLabel = L"Business Days Since 07/06/10";
    settings->xLabelLength = wcslen(settings->xLabel);
    settings->yLabel = L"Number of Put Options";
    settings->yLabelLength = wcslen(settings->yLabel);
    ScatterPlotSeries *s [] = {series};
    settings->scatterPlotSeries = s;
    settings->scatterPlotSeriesLength = 1;

    RGBABitmapImageReference *canvasReference = CreateRGBABitmapImageReference();
    DrawScatterPlotFromSettings(canvasReference, settings);

    //add line showing average slope
    double x1 = MapXCoordinateAutoSettings(0, canvasReference->image, indexArray,2350);
    double y1 = MapYCoordinateAutoSettings(putVolume[1], canvasReference->image, putVolume,2330);
    double x2 = MapXCoordinateAutoSettings(numberLinesInFile, canvasReference->image, indexArray,2330);
    double y2 = MapYCoordinateAutoSettings(putVolume[numberLinesInFile], canvasReference->image, putVolume, 2330);
    DrawLine(canvasReference->image, x1, y1, x2, y2, 4, CreateRGBColor(0,0,1));
    size_t length;
    double *pngdata = ConvertToPNG(&length, canvasReference->image);
    WriteToFile(pngdata, length, "graph-put_volume.png");
    DeleteImage(canvasReference->image);

    //create call volume graph
    ScatterPlotSeries *series1 = GetDefaultScatterPlotSeriesSettings();
    series1->xs = indexArray;
    series1->xsLength = numberLinesInFile;
    series1->ys = callVolume;
    series1->ysLength = numberLinesInFile;
    series1->linearInterpolation = true;
    series1->lineType = L"solid";
    series1->lineTypeLength = wcslen(series->lineType);
    series1->lineThickness = 2;
    series1->color = GetGray(0.3);

    ScatterPlotSettings *settings1 = GetDefaultScatterPlotSettings();
    settings1->width = 1920;
    settings1->height = 1080;
    settings1->autoBoundaries = true;
    settings1->autoPadding = true;
    settings1->title = L"SPY Call Volume Chart ";
    settings1->titleLength = wcslen(settings1->title);
    settings1->xLabel = L"Business Days Since 07/06/10";
    settings1->xLabelLength = wcslen(settings1->xLabel);
    settings1->yLabel = L"Number of Call Options";
    settings1->yLabelLength = wcslen(settings1->yLabel);
    ScatterPlotSeries *s1 [] = {series1};
    settings1->scatterPlotSeries = s1;
    settings1->scatterPlotSeriesLength = 1;

    RGBABitmapImageReference *canvasReference1 = CreateRGBABitmapImageReference();
    DrawScatterPlotFromSettings(canvasReference1, settings1);
    //RGBABitmapImageReference *canvasReference1 = CreateRGBABitmapImageReference();
    //DrawScatterPlot(canvasReference1, 1920, 1080, indexArray, 2330, callVolume, 2330);
    //add line showing average slope
    x1 = MapXCoordinateAutoSettings(0, canvasReference1->image, indexArray,2350);
    y1 = MapYCoordinateAutoSettings(callVolume[1], canvasReference1->image, callVolume,2330);
    x2 = MapXCoordinateAutoSettings(numberLinesInFile, canvasReference1->image, indexArray,2330);
    y2 = MapYCoordinateAutoSettings(callVolume[numberLinesInFile], canvasReference1->image, callVolume, 2330);
    DrawLine(canvasReference1->image, x1, y1, x2, y2, 4, CreateRGBColor(0,0,1));
    size_t length1;
    double *pngdata1 = ConvertToPNG(&length1, canvasReference1->image);
    WriteToFile(pngdata1, length1, "graph-call_volume.png");
    DeleteImage(canvasReference1->image);

}