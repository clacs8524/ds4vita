#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <string.h>

/*
 * 1 = habilita la escritura de ds4vita_log.txt en ur0:tai/
 * 0 = deshabilitado (defecto): no se crea ni se escribe el archivo.
 * LA variable BT_THREAD_SLEEP_MS (en main.c) controla el tiempo de escritura del log 
 */
#define LOG_ENABLED 0

#define LOG_PATH "ur0:tai/"
#define LOG_FILE LOG_PATH "ds4vita_log.txt"

void log_reset(void);
void log_write(const char *buffer, size_t length);
void log_flush(void);
int log_pending(void);
unsigned int log_dropped_bytes(void);

/*
 * 512 y no 256: [SUM] lleva 19 campos y su peor caso teorico ronda los 348 B.
 * Con 256 el snprintf truncan en silencio, y lo que se perdia era justo el
 * final de la linea (stalls, rearms, rep_post_rearm), que es lo que hace falta
 * para el experimento. El buffer de agrupacion de log.c (16 KB) no cambia.
 */
#if LOG_ENABLED
#define LOG(...) \
  do { \
    char buffer[512]; \
    snprintf(buffer, sizeof(buffer), ##__VA_ARGS__); \
    log_write(buffer, strlen(buffer)); \
  } while (0)
#else
#define LOG(...) (void)0
#endif

#define TEST_CALL(f, ...) ({ \
    int ret = f(__VA_ARGS__); \
    LOG(# f " returned 0x%08X\n", ret); \
    ret; \
  })

#endif
