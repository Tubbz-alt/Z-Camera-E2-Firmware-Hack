#include <pthread.h>

#include "log.h"

const char *log_path = "/tmp/hack_log.log";

// ###
pthread_mutex_t __logger_mutex;
//
void log_init()
{
    FILE* fp = fopen(log_path, "a");
    fprintf(fp, "Storyboard Creativity hack started!\n");

    if (pthread_mutex_init(&__logger_mutex, NULL) != 0)
    {
        fprintf(fp, "Could not init mutex for logger!\n");
        return;
    }

    fclose(fp);
}

void log_print(const char *format, ...)
{
    va_list arglist;

    pthread_mutex_lock(&__logger_mutex);
    FILE *fp = fopen(log_path, "a");

    va_start(arglist, format);
    vfprintf(fp, format, arglist);
    va_end(arglist);

    va_start(arglist, format);
    vfprintf(stdout, format, arglist);
    va_end(arglist);

    fclose(fp);
    pthread_mutex_unlock(&__logger_mutex);
}

void log_memory(uint8_t *ptr, uint32_t size)
{
    pthread_mutex_lock(&__logger_mutex);
    FILE *fp = fopen(log_path, "a");

    for (uint32_t i = 0; i < size; ++i)
    {
        fprintf(fp, "%02X ", (uint32_t)ptr[i]);
        fprintf(stdout, "%02X ", (uint32_t)ptr[i]);
    }
    fprintf(fp, "\n");
    fprintf(stdout, "\n");

    fclose(fp);
    pthread_mutex_unlock(&__logger_mutex);
}