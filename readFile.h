#ifndef __readFile_h__
#define __readFile_h__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *readFile(char *fileName) { // read file to char*
    FILE *file = fopen(fileName, "r");
    char *content;
    size_t n = 0;
    int c;

    if (file == NULL) {
        perror("Could not read file!");
        exit(EXIT_FAILURE);
    }
        
    fseek(file, 0, SEEK_END);
    long f_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    content = malloc(f_size);

    while ((c = fgetc(file)) != EOF) {
        content[n++] = (char)c;
    }

    content[n] = '\0';        

    return content;
}

double find_parameter(char*config, char* parameter) { // find parameter and return value enclosed by "
    char* parameter_start = strstr(config, parameter);
    char* start = strchr(parameter_start, '"') + 1;
    char* end = strchr(start, '"');
    *end = '\0';
    return(atof(start));
}

#endif