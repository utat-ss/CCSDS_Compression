/**
 * Yong Da Li
 * Saturday, July 16, 2022
 *
 * I need a quick and dirty logging thing
 * able to quickly turn on/off debug messages for the encoder
 * https://stackoverflow.com/questions/6508461/logging-library-for-c
 */

#ifndef LOGGER_H
#define LOGGER_H

// create log file wherever the top-level makefile is run
#define LOG_TO_FILE 		1
#define LOG_TO_TERMINAL 	1

void logger_init(const char* filename);
void logger_finalize();
void logger(const char* tag, const char* format, ...);

#endif /* LOG_H */