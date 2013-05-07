//Coded mostly myself using gdt.c as a template, filling in differing attributes where needed

#include <system.h>

//Defines an IDT entry
struct idt_entry
{
    unsigned short base_lo;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr
{
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

// Declare an IDT of 256 entries
struct idt_entry idt[256];
struct idt_ptr idtp;

//This exists in 'start.asm', and is used to load our IDT
extern void idt_load();

//This function sets an entry in the IDT
void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags){
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;

    idt[num].sel = sel;
    idt[num].always0 = 0;

    idt[num].flags = flags;
}

//Installs the IDT
void idt_install(){
    //Sets the special IDT pointer up, just like in 'gdt.c'
    idtp.limit = (sizeof (struct idt_entry) * 256) - 1;
    idtp.base = (int)&idt;

    //Clear out the entire IDT, initializing it to zeros
    memset((unsigned char *)&idt, 0, sizeof(struct idt_entry) * 256);

    //Add any new ISRs to the IDT here using idt_set_gate

    //Points the processor's internal register to the new IDT
    idt_load();
}