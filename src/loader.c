/**
 * Modified version of loader.c from AN0042
 *
 * Boot from SRAM/PSRAM
 */
#include <string.h>
#include "em_device.h"
#ifdef PSRAM_BOOT
#include "em_ebi.h"
#include "bsp.h"
#endif

const
#include "bootld.h"

#define ISR_VTOR_SIZE	(0x0040 + 4 * 39)
#define SRAM_ADDR		0x20000000
#ifdef PSRAM_BOOT
#define BOOT_ADDR	0x16000000
#else
#define BOOT_ADDR	0x20000000
#endif

extern void jump(uint32_t sp, uint32_t pc);

void main(void) {
	__set_MSP((SRAM_ADDR + sizeof(bootloader) + 0x400) & 0xFFFFFFF0);

#ifdef PSRAM_BOOT
	BSP_Init(BSP_INIT_DK_EBI);
#endif

	/* Load the entire bootloader into SRAM. */
//	memset((void *) BOOT_ADDR, 0x0, 0x4000);
	memcpy((void *) BOOT_ADDR, bootloader, sizeof(bootloader));

#ifdef SRAM_VECTOR_REMAP
	/* Copy(Mirror) PSRAM Vector Table to SRAM */
	memcpy((void*) SRAM_ADDR, bootloader, ISR_VTOR_SIZE);
	SCB->VTOR = SRAM_ADDR;
#else
	SCB->VTOR = BOOT_ADDR;
#endif

	/* Start executing the bootloader. */
	jump(*(uint32_t*) BOOT_ADDR, *(uint32_t*) (BOOT_ADDR + 4));
}
