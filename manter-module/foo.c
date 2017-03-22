#include <stdio.h>

struct pstate_t {
	int port;
	int state;
};

int read_ports_count = 0;
pstate_t read_ports[1000] = { 0 };

int write_ports_count = 0;
pstate_t write_ports[1000] = { 0 };

void utwente_init(){
	
}

void utwente_shutdown(){
	
}

int check_read(int port, const char* funcname) {
	pstate_t entry;
	for(int i = 0; i < read_ports_count; i++) {
		entry = read_ports[i];
		if (entry.port == port) {
			printf("utwente read %x %x\n", port, entry.state);
			return entry.state;
		}
	}
	int state = 0xFF;
	read_ports[read_ports_count++] = {
		.port = port,
		.state = state
	};
	printf("utwente read new %x %x\n", port, entry.state);
	return state;
}

void check_write(int port, int value, const char* funcname) {
	pstate_t entry;
	for(int i = 0; i < write_ports_count; i++) {
		entry = write_ports[i];
		if (entry.port == port) {
			entry.state = value;
			printf("utwente write %x %x\n", port, value);
			return;
		}
	}
	write_ports[write_ports_count++] = {
		.port = port,
		.state = value
	};
	printf("utwente write new %x %x\n", port, value);
}

unsigned char inb(unsigned short int port){
	return check_read((int) port, "inb");
}

unsigned char inb_p(unsigned short int port){
	return check_read((int) port, "inb_p");
}
unsigned short int inw(unsigned short int port){
	return check_read((int) port, "inw");
}
unsigned short int inw_p(unsigned short int port){
	return check_read((int) port, "inw_p");
}
unsigned int inl(unsigned short int port){
	return check_read((int) port, "inl");
}
unsigned int inl_p(unsigned short int port){
	return check_read((int) port, "inl_p");
}



void outb(unsigned char value, unsigned short int port){
	check_write((int) port, value, "outb");
}

void outb_p(unsigned char value, unsigned short int port){
	check_write((int) port, value, "outb_p");
}
void outw(unsigned short int value, unsigned short int port){
	check_write((int) port, value, "outw");
}
void outw_p(unsigned short int value, unsigned short int port){
	check_write((int) port, value, "outw_p");
}
void outl(unsigned int value, unsigned short int port){
	check_write((int) port, value, "outl");
}
void outl_p(unsigned int value, unsigned short int port){
	check_write((int) port, value, "outl_p");
}

void insb(unsigned short int port, void *addr, unsigned long int count){
	
}
void insw(unsigned short int port, void *addr, unsigned long int count){
	
}
void insl(unsigned short int port, void *addr, unsigned long int count){
	
}
void outsb(unsigned short int port, const void *addr, unsigned long int count){
	
}
void outsw(unsigned short int port, const void *addr, unsigned long int count){
	
}
void outsl(unsigned short int port, const void *addr, unsigned long int count){
	
}
