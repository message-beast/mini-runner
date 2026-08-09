#pragma optimize("03")
#pragma optimize("fast-math")
#define _POSIX_C_SOURCE 200809L
#include "../base/structure.h"
#include "../base/config.h"
#include <string.h>
#include <stdlib.h>
#include "../basic.h"
#define __INITIAL_SCALE_CAPACITY_OF_RESULTS__ 4
#define true 1
#define false 0


typedef struct result {
    service* service;
    __uint16_t score;
} result;


static inline __attribute__((always_inline, hot))int sort_results(const void * __restrict__ a, const void* __restrict__ b) {
    result* result1 = *(result **)a;
    result* result2 = *(result **)b;
    if (result1->score < result2->score) {
        return 1;
    }
    if (result1->score > result2->score) {
        return -1;
    }
    return 0;
}

static inline __attribute__((always_inline, hot)) char* defineTab(int x, int y) {
    if (x == y) {
        return "";
    } else {
        int s = y - x;
        char* tab = calloc(s + 1, sizeof(char));
        if (__builtin_expect(tab == NULL, 0)) {
            return "\0";
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


void listServices(service*** services) {
    if (__builtin_expect(numberOfProjects == 0, 0)) {
        printf("\033[33mThere is no services!\033[0m\n");
        return;
    }
    if (__builtin_expect(services == NULL ||*services == NULL, 0)) {
        fprintf(stderr, "error happend!\n");
        return;
    }   
    printf("NAME\t\t\t GITHUB-REPO\t\t\t\t\t\t\t\t\t\t\t     PID\n");
    #pragma GCC unroll 4
    for (register int i = 0; i < numberOfProjects; i++) {
        if (__builtin_expect((i & 15) == 0 || i == 0, 0)) {
            __builtin_prefetch(&(*services)[i + 16], 0, 3);
        }
        service* current = (*services)[i];
        char* firstGap = defineTab(strlen(current->name), 25);
        char* secondGap = defineTab(strlen(current->githubRepo), 100);
        printf("\033[32m%s%s\033[33m%s%s\033[31m%i\033[0m\n", current->name, firstGap, current->githubRepo, secondGap, current->pid);
        free(firstGap);
        free(secondGap);
        firstGap = NULL;
        secondGap = NULL;
    }
    return;
}



[[nodiscard]]static inline __attribute__((always_inline, hot)) __uint8_t findScoreByRepition(char x, char* __restrict__ wholeStr, __uint8_t len) {
    register __uint8_t score = 0;
    #pragma GCC ivdep
    #pragma GCC unroll 4
    for (register int i = 0; i < len; i++) {
        if (__builtin_expect((i & 255) == 0 || i == 0, 0)) {
            __builtin_prefetch(&wholeStr[i + 256], 0, 3);
        }
        if (x == wholeStr[i]) {
            score++;
        }
    }
    return score;
}


[[nodiscard]] static inline __attribute__((always_inline, hot)) __uint16_t findMatch(char* __restrict__  name, char* __restrict__ searchName) {
    if (__builtin_expect(name == NULL || searchName == NULL, 0)) {
        return 0;
    }
    register __uint16_t score = 0;
    int nameLength = strlen(name);
    int searchLength = strlen(searchName);
    int maxLength = (nameLength > searchLength) ? nameLength : searchLength;
    for (register int i = 0; i < maxLength; ++i) {
        int s = findScoreByRepition(searchName[i], name, nameLength);
        score += s;
    }
    int minLength = (nameLength > searchLength) ? searchLength : nameLength;
    #pragma GCC ivdep
    #pragma GCC unroll 4
    for (register int i = 0; i < minLength; i++) {
        if (__builtin_expect((i & 127) == 0 || i == 0, 0)) {
            __builtin_prefetch(&searchName[i + 128], 0, 3);
            __builtin_prefetch(&name[i + 128], 0, 3);
        }
        if (searchName[i] == name[i]) {
            score++;
        }
    }
    if (__builtin_expect(score == 0, 0)) {
        return 0;
    }
    return score;
}


__attribute__((hot)) void search(service*** __restrict__ services, char* __restrict__ serviceNameSearch, _Bool sort) {
    if (__builtin_expect(numberOfProjects == 0, 0)) {
        printf("\033[33myou don't have any service to search\033[0m\n");
        return;
    }
    if (__builtin_expect(services == NULL || *services == NULL, 0)) {
        return;
    }
    if (__builtin_expect(serviceNameSearch == NULL, 0)) {
        printf("searching service name doesn't provided!\n");
        return;
    }
    _Bool foundMatch = false;
    printf("\033[32m============== \033[34mSEARCH RESULTS FOR \033[33m\"%s\" \033[32m==============\033[0m\n", serviceNameSearch);
    printf("NAME\t\t\t GITHUB-REPO\t\t\t\t\t\t\t\t\t\t\t     PID\n");
    if (!sort) {
        #pragma GCC unroll 4
        for (register int i = 0; i < numberOfProjects; ++i) {
            if (__builtin_expect((i & 7) == 0 || i == 0, 0)) {
                __builtin_prefetch(&(*services)[i + 8], 0, 3);
            }
            if (__builtin_expect(findMatch((*services)[i]->name, serviceNameSearch) > 0, 1)) {
                if (__builtin_expect(!foundMatch, 0)) {
                    foundMatch = true;
                }
                service* current = (*services)[i];
                char* firstGap = defineTab(strlen(current->name), 25);
                char* secondGap = defineTab(strlen(current->githubRepo), 100);
                printf("\033[32m%s%s\033[33m%s%s\033[31m%i\033[0m\n", current->name, firstGap, current->githubRepo, secondGap, current->pid);
                free(firstGap);
                free(secondGap);
                firstGap = NULL;
                secondGap = NULL;
            }
        }
        if (__builtin_expect(!foundMatch, 0)) {
            printf("there is not service name found match for \"%s\"\n", serviceNameSearch);
            return;
        }
        return;
    } else {
        int numberOfResults = 0;
        int resultsCapacity = __INITIAL_SCALE_CAPACITY_OF_RESULTS__;
        DEBUG
        result** results = malloc( __INITIAL_SCALE_CAPACITY_OF_RESULTS__ * sizeof(result *));
        if (__builtin_expect(results == NULL, 0)) {
            perror("can not allocate memory for results\n");
            return;
        }
        #pragma omp parallel for num_threads(8)
        for (register int i = 0; i < numberOfProjects; ++i) {
            if (__builtin_expect((i & 7) == 0 || i == 0, 0)) {
                __builtin_prefetch(&(*services)[i + 8], 0, 3);
            }
            __uint16_t score = findMatch((*services)[i]->name, serviceNameSearch);
            if (score > 0) {
                if (__builtin_expect(foundMatch == false, 0)) {
                    foundMatch = true;
                }
                if (__builtin_expect(numberOfResults >= resultsCapacity, 0)) {
                    int newCapacity = resultsCapacity + __INITIAL_SCALE_CAPACITY_OF_RESULTS__;
                    result** tmp = realloc(results, sizeof(result*) * newCapacity);
                    if (__builtin_expect(tmp == NULL, 0)) {
                        perror("can not reallocate memory for results\n");
                        exit_program(-1)
                    }
                    results = tmp;
                    tmp = NULL;
                    resultsCapacity = newCapacity;
                }
                result* newResult = malloc(sizeof(result));
                newResult->service = (*services)[i],
                newResult->score = score;
                results[numberOfResults] = newResult;
                numberOfResults++;
            }
        }
        if (__builtin_expect(!foundMatch, 0)) {
            printf("there is not service name found match for \"%s\"\n", serviceNameSearch);
            free(results);
            results = NULL;
            return;
        }

        qsort(results, numberOfResults, sizeof(result*), sort_results);
        #pragma GCC ivdep
        #pragma GCC unroll 4
        for (register int i = 0; i < numberOfResults; ++i) {
            if (__builtin_expect((i & 31) == 0 || i == 0, 0)) {
                __builtin_prefetch(&(*services)[i + 32], 0, 3);
            }
            service* current = results[i]->service;
            char* firstGap = defineTab(strlen(current->name), 25);
            char* secondGap = defineTab(strlen(current->githubRepo), 100);
            printf("\033[32m%s%s\033[33m%s%s\033[31m%i\033[0m\n", current->name, firstGap, current->githubRepo, secondGap, current->pid);
            free(firstGap);
            free(secondGap);
            firstGap = NULL;
            secondGap = NULL;
        }
        #pragma GCC ivdep
        #pragma GCC unroll 4
        for (register int i = 0; i < numberOfResults; ++i) {
            if (__builtin_expect((i & 255) == 0 || i == 0, 0)) {
                __builtin_prefetch(&(*services)[i+256], 0, 3);
            }
            result* curRes = results[i];
            free(curRes);
            curRes = NULL;
        }
        free(results);
        results = NULL;
        return;
    }

    return;
}