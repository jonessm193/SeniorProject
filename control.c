#include <system.h>

unsigned char cmdString[5];
unsigned int count = 0;


unsigned int compare(unsigned char * a, unsigned char * b){
	unsigned int i;
	for(i = 0; i < 4; i++){
		if(a[i] != b[i])
			return 0;
	}
	return 1;
}

void filedone(){
	pullChars = 0;
}

void filem(){
	pullChars = 1;
}

void control_enterCommand(){
	if(compare(cmdString, (unsigned char*)"hello")){
		puts((unsigned char*)("My controller for simple terminal commands is working!\n"));
	}else if(compare(cmdString, (unsigned char*) "clear")){
		cls();
	}else if(compare(cmdString, (unsigned char*)("filem"))){
		filem();
	}else if(cmdString[0] == 0){
		puts((unsigned char*)("\n"));
	}else{
		puts((unsigned char*)("Unknown command\n"));
	}
	int i;
	for(i = 0; i < 50; i++){
		cmdString[i] = 0;
	}
	count = 0;
}

void writefilem(char finput){
	puts((unsigned char*)("Derpa derpa derpa\n"));
	filedone();
	control_enterCommand();
}

void control_key(char input){
	if(input == '\n'){
		control_enterCommand();
	}else{
		cmdString[count] = (unsigned char)input;
		count++;
	}
}