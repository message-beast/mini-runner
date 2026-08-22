#include <stdio.h>
#include "../base/structure.h"
#include "../base/config.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

static inline __attribute__((always_inline, hot)) char* defineTab(int x, int y) {
    if (x == y) {
        return "";
    } else {
        int s = y - x;
        char* tab = calloc(s + 1, sizeof(char));
        if (__builtin_expect(tab == NULL, 0)) {
            return NULL;
        }
        #pragma GCC ivdep
        #pragma GCC unroll 4
        for (register int i = 0; i < s; ++i) {
            tab[i] = ' ';
        }
        tab[s] = '\0';
        return tab;
    }
}



__attribute__((hot)) void showJobs(job*** jobs) {
    if (__builtin_expect(numberOfJobs == 0, 0)) {
        printf("\033[33mthere is no jobs!\n");
        return;
    }
    printf("NAME\t\t\tRUNNABE-FILE\t\t\t\t\t\t\t\t\t\t\t    SECONDS-INTERVAL\t\t  LAST-RUNNED-TIME\t   PID\t\t\n");
    for (register int i = 0; i < numberOfJobs; ++i) {
        job* currentJob = (*jobs)[i];
        struct tm* lastRunned = localtime((void*)(&(currentJob->lastTimeRunned)));
        if (__builtin_expect(lastRunned == NULL, 0)) {
            perror("failed to parse time!\n");
        }
        char lastTimeRunnedBuff[25];
        size_t len = snprintf(lastTimeRunnedBuff, 25, "%04i/%02i/%02i %02i:%02i:%02i", lastRunned->tm_year + 1900, lastRunned->tm_mon + 1, lastRunned->tm_mday, lastRunned->tm_hour, lastRunned->tm_min, lastRunned->tm_sec);
        char* fourthGap = defineTab(len, 25);
        char* firstGap = defineTab(strlen(currentJob->name), 24);
        char* secondGap = defineTab(strlen(currentJob->runnableFile), 100);
        char pidB[30];
        len = snprintf(pidB, 30, "%li", currentJob->secondsInterval);
        char* thirdGap = defineTab(len, 30);
        printf("\033[32m%s%s\033[33m%s%s\033[31m%li%s\033[32m%s%s\033[34m%i\033[0m\n", currentJob->name, firstGap, currentJob->runnableFile, secondGap, currentJob->secondsInterval, thirdGap, lastTimeRunnedBuff, fourthGap, currentJob->pid);
        free(firstGap);
        free(secondGap);
        free(thirdGap);
        free(fourthGap);
        firstGap = NULL;
        secondGap = NULL;
        thirdGap = NULL;
    }
    return;
}