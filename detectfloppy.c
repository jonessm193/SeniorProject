/* SJones
	Floppy Disk Detection
	Somewhat unecessary in this regard as we know what type of floppy we are using
*/

#include <system.h>

void detectfloppy(){
	unsigned char c;
	unsigned char first;
	char *driveType[6] = {"no floppy drive",
							"360kb 5.25in floppy drive",
							"1.2mb 5.25in floppy drive",
							"720kb 3.5in",
							"1.44mb 3.5in",
							"2.88mb 3.5in"};
	//Make a request to the CMOS for data about the floppys detected
	outportb(0x70, 0x10);
	//Get the requested data
	c = inportb(0x71);

	first = c >>4; //get high portion
	//second = c & 0xF; //get low portion

	puts((unsigned char*)("Floppy drive A is an: \n"));
	puts((unsigned char*)(driveType[first]));
	puts((unsigned char*) "\n");
	//puts((unsigned char*)("Floppy drive B is an: \n"));
	//puts((unsigned char*)driveType[second]);
	
}