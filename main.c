
#include <system.h>

unsigned char *memcpy(unsigned char *dest, const unsigned char *src, int count){
    int i;
    for(i = 0 ; i < count ; i++)
        dest[i]=src[i];
    return dest;
}

unsigned short *memcpyw(unsigned short *dest, const unsigned short *src, int count){ 
    int i;  
    for (i = 0; i < count; i++)
        dest[i] = src[i];

    return dest;
}

unsigned char *memset(unsigned char *dest, unsigned char val, int count){
    int i;
    for(i = 0 ; i < count ; i++)
        dest[i]=val;
    return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, int count){
    int i;
    for(i = 0 ; i < count ; i++)
        dest[i]=val;
    return dest;
}

int strlen(const char *str){
    int i;
    for(i = 0; str[i] != '\0'; i++)
        ;
    return i;
}

unsigned char inportb (unsigned short _port){
    unsigned char rv;
    __asm__ __volatile__ ("inb %1, %0" : "=a" (rv) : "dN" (_port));
    return rv;
}

void outportb (unsigned short _port, unsigned char _data){
    __asm__ __volatile__ ("outb %1, %0" : : "dN" (_port), "a" (_data));
}


int main(){
    gdt_install();
    idt_install();
    isrs_install();
    irq_install();
    timer_install();
    keyboard_install();
    puts((unsigned char*) "I'm causing an interrupt to occur, but you can't see it because that stuff works now.");
    __asm__ __volatile__ ("sti"); 
    init_video();
    puts((unsigned char *)("Now the PIT will count for you!"));
    for (;;);
}