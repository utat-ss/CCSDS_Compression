#ifndef MAIN_H
#define MAIN_H
// printing to binary
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 


void check_single_encode(void);
void check_single_decode(void);


void print_binary_32(uint32_t word){
    printf("word: hex = %X, \tbin = "\
        BYTE_TO_BINARY_PATTERN " " \
        BYTE_TO_BINARY_PATTERN " " \
        BYTE_TO_BINARY_PATTERN " " \
        BYTE_TO_BINARY_PATTERN \
        ", \thex = %x\n", word, \
        BYTE_TO_BINARY(word>>24), \
        BYTE_TO_BINARY(word>>16), \
        BYTE_TO_BINARY(word>>8), \
        BYTE_TO_BINARY(word), word);  
}


#endif // MAIN_H