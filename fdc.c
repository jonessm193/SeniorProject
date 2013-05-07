/* Floppy Disk Controller
 * Coded entirely by SJones with assistance in setup style from osDev.org
 * This setup does check for certian mechanical issues that aren't present in our
 * build as we use a floppy image.
 * There is also 'motor' enabling and checks. This is actually needed as bochs emulates physical hardware
 */

#include <system.h>

unsigned int baseFAddress = 300;

//Floppy register addresses to be accessed
enum FloppyRegisters{
	F_BASE 			= 0x3F0, //Base address for FDC
	F_DOR			= 0x3F2, //Digital Output Register
	F_MSR			= 0x3F4, //Master Status Register, read-only
	F_FIFO			= 0x3F5, //FIFO data register, DMA commands
	//There is a 0x3F6 register for hard disk ATA as well
	F_CCR			= 0x3F7  //Config Control Register, write-only
};


//Commands to be written to the DATA_FIFO port. There are three optional bits to be OR'd to some.
//Each command is followed by parameter bytes and returns result bytes.
//Command bytes may only be sent if the RQM bit is 1 and the DIO bit is 0. Otherwise a prev command failed and a reset is needed
enum FloppyCommands
{
   SPECIFY =                    3,      // * set drive parameters
   WRITE_DATA =                 5,      // * write to the disk
   READ_DATA =                  6,      // * read from the disk
   RECALIBRATE =                7,      // * seek to cylinder 0
   SENSE_INTERRUPT =            8,      // * ack IRQ6, get status of last command
   SEEK =                       15,     // * seek both heads to cylinder X
};

//'motor' control variables
enum{
	f_motor_off = 0,
	f_motor_on,
	f_motor_wait
};
static volatile int f_motor_ticks = 0;
static volatile int f_motor_state = 0;


//Write a command when ready
void f_writeCommand(unsigned char cmd){
	unsigned int i;
	for(i = 0; i < 600; i++){
		timer_wait(1);
		if(0x80 & inportb(F_MSR))
			return (void) outportb(F_FIFO, cmd);
	}
	panic((unsigned char*)("f_writeCommand: timeout"));
}

//read data when ready
unsigned char f_readData(){
	unsigned int i;
	for(i = 0; i < 600; i++){
		timer_wait(1);
		if(0x80 & inportb(F_MSR))
			return inportb(F_FIFO);
	}
	panic((unsigned char*)("f_readData: timout"));
	return 0; //not reached
}

//write Sense Interrupt command and read back data to check
void f_checkInterrupt(unsigned int * st0, unsigned int * cyl){
	f_writeCommand(SENSE_INTERRUPT);

	*st0 = f_readData();
	*cyl = f_readData();
}

void f_motor(unsigned int onoff){
	if(onoff){
		if(!f_motor_state){
			//then it needs to be on
			outportb(F_DOR, 0x1C);
			timer_wait(50);
		}
		f_motor_state = f_motor_on;
	}else{
		if(f_motor_state == f_motor_wait){
			panic((unsigned char*)("f_motor: The FD Monitor State was already waiting...whatever"));
		}
		f_motor_ticks = 300;
		f_motor_state = f_motor_wait;
	}

}

//Calibrate Drive
unsigned int f_calibrate(){
	unsigned int i, st0, cyl = -1; //Purposely wrong cylinder
	f_motor(f_motor_on);

	for(i = 0; i < 10; i++){
		f_writeCommand(RECALIBRATE);
		f_writeCommand((unsigned char)F_BASE);

		unsigned int j;
		for(j = 0; j < 100; j++){
			timer_wait(1);
			irq_wait(6);
		}
		f_checkInterrupt(&st0, &cyl);

		if(st0 & 0xC0){
			static const char * status[] = {0, "error", "invalid", "drive"};
			puts((unsigned char*) ("f_calibrate: status = "));
			puts((unsigned char*)(status[st0 >>6]));
			continue;
		}

		if(!cyl){
			f_motor(f_motor_off);
			return 0;
		}
	}

	panic((unsigned char*)("f_calibrate: Failed 10 times\n"));
	f_motor(f_motor_off);
	return -1;
}

//Reset floppy
unsigned int f_reset(){
	outportb(F_DOR, 0x00); //disable controller
	outportb(F_DOR, 0x0C); //enable it

	unsigned int j;
	for(j = 0; j < 100; j++){
			timer_wait(1);
			irq_wait(6);
	}
		

	{
		unsigned int st0, cyl;
		f_checkInterrupt(&st0, &cyl);
	}

	outportb(F_CCR, 0x00); //transfer speed setting

	f_writeCommand(SPECIFY);
	//some numbers corresponding to the specific drive type we're using, otherwise they'd be variables and adjustable
	f_writeCommand(0xdf);
	f_writeCommand(0x02);

	//if it failed
	if(f_calibrate()){
		return -1;
	}
	return 0;
}

unsigned int f_seek(unsigned cyli, unsigned int head){
	unsigned i, st0, cyl = -1;

	f_motor(f_motor_on);

	for(i = 0; i < 10; i++){
		f_writeCommand(SEEK);
		f_writeCommand(head<<2);
		f_writeCommand(cyli);

		unsigned int j;
		for(j = 0; j < 100; j++){
			timer_wait(1);
			irq_wait(6);
		}
		
		f_checkInterrupt(&st0, &cyl);

		if(st0 & 0xC0){
			static const char* status[] = {"normal", "error", "invalid", "drive"};
			puts((unsigned char*)("f_seek: status = "));
			puts((unsigned char*)(status[st0 >>6]));
			continue;
		}

		if(cyl == cyli){
			f_motor(f_motor_off);
			return 0;
		}
	}

	panic((unsigned char*)("f_seek: Failed 10 times\n"));
	f_motor(f_motor_off);
	return -1;
}

//Helpful to tell weather we're reading or writing for our DMA
typedef enum{
	f_dir_read = 1,
	f_dir_write = 2
} f_dir;

//Reserve memory for DMA to transfer with
#define f_dmalen 0x4800
static const char f_dmabuf[f_dmalen]__attribute__((aligned(0x8000)));

static void f_dma_init(f_dir dir){

	union{
		unsigned char b[4];
		unsigned long one;
	}a, c;

	a.one = (unsigned) &f_dmabuf;
	c.one = (unsigned) f_dmalen-1;

	//check size is correct in all dimensions
	if((a.one >> 24) || (c.one >>16) || (((a.one&0xFFFF)+c.one)>>16)){
		panic((unsigned char*)("f_dma_init: static buffer issue\n"));
	}	

	unsigned char mode;
	switch(dir){
		case f_dir_read: mode = 0x46; break;
		case f_dir_write: mode = 0x4A; break;
		default: panic((unsigned char*)("f_dma_init: invalid direction"));
				 return;
	}

	outportb(0x0A, 0x06); //mask chan 2

	outportb(0x0C, 0xFF); //reset flip-flop
	outportb(0x04, a.b[0]);	// address low byte
	outportb(0x04, a.b[1]); // address high byte

	outportb(0x81, a.b[2]); // external page register

	outportb(0x0C, 0xFF); //reset flip-flop
	outportb(0x05, c.b[0]);//count low byte
	outportb(0x05, c.b[1]);//count high byte

	outportb(0x0B, mode); //set mode
	outportb(0x0a, 0x02); //unmask chan 2
}


unsigned int f_do_track(unsigned cyl, f_dir dir){
	//transfer command
	unsigned char cmd;
	puts((unsigned char*)("Start"));

	//we're doing multitrack and keeping everything hardcoded to that to keep it simple
	static const int flags = 0xC0;
	switch(dir){
		case f_dir_read:
			cmd = READ_DATA | flags;
			break;
		case f_dir_write:
			cmd = WRITE_DATA | flags;
			break;
		default:
			panic((unsigned char*)("f_do_track: invalid direction"));
			return 0;
	}
	puts((unsigned char*)("After switch"));

	if(f_seek(cyl, 0)) return -1;
	if(f_seek(cyl, 0)) return -1;
	puts((unsigned char*)("Before loop"));
	unsigned int i;
	for(i = 0; i < 20; i++){
		puts((unsigned char*)("Start loop"));
		f_motor(f_motor_on);

		f_dma_init(dir);

		timer_wait(10);

		f_writeCommand(cmd);	//Current direction
		f_writeCommand(0);		//head
		f_writeCommand(cyl);	
		f_writeCommand(0);		//head
		f_writeCommand(1);		//first sector
		f_writeCommand(2);		//bytes per sector 2 -> 512
		f_writeCommand(18);		//# of tracks to do
		f_writeCommand(0x1B);	//GAP3, 27 is default number for 3.5" 1.44 drives
		f_writeCommand(0xFF);	//data length

		unsigned int j;
		for(j = 0; j < 100; j++){
			timer_wait(1);
			irq_wait(6);
		}
		
		puts((unsigned char*)("Reading data?"));
		unsigned char st0, st1, st2, rcy, rhe, rse, bps;
		st0 = f_readData();
		st1 = f_readData();
		st2 = f_readData();
		rcy = f_readData();
		rhe = f_readData();
		rse = f_readData();
		bps = f_readData();

		rcy++;
		rhe++;
		rse++;

		unsigned int error = 0;

		if(st0 & 0xC){
			static const char * status[] = {0, "error", "invalid command", "drive not ready"};
			puts((unsigned char*)("f_do_track: status = "));
			puts((unsigned char*)(status[st0 >>6]));
			error = 1;
		}
		if(st1 & 0x08){
			puts((unsigned char*)("f_do_track: end of cylinder\n"));
			error = 1;
		}
		if(st0 & 0x08){
			puts((unsigned char*)("f_do_track: drive not ready\n"));
			error = 1;
		}
		if(st1 & 0x20){
			puts((unsigned char*)("f_do_track: CRC error\n"));
			error = 1;
		}
		if(st1 & 0x10){
			puts((unsigned char*)("f_do_track: controller timeout\n"));
			error = 1;
		}
		if(st1 & 0x04){
			puts((unsigned char*)("f_do_track: no data found\n"));
			error = 1;
		}
		if((st1|st2) & 0x01){
			puts((unsigned char*)("f_do_track: no address mark found\n"));
			error = 1;
		}
		if(st2 & 0x40){
			puts((unsigned char*)("f_do_track: deleted address mark\n"));
			error = 1;
		}
		if(st2 & 0x20){
			puts((unsigned char*)("f_do_track: CRC error in data\n"));
			error = 1;
		}
		if(st2 & 0x10){
			puts((unsigned char*)("f_do_track: wrong cylinder\n"));
			error = 1;
		}
		if(st2 & 0x04){
			puts((unsigned char*)("f_do_track: uPD765 sector not found\n"));
			error = 1;
		}
		if(st2 & 0x02){
			puts((unsigned char*)("f_do_track: bad cylinder\n"));
			error = 1;
		}
		if(bps != 0x2){
			puts((unsigned char*)("f_do_track: wanted 512b/sector, got "));
			puts((unsigned char*)((1<<(bps+7))));
			error = 1;
		}
		if(st1 & 0x02){
			puts((unsigned char*)("f_do_track: not writable\n"));
			error = 2;
		}
		if(!error){
			f_motor(f_motor_off);
			return 0;
		}
		if(error>1){
			panic((unsigned char*)("f_do_track: Not retrying."));
			f_motor(f_motor_off);
			return -2;
		}
	}

	panic((unsigned char*)("f_do_track: Failed 20 times...\n"));
	f_motor(f_motor_off);
	return -1;
}

unsigned int f_read_track(unsigned cyl){
	return f_do_track(cyl, f_dir_read);
}

unsigned int f_write_track(unsigned cyl){
	return f_do_track(cyl, f_dir_write);
}

void fdc_install(){
	unsigned int x;
	x = f_calibrate();
	x++;
}

