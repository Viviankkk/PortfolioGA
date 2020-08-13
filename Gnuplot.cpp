//
// Created by Vivian Kang on 8/7/20.
//

#include "Gnuplot.h"
#include <unistd.h>
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;



void plotResults(vector<double>& yData, int dataSize,const string name) {
    FILE* gnuplotPipe, * tempDataFile;
    //const char* tempDataFileName1 = "tempData1";
    double x, y;

    gnuplotPipe = popen("/opt/local/bin/gnuplot","w");
    sleep(5);

    //gnuplotPipe = _popen("C:\\PROGRA~1\\gnuplot\\bin\\gnuplot.exe", "w");
    if (gnuplotPipe) {
        tempDataFile = fopen(name.c_str(), "w");
        string title="set title font \"Times New Roman ,18\"\n set title \"Stall Generations\" \n set ylabel \"Fitness\"\n set xlabel \"Generations\"\n";
        fprintf(gnuplotPipe, "%s", title.c_str());
        fprintf(gnuplotPipe, "plot \"%s\" with lines \n", name.c_str());
        fflush(gnuplotPipe);
        for (int i = 0; i < dataSize; i++) {
            x = i+1;
            y = yData[i];
            fprintf(tempDataFile, "%lf %lf\n", x, y);
        }
        fclose(tempDataFile);;
        printf("press enter to continue...\n");
        getchar();
        remove(name.c_str());
        fprintf(gnuplotPipe, "exit \n");
    }
    else {
        printf("gnuplot not found...");
    }
}
void plotCumulative(vector<double>& ptf,vector<double>& ref) {
    FILE* gnuplotPipe, * tempDataFile1,* tempDataFile2;
    //const char* tempDataFileName1 = "tempData1";
    double x1, y1, x2, y2;
    const string name1="Portfolio";
    const string name2="SPY";


    gnuplotPipe = popen("/opt/local/bin/gnuplot","w");
    sleep(2);

    //gnuplotPipe = _popen("C:\\PROGRA~1\\gnuplot\\bin\\gnuplot.exe", "w");
    if (gnuplotPipe) {

        string title="set title font \"Times New Roman ,18\"\n set title \"Cumulative Return\" \n set ylabel \"Cumulative Return\"\n set xlabel \"Days\"\n";
        fprintf(gnuplotPipe, "%s", title.c_str());
        fprintf(gnuplotPipe, "plot \"%s\" with lines, \"%s\" with lines\n", name1.c_str(),name2.c_str());
        fflush(gnuplotPipe);
        tempDataFile1 = fopen(name1.c_str(), "w");
        for (int i = 0; i < ptf.size(); i++) {
            x1 = i+1;
            y1 = ptf[i];
            fprintf(tempDataFile1, "%lf %lf\n", x1, y1);
        }
        fclose(tempDataFile1);

        tempDataFile2 = fopen(name2.c_str(), "w");
        for (int i = 0; i < ptf.size(); i++) {
            x2 = i+1;
            y2 = ref[i];
            fprintf(tempDataFile2, "%lf %lf\n", x2, y2);
        }
        fclose(tempDataFile2);;
        printf("press enter to continue...\n");
        getchar();
        sleep(2);
        remove(name1.c_str());remove(name2.c_str());
        fprintf(gnuplotPipe, "exit \n");
    }
    else {
        printf("gnuplot not found...");
    }
}
