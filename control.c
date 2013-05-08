/* Coded entirely by SJones
	This is a controller for the 'terminal'-esque view now present in the OS.
	Few commands are accepted and all others are 'unknown'
*/

#include <system.h>

unsigned char cmdString[5];
unsigned int count = 0;

//Compares two character byte arrays to check validity
unsigned int compare(unsigned char * a, unsigned char * b){
	unsigned int i;
	for(i = 0; i < 4; i++){
		if(a[i] != b[i])
			return 0;
	}
	return 1;
}

//Sets variable in kb.c to continue reading characters for command recognition
void filedone(){
	pullChars = 0;
}

//Sets variable in kb.c to start reading characters to a file
void filew(){
	pullChars = 1;
}

void filer(){
	pullChars = 2;
}

//Goofy method to prove concept of the commands working
void faces(){
	puts((unsigned char*)("^_^ "));
	puts((unsigned char*)("^_~ "));
	puts((unsigned char*)(">_< "));
	puts((unsigned char*)("-_- "));
	puts((unsigned char*)("O_o\n"));
}

//Sorts out commands taken in
void control_enterCommand(){
	if(compare(cmdString, (unsigned char*) "clear")){
		cls();
	}else if(compare(cmdString, (unsigned char*)("filew"))){
		filew();
	}else if(compare(cmdString, (unsigned char*)("filer"))){
		filer();
	}else if(compare(cmdString, (unsigned char*)("hello"))){
		puts((unsigned char*)("HI!!\n"));
	}else if(compare(cmdString, (unsigned char*)("flopp"))){
		detectfloppy();
	}else if(compare(cmdString, (unsigned char*)("-help"))){
		puts((unsigned char*)("--------------------------------------------------------------\n"));
		puts((unsigned char*)("Commands currently implemented:\n"));
		puts((unsigned char*)("clear = Clears the screen\n"));
		puts((unsigned char*)("hello = Says hi!!\n"));
		puts((unsigned char*)("faces = Just for fun\n"));
		puts((unsigned char*)("flopp = Find out what type of floppy we're on\n"));
		puts((unsigned char*)("--------------------------------------------------------------\n"));
		puts((unsigned char*)("Tried to get some simple file reading and writing commands.\n"));
		puts((unsigned char*)("Maybe after some time I'll get a chance in later versions.\n"));
		puts((unsigned char*)("--------------------------------------------------------------\n"));
	}else if(cmdString[0] == 0){
		puts((unsigned char*)("Type '-help' for assistance\n"));
	}else if(compare(cmdString, (unsigned char*)("faces"))){
		faces();
	}else{
		puts((unsigned char*)("Unknown command. Try '-help'?\n"));
	}
	int i;
	for(i = 0; i < 50; i++){
		cmdString[i] = 0;
	}
	count = 0;
}

//Calls to read files with the FDC, but for now it just prints something
void readfile(){
	f_read_track(1);
	filedone();
	control_enterCommand();
}

//Calls to write files with the FDC, but for now it just prints something
void writefile(char input){
	f_write_track(1);
	filedone();
	control_enterCommand();
}

//This takes in keyboard input to be sent for deciphering upon receipt of the return key
void control_key(char input){
	if(input == '\n'){
		control_enterCommand();
	}else{
		cmdString[count] = (unsigned char)input;
		count++;
	}
}