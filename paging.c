//Coded by SJones with some help from a tutorial on osdever.net

#include <system.h>

unsigned long *page_directory = (unsigned long *) 0x9C000;
unsigned long *page_table = (unsigned long *) 0x9D000; // the page table comes right after the page directory
unsigned long address=0; // holds the physical address of where a page is
unsigned int i;
extern void write_cr0();
extern void write_cr3();
extern unsigned long read_cr0();

void paging_install(){
	// map the first 4MB of memory
	for(i=0; i<1024; i++){
		page_table[i] = address | 3; // attribute set to: supervisor level, read/write, present(011 in binary)
		address = address + 4096; // 4096 = 4kb
	}

	// fill the first entry of the page directory
	page_directory[0] = (int)page_table; // attribute set to: supervisor level, read/write, present(011 in binary)
	page_directory[0] = page_directory[0] | 3;

	for(i=1; i<1024; i++){
		page_directory[i] = 0 | 2; // attribute set to: supervisor level, read/write, not present(010 in binary)
	}

	// write_cr3, read_cr3, write_cr0, and read_cr0 all come from the assembly functions
	write_cr3(page_directory); // put that page directory address into CR3
	write_cr0(read_cr0() | 0x80000000); // set the paging bit in CR0 to 1

	// I thought it'd be a lot harder to implement,
	// but most architectures now have memory management units built in apparently.
}