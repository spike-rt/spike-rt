/* This file is generated from target_rename.def by genrename. */

#ifndef TOPPERS_TARGET_RENAME_H
#define TOPPERS_TARGET_RENAME_H

/*
 *  target_kernel_impl.h
 */
#define lock_cpu					_kernel_lock_cpu
#define unlock_cpu					_kernel_unlock_cpu
#define sense_lock					_kernel_sense_lock

/*
 *  target_kernel_impl.c
 */
#define dispatch					_kernel_dispatch
#define start_dispatch				_kernel_start_dispatch
#define exit_and_dispatch			_kernel_exit_and_dispatch
#define int_handler_entry			_kernel_int_handler_entry
#define exc_handler_entry			_kernel_exc_handler_entry
#define call_exit_kernel			_kernel_call_exit_kernel
#define start_r						_kernel_start_r
#define target_initialize			_kernel_target_initialize
#define target_exit					_kernel_target_exit

/*
 *  target_timer.c
 */
#define target_hrt_initialize		_kernel_target_hrt_initialize
#define target_hrt_terminate		_kernel_target_hrt_terminate
#define target_hrt_handler			_kernel_target_hrt_handler
#define target_ovrtimer_initialize	_kernel_target_ovrtimer_initialize
#define target_ovrtimer_terminate	_kernel_target_ovrtimer_terminate
#define target_ovrtimer_handler		_kernel_target_ovrtimer_handler


#endif /* TOPPERS_TARGET_RENAME_H */
