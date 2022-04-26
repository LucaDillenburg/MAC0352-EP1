#ifndef UTILS_C
#define UTILS_C

#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "data.c"

/* Function Headers */
void print_bytes(char *bytes, int length);
char *decode_remaining_length(char *encoded_remaining_length, long int *remaining_length);
unsigned int construct_int(unsigned char low, unsigned char high);
unsigned char high_char(unsigned int n);
unsigned char low_char(unsigned int n);
void print_packet_header(struct packet_header header);
void print_binary(unsigned int number);
int last_index_of(char *str, char c);
char *copy_str(char *src, int n);

/* ========================================================= */
/*                   FUNCTION DEFINITIONS                    */
/* ========================================================= */

void print_bytes(char *bytes, int length)
{
    for (int i = 0; i < length; i++)
        printf("%d(%c) ", (int)bytes[i], bytes[i]);
    printf("\n");
}

char *decode_remaining_length(char *encoded_remaining_length, long int *remaining_length)
{
    char *rest_bytes = encoded_remaining_length;
    int multiplier = 1;
    *remaining_length = 0;
    char encoded_byte;
    do
    {
        encoded_byte = *rest_bytes;
        *remaining_length += (encoded_byte & 127) * multiplier;
        multiplier *= 128;
        rest_bytes++;
    } while ((encoded_byte & 128) != 0);
    // *remaining_ptr = *remaining_ptr + 1;
    return rest_bytes;
}

unsigned int construct_int(unsigned char low, unsigned char high)
{
    return low + (high << CHAR_BIT);
}

unsigned char high_char(unsigned int n)
{
    return n & 0xff;
}

unsigned char low_char(unsigned int n)
{
    return n >> CHAR_BIT;
}

void print_packet_header(struct packet_header header)
{
    printf("{ type: %d, flags: %d, id: %d }\n",
           (int)header.type, (int)header.flags, (int)header.id);
}

/**
 * Código adapdado do Stack Overflow feito por <roylewilliam>
 * (referência: https://stackoverflow.com/a/70930112/11317116).
 * Prints the binary representation of any unsigned integer.
 * **/
void print_binary(unsigned int number)
{
    if (number >> 1)
    {
        print_binary(number >> 1);
        putc((number & 1) ? '1' : '0', stdout);
    }
    else
    {
        putc((number & 1) ? '1' : '0', stdout);
    }
}

char *copy_str(char *src, int n)
{
    char *cpy = (char *)malloc((n + 1) * sizeof(char));
    memcpy((void *)cpy, (void *)src, n);
    cpy[n] = '\0';
    return cpy;
}

int last_index_of(char *str, char c)
{
    int length = strlen(str);
    int index_of = -1;
    for (int i = 0; i < length; i++)
        if (str[i] == c)
            index_of = i;
    return index_of;
}

#endif
