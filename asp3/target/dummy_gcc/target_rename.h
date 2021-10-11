/* This file is generated from target_rename.def by genrename. */

#ifndef TOPPERS_TARGET_RENAME_H
#define TOPPERS_TARGET_RENAME_H

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

/*
 *  tTraceLog.c
 */
#define log_dsp_enter				_kernel_log_dsp_enter
#define log_dsp_leave				_kernel_log_dsp_leave
#define log_inh_enter				_kernel_log_inh_enter
#define log_inh_leave				_kernel_log_inh_leave
#define log_exc_enter				_kernel_log_exc_enter
#define log_exc_leave				_kernel_log_exc_leave


#endif /* TOPPERS_TARGET_RENAME_H */
