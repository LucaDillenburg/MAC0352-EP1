#ifndef UTILS_C
#define UTILS_C

#include <limits.h>
#include <stdio.h>
#include "data.c"

/* Function Headers */
void print_bytes(char *bytes, int length);
long int decode_remaining_length(char *encoded_remaining_length, char **remaining_ptr);
unsigned int construct_int(unsigned char low, unsigned char high);
void printf_binary(unsigned int number);

/* ========================================================= */
/*                   FUNCTION DEFINITIONS                    */
/* ========================================================= */

void print_bytes(char *bytes, int length)
{
    for (int i = 0; i < length; i++)
        printf("%d(%c) ", (int)bytes[i], bytes[i]);
    printf("\n");
}

long int decode_remaining_length(char *encoded_remaining_length, char **remaining_ptr)
{
    int multiplier = 1;
    long int remaining_length = 0;
    char encoded_byte;
    *remaining_ptr = encoded_remaining_length;
    do
    {
        encoded_byte = **remaining_ptr;
        remaining_length += (encoded_byte & 127) * multiplier;
        multiplier *= 128;
        *remaining_ptr = *remaining_ptr + 1;
    } while ((encoded_byte & 128) != 0);
    *remaining_ptr = *remaining_ptr + 1;
    return remaining_length;
}

unsigned int construct_int(unsigned char low, unsigned char high)
{
    return low + (high << CHAR_BIT);
}

void print_packet(struct mttq_packet packet)
{
    printf("{ type: %d, flags: %d, id: %d, payload_length: %ld }\n",
           (int)packet.type, (int)packet.flags, (int)packet.id, packet.payload_length);
    printf("  |> payload: ");
    print_bytes(packet.payload, packet.payload_length);
}

// Código adapdado do Stack Overflow feito por <roylewilliam>
// (referência: https://stackoverflow.com/a/70930112/11317116).
// Prints the binary representation of any unsigned integer
// When running, pass 1 to first_call
void printf_binary(unsigned int number)
{
    if (number >> 1)
    {
        printf_binary(number >> 1);
        putc((number & 1) ? '1' : '0', stdout);
    }
    else
    {
        putc((number & 1) ? '1' : '0', stdout);
    }
}

size_t copy_str(char *dst, const char *src, size_t dstsize)
{
    size_t len = strlen(src);
    if (dstsize)
    {
        size_t bl = (len < dstsize - 1 ? len : dstsize - 1);
        ((char *)memcpy(dst, src, bl))[bl] = 0;
    }
    return len;
}

#endif
