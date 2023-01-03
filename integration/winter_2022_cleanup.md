# Getting CCSDS in C to work
Yong Da Li


## Viewing hex files
- us VSCode hex editor
- or `hexdump -C <binary_file>`
    - `-C` = canonical, always get consistent byte-by-byte output regardless of your system's endianness

## writing to bitfile
Yong Da Li, Saturday, July 16, 2022
- note that the bitfile library BitFilePutBits() (plural) gives some weird padding issue
  - breaks assumptions about binary file structure
  - falling back to use the singular BitFilePutBit() and looping through --> avoids padding issue


## Saturday, December 29, 2022


## Saturday, December 24, 2022
### restructure
- changed to have a top-level make file


### predictor
- reread that paper to re-familiarize myself with the predictor
>Post-print of: "A tutorial on image compression for optical space imaging systems" / I. Blanes, E. Magli, J. Serra-Sagristà in IEEE Geoscience and remote sensing magazine (IEEE), Vol. 2, Issue 3 (Sept. 2014) , p. 8-26. DOI 10.1109/MGRS.2014.2352465

- this also helps: An Efficient Real-Time FPGA Implementation of the CCSDS-123 Compression Standard for Hyperspectral Images
> https://ieeexplore.ieee.org/document/8472142
  
### away from GSL
- since we're literally only doing vector-vector operations, I made my own matrix/vector library
- don't need to use GSL
- this is work that needs to be done at some point --> need to make it work on ARM math operations

### VSCode
- back to using VSCode on Windows side (not SSH into WLS2)
    - file index doesn't refresh automatically --> annoying because file list is not accurate

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