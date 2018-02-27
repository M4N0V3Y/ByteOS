#include "interrupts.h"
#include "drivers/apic.h"
#include "libk.h"
#include "types.h"

#define INT_PAGE_FAULT 14

static const char *const exception_messages[32] = {
	"Division by zero",
	"Debug",
	"Non-maskable interrupt",
	"Breakpoint",
	"Overflow",
	"Bound range exceeded",
	"Invalid opcode",
	"Device not available",
	"Double fault",
	"(reserved exception 9)",
	"Invalid TSS",
	"Segment not present",
	"Stack segment fault",
	"General protection fault",
	"Page fault",
	"(reserved exception 15)",
	"x87 floating-point exception",
	"Alignment check",
	"Machine check",
	"SIMD floating-point exception",
	"Virtualization exception",
	"(reserved exception 21)",
	"(reserved exception 22)",
	"(reserved exception 23)",
	"(reserved exception 24)",
	"(reserved exception 25)",
	"(reserved exception 26)",
	"(reserved exception 27)",
	"(reserved exception 28)",
	"(reserved exception 29)",
	"(reserved exception 30)",
	"(reserved exception 31)"
};

static void page_fault(uint8_t int_no, struct isr_context *regs)
{
	uintptr_t faulting_address;
	asm volatile("mov %%cr2, %0" : "=r" (faulting_address));
	panic(
		"%s:\n"
		"\tfaulting address: %p\n"
		"\trip: %p, rsp: %p\n"
		"\tint_no: %u, err_code: %lu, flags: %u\n",
		exception_messages[int_no],
		(void *)faulting_address,
		(void *)regs->rip, (void *)regs->rsp,
		int_no, (regs->info & 0xFFFFFFFF),
		(unsigned int)(regs->info & 0x1F)
	);
}

void exception_handler(struct isr_context *regs)
{
	uint8_t int_no = (uint8_t)(regs->info >> 32);
	switch (int_no) {
		case INT_PAGE_FAULT:
			page_fault(int_no, regs);
			break;
		default:
			panic(
				"%s:\n"
				"\trip: %p, rsp: %p\n"
				"\tint_no: %u, err_code: %lu\n",
				exception_messages[int_no],
				(void *)regs->rip, (void *)regs->rsp,
				int_no, (regs->info & 0xFFFFFFFF)
			);
	}
}
