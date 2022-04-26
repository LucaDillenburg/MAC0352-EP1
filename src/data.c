#ifndef DATA_C
#define DATA_C

/* MQTT Packet Types */
#define CONNECT 1
#define CONNACK 2
#define PUBLISH 3
#define PUBACK 4
#define PUBREC 5
#define PUBREL 6
#define PUBCOMP 7
#define SUBSCRIBE 8
#define SUBACK 9
#define UNSUBSCRIBE 10
#define UNSUBACK 11
#define PINGREQ 12
#define PING 13
#define DISCONNECT 14

/* Structs */

struct packet_header
{
    unsigned char type;
    unsigned char flags;
    unsigned int id;
    char *last_ptr;
};

struct vector
{
    char *array;
    int length;
};

#endif
