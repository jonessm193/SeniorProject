//Link all methods/globals

#ifndef __SYSTEM_H
#define __SYSTEM_H

#define size_t int

/* MAIN.C */
extern unsigned char *memcpy(unsigned char *dest, const unsigned char *src, int count);
extern unsigned char *memset(unsigned char *dest, unsigned char val, int count);
extern unsigned short *memcpyw(unsigned short *dest, const unsigned short *src, int count);
extern unsigned short *memsetw(unsigned short *dest, unsigned short val, int count);
extern int strlen(const char *str);
extern unsigned char inportb(unsigned short _port);
extern void outportb(unsigned short _port, unsigned char _data);
extern void panic(unsigned char * message);

/*SCRN.C */
extern void cls();
extern void putch(unsigned char c);
extern void puts(unsigned char *str);
extern void settextcolor(unsigned char forecolor, unsigned char backcolor);
extern void init_video();

/*GDT.C*/
extern void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
extern void gdt_install();

/*IDT.C*/
extern void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);
extern void idt_install();

/*ISRS.C*/
/* This defines what the stack looks like after an ISR was running */
struct regs
{
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};
extern void isrs_install();

/*IRQ.C*/
extern void irq_install();
extern void irq_install_handler(int irq, void (*handler)(struct regs *r));
extern void irq_uninstall_handler(int irq);
extern unsigned int irq_wait(unsigned int num);
extern unsigned int irqCurr;

/*TIMER.C*/
extern void timer_install();
extern void timer_wait(int ticks);

/*KB.C*/
extern void keyboard_install();
extern unsigned int pullChars;

/*PAGING.C*/
extern void paging_install();

/*DETECTFLOPPY.C*/
extern void detectfloppy();

/*FDC.C*/
extern unsigned int f_read_track(unsigned cyl);
extern unsigned int f_write_track(unsigned cyl);

/*CONTROL.C*/
extern void control_key(char input);
extern void writefile(char finput);
extern void readfile();

#endif