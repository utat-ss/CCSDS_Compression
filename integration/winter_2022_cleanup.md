# Getting CCSDS in C to work
Yong Da Li

## Friday, December 23, 2022
### decompression
- run the predictor to see what the predicted values are
- take the residuals and add it
- recover the real value
- reference App.java (line 176: decoder.decode) > Decoder.java (line 267: predictor.decompress) > Predictor.java (line 467)

### game plan
- going to make all changes on the `integration` branch then nuke it back to main
- I think I should have (based on structure of other C code):
    - encoder / decoder
    - predictor / unpredictor
    - helper / utils
    - main / compressor

### makefiles
- modernized the makefile for encoder to take a list of source files and automatically generate dependencies, to produce an executable
  
### VSCode
- hex editor in VSCode makes viewing binary files much easier
- run VSCode from WSL Ubuntu SSH connection because I configured the C/C++ intelisense to find `/usr/include/gsl/*` for the `gsl` header files