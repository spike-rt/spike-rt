/*
 * SPDX-License-Identifier: MIT
 *
 * Stubs for Newlib.
 * For details, please see the official document
 *  - https://sourceware.org/newlib/libc.html#Stubs
 *
 * Copyright (c) 2022-2023 Embedded and Real-Time Systems Laboratory,
 *            Graduate School of Information Science, Nagoya Univ., JAPAN
 */

#include <errno.h>
#include <sys/stat.h>
#include <reent.h>

#undef errno
extern int errno;

#include <kernel.h>
#include <kernel_cfg.h>
#include <t_syslog.h>
#include <syssvc/serial.h>
#include <serial/serial.h>
#include <spike/hub/system.h>

#include <tlsf.h>

#include <stdio.h>

//#define STDIN_FILENO  0
//#define STDOUT_FILENO 1
//#define STDERR_FILENO 2
enum {
  SIO_USB_FILENO = 3,
  SIO_BLUETOOTH_FILENO,
  SIO_TEST_FILENO,
  SIO_UART_F_FILENO,
  SIO_UART_E_FILENO,
};

static inline int _get_fd_from_sio_portid(ID portid) {
  switch(portid) {
  case SIO_USB_PORTID:
    return SIO_USB_FILENO;
  case SIO_BLUETOOTH_PORTID:
    return SIO_BLUETOOTH_FILENO;
  case SIO_TEST_PORTID:
    return SIO_TEST_FILENO;
  case SIO_UART_E_PORTID:
    return SIO_UART_F_FILENO;
  case SIO_UART_F_PORTID:
    return SIO_UART_F_FILENO;
  }

  return 0;
}

static inline ID _get_sio_portid_from_fd(int fd) {
  switch(fd) {
  //case STDIN_FILENO:
  //  return SIO_STDIN_PORTID;
  //case STDOUT_FILENO:
  //  return SIO_STDOUT_PORTID;
  //case STDERR_FILENO:
  //  return SIO_STDERR_PORTID;
  case SIO_USB_FILENO:
    return SIO_USB_PORTID;
  case SIO_BLUETOOTH_FILENO:
    return SIO_BLUETOOTH_PORTID;
  case SIO_TEST_FILENO:
    return SIO_TEST_PORTID;
  case SIO_UART_E_FILENO:
    return SIO_UART_E_PORTID;
  case SIO_UART_F_FILENO:
    return SIO_UART_F_PORTID;
  }

  return 0;
}


void _exit(int status) {
  hub_system_shutdown();
  /* Never come back here */
  while(1);
}

int _close(int fd) {
  return -1;
}

int _fstat(int file, struct stat *st) {
  ID portid = _get_sio_portid_from_fd(file);
  if (portid > 0) {
    st->st_mode = S_IFCHR;
    return 0;
  }

  errno = EBADF;
  return -1;
}

int _getpid(void) {
  return 1;
}

int _kill(int pid, int sig) {
  errno = EINVAL;
  return -1;
}


int link(const char *old, const char *new) {
  errno = EMLINK;
  return -1;
}

int _lseek(int file, int ptr, int dir) {
  return 0;
}

int _isatty(int file) {
  ID portid = _get_sio_portid_from_fd(file);
  if (portid > 0) {
    return 1;
  }

  errno = EBADF;
  return -1;
}


FILE* serial_open_newlib_file(ID portid) {
  int fd = _get_fd_from_sio_portid(portid);
  if (fd <= 0) {
    //API_ERROR("Invalid port id %d.", port);
    return NULL;
  }

  FILE *fp = fdopen(fd, "a+");
  if (fp != NULL)
    setbuf(fp, NULL); /* IMPORTANT! */
    //API_ERROR("fdopen() failed, fd: %d.", fd);
  return fp;
}

int _open(const char *name, int flags, int mode) {
  return -1;
}

int _write(int fd, const void *buf, size_t cnt) {
  ID portid = _get_sio_portid_from_fd(fd);
  if (portid > 0) {
    // Serial Port
    ER_UINT erlen = serial_wri_dat(portid, (const char *)buf, cnt);
    if (erlen < 0) {
      return -1;
    }
    return erlen;
  } else {
    errno = EBADF;
    return -1;
  }
}

int _read(int fd, char *buf, int cnt) {
  ID portid = _get_sio_portid_from_fd(fd);
  if (portid > 0) {
    // Serial Port
    ER_UINT erlen = serial_rea_dat(portid, (char *)buf, cnt);
    if (erlen < 0) {
      return -1;
    }
    return erlen;
  } else {
    errno = EBADF;
    return -1;
  }
}


// TODO: make configurable
#define HEAP_AREA_SIZE 32*1024

tlsf_t newlib_tlsf;

static uint8_t heap_area[HEAP_AREA_SIZE] __attribute__((aligned(4096)));

void newlib_tlsf_init(void) {
  newlib_tlsf = tlsf_create_with_pool(heap_area, HEAP_AREA_SIZE);
  //TODO: hang if newlib_tlsf == NULL
  assert(newlib_tlsf != NULL);
}

void *_malloc_r(void *reent, size_t nbytes) {
  void *mem;

  wai_sem(APP_HEAP_SEM);
  mem = tlsf_malloc(newlib_tlsf, nbytes);
  sig_sem(APP_HEAP_SEM);
  return mem;
}

void *_realloc_r(void *reent, void *aptr, size_t nbytes){
  void *mem;

  wai_sem(APP_HEAP_SEM);
  mem = tlsf_realloc(newlib_tlsf, aptr, nbytes);
  sig_sem(APP_HEAP_SEM);
  return mem;
}

void _free_r(void *reent, void *aptr) {
  wai_sem(APP_HEAP_SEM);
  tlsf_free(newlib_tlsf, aptr);
  sig_sem(APP_HEAP_SEM);
}

void *_memalign_r(void *reent, size_t align, size_t nbytes) {
  void *mem;

  wai_sem(APP_HEAP_SEM);
  mem = tlsf_memalign(newlib_tlsf, align, nbytes);
  sig_sem(APP_HEAP_SEM);
  return mem;
}
