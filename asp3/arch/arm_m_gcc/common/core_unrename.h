/* This file is generated from core_rename.def by genrename. */

/* This file is included only when core_rename.h has been included. */
#ifdef TOPPERS_CORE_RENAME_H
#undef TOPPERS_CORE_RENAME_H

/*
 *  core_kernel_impl.c
 */
#undef exc_tbl
#undef vector_table
#undef lock_flag
#undef saved_iipm
#undef default_exc_handler
#undef default_int_handler
#undef config_int
#undef core_initialize
#undef core_terminate
#undef bitpat_cfgint
#undef set_exc_int_priority
#undef enable_exc
#undef disable_exc
#undef core_int_entry

/*
 *  core_support.S
 */
#undef core_exc_entry
#undef svc_handler
#undef pendsv_handler
#undef dispatch
#undef do_dispatch
#undef dispatcher_1
#undef start_r
#undef start_dispatch
#undef exit_and_dispatch
#undef call_exit_kernel

#undef iipm_enable_masks
#undef current_iipm_enable_mask
#undef iipm_enable_irq_tbl


#endif /* TOPPERS_CORE_RENAME_H */
