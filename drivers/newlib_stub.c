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
#include <spike/hub/system.h>
#include <tlsf.h>


void _exit(int status) {
  hub_system_shutdown();
  /* Never come back here */
}

int close(int file) {
  return -1;
}

char *__env[1] = { 0 };
char **environ = __env;

int execve(char *name, char **argv, char **env) {
  errno = ENOMEM;
  return -1;
}

int fork(void) {
  errno = EAGAIN;
  return -1;
}

int fstat(int file, struct stat *st) {
  st->st_mode = S_IFCHR;
  return 0;
}

int getpid(void) {
  return 1;
}

int isatty(int file) {
  return 1;
}

int kill(int pid, int sig) {
  errno = EINVAL;
  return -1;
}


int link(char *old, char *new) {
  errno = EMLINK;
  return -1;
}

int lseek(int file, int ptr, int dir) {
  return 0;
}

int open(const char *name, int flags, int mode) {
  return -1;
}

int read(int file, char *ptr, int len) {
  return 0;
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
