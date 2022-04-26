#ifndef FILES_C
#define FILES_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>

/* Structs */
struct file
{
    char *path;
    int fd;
};

/* Function Headers */
void send_message(char *topic, char *bytes, int length);
struct file listen_topic(char *topic);
void stop_listening_topic(struct file f);
void create_dir(char *dir);

/* Defines */
#define BROKER_DIR "MAC0352_mqtt_broker"
#define MAX_FILE_LENGTH 201
#define FILE_MODE 0777 // TODO: 0770

/* ========================================================= */
/*                    AUXILIAR FUNCTIONS                     */
/* ========================================================= */

char *encode_topic(char *topic)
{
    size_t length = strlen(topic);
    char *encoded = (char *)malloc(length + sizeof(char));
    for (int i = 0, j = 0; i < length; i++, j++)
    {
        if (topic[i] == '\0')
        {
            encoded[j] = '\0';
            break;
        }

        char topic_char = topic[i];
        char decoded_char;
        if (topic_char == ' ' || topic_char == '/' || topic_char == '\\')
            decoded_char = ' ';
        else
            decoded_char = topic_char;
        encoded[j] = decoded_char;
    }
    return encoded;
}

char *get_dir_path(char *topic)
{
    char *encoded_topic = encode_topic(topic);
    char *dir_name = (char *)malloc(MAX_FILE_LENGTH * sizeof(char));
    sprintf(dir_name, "%s/%s/", BROKER_DIR, encoded_topic);
    free(encoded_topic);
    return dir_name;
}

void create_dir(char *dir)
{
    for (int i = 0;; i++)
    {
        if (dir[i] == '\0')
            break;
        if (dir[i] == '/' && i != 0)
        {
            dir[i] = '\0';

            if (access(dir, F_OK) == -1)
            {
                int res = mkdir(dir, FILE_MODE);
                printf("Created folder: '%s' | Res: %d\n", dir, res); // TODO: REMOVE
            }

            dir[i] = '/';
        }
    }
}

/* ========================================================= */
/*                   FUNCTION DEFINITIONS                    */
/* ========================================================= */

void send_message(char *topic, char *bytes, int length)
{
    char *path_suffix = get_dir_path(topic);
    char *path_preffix = tempnam(NULL, NULL);
    path_preffix[last_index_of(path_preffix, '/')] = '\0';
    char *topic_dir_path = (char *)malloc(MAX_FILE_LENGTH * sizeof(char));
    sprintf(topic_dir_path, "%s/%s", path_preffix, path_suffix);
    free(path_preffix);
    free(path_suffix);

    printf("> Openning dir: '%s'\n", topic_dir_path);
    DIR *topic_dir = opendir(topic_dir_path);
    if (topic_dir == NULL)
    {
        perror("opendir :(\n");
        exit(9);
    }
    struct dirent *file;

    while ((file = readdir(topic_dir)) != NULL)
    {
        if (file->d_type != DT_DIR)
        {
            char *file_name = (char *)malloc((strlen(topic_dir_path) + strlen(file->d_name) + 2) * sizeof(char));
            sprintf(file_name, "%s%s", topic_dir_path, file->d_name);
            int fd = open(file_name, O_WRONLY);
            write(fd, bytes, length);
            printf("> Sent message message to file (%d bytes): '%s'\n", length, file_name);
            close(fd);
        }
    }
    closedir(topic_dir);
}

/* Returns file descriptor */
struct file listen_topic(char *topic)
{
    struct file f;
    char *dir_name = get_dir_path(topic);
    f.path = tempnam(NULL, dir_name);
    create_dir(f.path);
    free(dir_name);

    /* O modo é 0644 para que o processo possa escrever e os
     * outros processos ou usuários possam ler */
    if (mkfifo((const char *)f.path, FILE_MODE) == -1)
    {
        perror("mkfifo :(\n");
        exit(8);
    }

    printf("Openning file: '%s'\n", f.path); // TODO: REMOVE
    f.fd = open(f.path, O_RDONLY);
    printf("Finishing openning file: '%s'\n", f.path); // TODO: REMOVE
    return f;
}

/* Returns file descriptor */
void stop_listening_topic(struct file f)
{
    close(f.fd);
    if (remove(f.path) != 0)
        perror("remove :(\n");
    free(f.path);
}

#endif
