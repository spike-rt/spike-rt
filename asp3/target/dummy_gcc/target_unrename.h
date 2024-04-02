/* This file is generated from target_rename.def by genrename. */

/* This file is included only when target_rename.h has been included. */
#ifdef TOPPERS_TARGET_RENAME_H
#undef TOPPERS_TARGET_RENAME_H

/*
 *  target_kernel_impl.h
 */
#undef lock_cpu
#undef unlock_cpu
#undef sense_lock

/*
 *  target_kernel_impl.c
 */
#undef dispatch
#undef start_dispatch
#undef exit_and_dispatch
#undef int_handler_entry
#undef exc_handler_entry
#undef call_exit_kernel
#undef start_r
#undef target_initialize
#undef target_exit

/*
 *  target_timer.c
 */
#undef target_hrt_initialize
#undef target_hrt_terminate
#undef target_hrt_handler
#undef target_ovrtimer_initialize
#undef target_ovrtimer_terminate
#undef target_ovrtimer_handler


#endif /* TOPPERS_TARGET_RENAME_H */
