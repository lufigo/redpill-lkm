#ifndef REDPILLLKM_CALL_PROTECTED_H
#define REDPILLLKM_CALL_PROTECTED_H

#include <linux/version.h> //LINUX_VERSION_CODE, KERNEL_VERSION
#include <linux/types.h> //bool

bool kernel_has_symbol(const char *name);

// ************************** Exports of normally protected functions ************************** //

//A usual macros to make defining them easier & consistent with .c implementation
#define CP_LIST(...) __VA_ARGS__ //used to pass a list of arguments as a single argument
#define CP_DECLARE_SHIM(return_type, org_function_name, call_args) return_type _##org_function_name(call_args);

#include <linux/seq_file.h>
CP_DECLARE_SHIM(int, cmdline_proc_show, CP_LIST(struct seq_file *m, void *v));

//See https://github.com/torvalds/linux/commit/6bbb614ec478961c7443086bdf7fd6784479c14a
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,1,0)
CP_DECLARE_SHIM(int, set_memory_ro, CP_LIST(unsigned long addr, int numpages));
CP_DECLARE_SHIM(int, set_memory_rw, CP_LIST(unsigned long addr, int numpages));
#else
#define _set_memory_ro(...) set_memory_ro(__VA_ARGS__)
#define _set_memory_rw(...) set_memory_rw(__VA_ARGS__)
#endif

/* Thanks Jeff... https://groups.google.com/g/kernel-meetup-bangalore/c/rvQccTl_3kc/m/BJCnnXGCAgAJ
 * In case the link disappears: Jeff Layton from RedHat decided to just nuke the getname() API after 7 years of it being
 *  exposed in the kernel. So in practice we need to use kallsyms to get it on kernels >=3.14 (up to current 5.14)
 * See https://github.com/torvalds/linux/commit/9115eac2c788c17b57c9256cb322fa7371972ddf
 * Another unrelated change which happened in v3.14 was that when "struct filename*" is passed the callee is responsible
 *  for freeing it (using putname()). However, in older versions we (the caller) needs to free it
 * See https://github.com/torvalds/linux/commit/c4ad8f98bef77c7356aa6a9ad9188a6acc6b849d
 */
struct filename;
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,14,0)
CP_DECLARE_SHIM(int, do_execve, CP_LIST(const char *filename,
        const char __user *const __user *__argv,
        const char __user *const __user *__envp));

#include <linux/fs.h>
#define _getname(...) getname(__VA_ARGS__)

//If syscall audit is disabled putname is an alias to final_putname(), see include/linux/fs.h; later on this was changed
// but this branch needs to handle only <3.14 as we don't need (and shouldn't use!) putname() in >=3.14
#ifndef CONFIG_AUDITSYSCALL //if CONFIG_AUDITSYSCALL disabled we unexport final_putname and add putname define line fs.h
#define _putname(name) _final_putname(name)
CP_DECLARE_SHIM(void, final_putname, CP_LIST(struct filename *name));
#else //if the CONFIG_AUDITSYSCALL is enabled we need to proxy to traced putname to make sure references are counted
CP_DECLARE_SHIM(void, putname, CP_LIST(struct filename *name));
#endif
#else
CP_DECLARE_SHIM(int, do_execve, CP_LIST(struct filename *filename,
        const char __user *const __user *__argv,
        const char __user *const __user *__envp));
CP_DECLARE_SHIM(struct filename *, getname, CP_LIST(const char __user *name));
#endif

typedef struct uart_port *uart_port_p;
CP_DECLARE_SHIM(int, early_serial_setup, CP_LIST(struct uart_port *port));
CP_DECLARE_SHIM(int, update_console_cmdline, CP_LIST(char *name, int idx, char *name_new, int idx_new, char *options));

#ifdef CONFIG_SYNO_BOOT_SATA_DOM
struct Scsi_Host;
CP_DECLARE_SHIM(int, scsi_scan_host_selected, CP_LIST(struct Scsi_Host *shost, unsigned int channel, unsigned int id, u64 lun, int rescan));
#endif

#include <linux/notifier.h>
void _usb_register_notify(struct notifier_block *nb);
void _usb_unregister_notify(struct notifier_block *nb);
#endif //REDPILLLKM_CALL_PROTECTED_H
