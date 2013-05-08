//Mostly coded by SJones(fixed many methods and the main) but a lot taken from the tutorial

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

void panic(unsigned char * message){
    puts(message);
}

int main(){
    gdt_install();
    idt_install();
    isrs_install();
    irq_install();
    timer_install();
    keyboard_install();
    paging_install();
    __asm__ __volatile__ ("sti"); 
    init_video();
    /* Uncomment to test that paging is really on and working*/
    //puts((unsigned char*) ("test page fault: %d \n", *(( unsigned int *)0xA0000000 )));
    for (;;);
}