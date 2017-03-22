#include <stdio.h>

int read_ports_count = 0;
int read_ports[1000] = { 0 };

int write_ports_count = 0;
int write_ports[1000] = { 0 }; 

void utwente_init(){
	
}
void utwente_shutdown(){
	
}

void check_read(int port) {
	for(int i = 0; i < read_ports_count; i++) {
		if (read_ports[i] == port) return;
	}
	read_ports[read_ports_count++] = port;
	printf("utwente read %d\n", port);
}

void check_write(int port) {
	for(int i = 0; i < write_ports_count; i++) {
		if (write_ports[i] == port) return;
	}
	write_ports[write_ports_count++] = port;
	printf("utwente write %d\n", port);
}

unsigned char inb(unsigned short int port){
	check_read((int) port);
	return 0xFF;
}
unsigned char inb_p(unsigned short int port){
	check_read((int) port);
	return 0xFF;
}
unsigned short int inw(unsigned short int port){
	check_read((int) port);
	return 0xFF;
}
unsigned short int inw_p(unsigned short int port){
	check_read((int) port);
	return 0xFF;
}
unsigned int inl(unsigned short int port){
	check_read((int) port);
	return 0xFF;
}
unsigned int inl_p(unsigned short int port){
	check_read((int) port);
	return 0xFF;
}

void outb(unsigned char value, unsigned short int port){
	check_write((int) port);
}
void outb_p(unsigned char value, unsigned short int port){
	check_write((int) port);
}
void outw(unsigned short int value, unsigned short int port){
	check_write((int) port);
}
void outw_p(unsigned short int value, unsigned short int port){
	check_write((int) port);
}
void outl(unsigned int value, unsigned short int port){
	check_write((int) port);
}
void outl_p(unsigned int value, unsigned short int port){
	check_write((int) port);
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
