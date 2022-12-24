# logger

## backstory
 Yong Da Li
 Saturday, July 16, 2022
 
 I need a quick and dirty logging thing
 - I spent a long-ass time trying to debug the decoder
    - working with binary files is so hard, cuz you can't just open it in a text editor to debug
 - I want able to quickly turn on/off debug messages if I ever come back to this for the future
 - https://stackoverflow.com/questions/6508461/logging-library-for-c

## usage
Turn it on by setting `LOG_TO_FILE` in logger.c

The logger writes a file called "compression.log" (hard coded) to wherever the top level makefile was called. You give it 2 arguments.
1. `const char* tag`, it can be whatever you want. But standard ones are like "INFO", "DEBUG", "WARNING", and "ERROR". "INFO" for general user stuff. "DEBUG" for deep internal messages for debugging use. "WARNING" to warn the user weird shit is happening. "ERROR" for fatal stuff

2. `const char* message, ...`. This is the `printf()` style message. Put in your message with whatever stuff like `%d` you need. Then the variables in the back

## internal
There's a global file pointer called `logger_fptr`. 
1. call `logger_init()` at the top of your `main()` function. This opens the file
2. make a log with `logger(...)`
3. close the global file pointer with `logger_finalize()`