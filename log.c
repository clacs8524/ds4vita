#include <psp2kern/io/fcntl.h>

#include "log.h"

extern int ksceIoMkdir(const char *, int);

#if LOG_ENABLED
static unsigned int log_buf_ptr = 0;
static unsigned int log_dropped = 0;
static int log_dir_ready = 0;
static char log_buf[16 * 1024];
#endif

void log_reset(void) {
#if LOG_ENABLED
  SceUID fd = ksceIoOpen(LOG_FILE, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_TRUNC, 6);
  if (fd < 0) return;

  ksceIoClose(fd);

  memset(log_buf, 0, sizeof(log_buf));
  log_buf_ptr = 0;
  log_dropped = 0;
  log_dir_ready = 0;
#endif
}

/*
 * log_write() no toca la SD: solo copia al buffer de RAM. El volcado lo hace
 * log_flush(), que se llama desde el hilo de Bluetooth y nunca desde el
 * callback, porque escribir en la SD dentro del callback ya se vio romper la
 * cadena de lectura.
 */
void log_write(const char *buffer, size_t length) {
#if LOG_ENABLED
  if ((log_buf_ptr + length) >= sizeof(log_buf)) {
    log_dropped += length;
    return;
  }

  memcpy(log_buf + log_buf_ptr, buffer, length);
  log_buf_ptr = log_buf_ptr + length;
#endif
}

int log_pending(void) {
#if LOG_ENABLED
  return (log_buf_ptr != 0);
#else
  return 0;
#endif
}

unsigned int log_dropped_bytes(void) {
#if LOG_ENABLED
  return log_dropped;
#else
  return 0;
#endif
}

void log_flush(void) {
#if LOG_ENABLED
  SceUID fd;

  if (log_buf_ptr == 0) return;

  if (!log_dir_ready) {
    ksceIoMkdir(LOG_PATH, 6);
    log_dir_ready = 1;
  }

  fd = ksceIoOpen(LOG_FILE, SCE_O_WRONLY | SCE_O_CREAT | SCE_O_APPEND, 6);
  if (fd < 0) return;

  ksceIoWrite(fd, log_buf, log_buf_ptr);
  ksceIoClose(fd);

  log_buf_ptr = 0;
#endif
}