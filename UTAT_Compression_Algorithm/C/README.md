# Bitstreamer
### This library aims to allow the user to write and write values of any size (1-64 bits) to a binary file, while ensuring the most effecient use of space. Instead of each variable taking at least 1 byte of disk space, the library will concatenate the binary form of each provided value. 
## Example:
``` c
a = 0b101 //6
b = 0b11  //3
c = 0b100 //4

// A) On disk represtation without bitstreamer:
00000101 00000011 00000100 //0 Padding to make each variable fill 1 byte minimum
// Eff_A = num_of_needed_bits / num_of_used_bits = 8/24 = 0.333...

// B) On disk represtation with bitstreamer:
10111100 //101,11,100
// Eff_B = num_of_needed_bits / num_of_used_bits = 8/8 = 1
```


# Functions
```c 
uint64_t* init_bitstream(void);
void init_bitstreamer_variables(void);
void write_to_disk(uint64_t* bitstream);
void read_from_disk(uint64_t* bitstream);
void write_to_bitstream(uint64_t value, uint64_t* bitstream, uint8_t width);
uint64_t read_from_bitstream(uint64_t* bitstream, uint8_t width);
void pad_bitstream(uint64_t* bitstream);
void depad_bitstream(uint64_t* bitstream);
uint16_t get_size(void);
void reset_bin_file(void);
```

# How to Use
## Definitions
_Here, I define some terms that I will be using in the later sections, in order improve understanding._

data_structure: The set of variables the user wishes to write to disk in one go (Ex. An array, a struct, etc...).

Bitstream: The in-memory variable housing all the provided data in an optimized manner. This variable, held in the form of a variable-sized array, is written to disk and emptied as soon as it's full.

## Parameters
```BUFFER_SIZE```: Size of in-memory data buffer. This is where the encoded variables are held before being written to disk once full. Define this value as wanted in ```bitstreamer.h```. Value should be number of bytes to be held in-memory. Minimum of 8 (Numbers more than 0 and less than 8 default to 8).

## Order of Operations
```reset_bin_file()```: Can be used if the user wishes to start a new ```.bin``` file. Should be used at the beginning of the super loop to initialize the environment.
```init_bitstreamer_variables()```: Must be used at the beginning of writing a user data. Sets necessary global varibles to initial value.
```encode_data(data_structure)```: User written function. Encodes all values in the data structure and writes them to disk. This function must consist of 4 steps:
1. Initialize bitstream:
```c
uint64_t* bitstream = init_bitstream();
```
2. Loop through each variable in data structure and pass to ```write_to_bitstream``` as ```uint64_t```. Also provide bitstream, and number of bits needed to to represent the variable.
```c
write_to_bitstream(value, bitstream, value_width_in_bits);
```
3. Add minimal 0-padding to file on disk. Once all variables have been iterated through and passed to ```write_to_bitstream```, call:
```c
pad_bitstream(bitstream)
```
4. Free the memory allocated to ```bitstream```:
```c
free(bitstream;
```

```decode_data(data_strucutre)```: User written function.  Decodes all values into the data structure after reading them from disk. This function must consist of ? steps:
1. Initialize bitstream:
```c
uint64_t* bitstream = init_bitstream();
```
2. Begin reading from disk and storing into ```bitstream``` by calling:
```c
read_from_disk(bitstream);
```
3. Remove padding added in the previous space by calling:
```c
depad_bitstream(bitstream);
```
4. Loop through each variable in data structure and call ```read_from_bitstream``` to populate the data structure with values. Provide bitstream, and the number of bits needed to to represent the variable.
```c
uint64_t value = read_from_bitstream(bitstream, width);
```


## Example
### Below is an example of ```encode_data(...)``` and ```decode_data(...)``` for the Image Metadata struct, defined in ```header.h```.
```c
void encode_imageMetadata(imageMetadata* params)
{
    uint64_t* bitstream = init_bitstream();

    uint16_t* cursor = (uint16_t*) params; //Used to iterate through data structure.

    for(int i = 0; i <= imageMetadataSize; ++i)
    {
        write_to_bitstream(*(cursor + i), bitstream, imageMetadataWidths[i]);
    }
    
    pad_bitstream(bitstream);

    free(bitstream);
    return;
}

void decode_imageMetadata(imageMetadata* params)
{
    uint64_t* bitstream = init_bitstream();

    uint16_t* cursor = (uint16_t*) params; //Used to iterate through data structure.

    read_from_disk(bitstream);

    depad_bitstream(bitstream);

    for(int i = imageMetadataSize - 1; i >= 0; --i)
    {
        uint64_t value = read_from_bitstream(bitstream, imageMetadataWidths[i]);
        *(cursor + i) = (uint16_t) value;
    }

    free(bitstream);
    return;
}

```
