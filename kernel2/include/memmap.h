#ifndef _MEMMAP_H_
#define _MEMMAP_H_

#include <sizes.h>
#include <memphys.h>

// Lower MB
#define IVT_ADDRESS      0x00000
#define IVT_SIZE         0x00200

#define BIOSCODE_ADDRESS 0xC0000
#define BIOSCODE_SIZE    0x40000

// Memory map

#define PHYS_MEMORY memphys_memory          ///< phys memory available
#define PHYS_PAGES  (PHYS_MEMORY/PAGE_SIZE) ///< phys pages available

#define BIOSDATA_ADDRESS   0                                                          ///< virt/phys address of BIOS data
#define BIOSDATA_SIZE      (1*MBYTES)                                                 ///< virt/phys address of BIOS data

#define KERNELCODE_ADDRESS (BIOSDATA_ADDRESS+BIOSDATA_SIZE)                           ///< virt/phys address of kernel code
#define KERNELCODE_SIZE    (2*MBYTES)                                                 ///< virt/phys size of kernel code
/// @todo We don't need 2MB for kernel code, but for loaded programs.

#define STACKDATA_ADDRESS  (KERNELCODE_ADDRESS+KERNELCODE_SIZE)                       ///< virt address of stack data
#define STACKDATA_SIZE     (4*MBYTES)                                                 ///< virt size of stack data

#define KERNELDATA_ADDRESS (STACKDATA_ADDRESS+STACKDATA_SIZE)                         ///< virt address of kernel data
#define KERNELDATA_SIZE    (1*GBYTES-KERNELDATA_ADDRESS)                              ///< virt size of kernel data

#define USERDATA_ADDRESS   (1*GBYTES)                                                 ///< virt address of user data
#define USERDATA_SIZE      (3068*MBYTES-4*KBYTES)                                     ///< virt size of user data

#define KERNELPD_ADDRESS   ((unsigned int)PAGEUP(KERNELCODE_ADDRESS+KERNELCODE_SIZE)) ///< phys address of Kernel PD (and PTs)
#define KERNELPD_SIZE      (3*PAGE_SIZE)                                              ///< phys size of Kernel PD (and PTs)

#define FREEPHYS_ADDRESS   (KERNELPD_ADDRESS+KERNELPD_SIZE)                           ///< phys address of free (phys) pages
#define FREEPHYS_SIZE      (PHYS_MEMORY-FREEPHYS_ADDRESS)                             ///< phys size of free (phys) pages

#define BUFPAGE_ADDRESS    (4092*MBYTES-4*KBYTES)                                     ///< Buffer page for writing/reading physical memory
#define PAGETABLES_ADDRESS (4092*MBYTES)                                              ///< loaded Pagetables
#define PAGEDIR_ADDRESS    (4*GBYTES-4*KBYTES)                                        ///< loaded Pagedir

#endif
