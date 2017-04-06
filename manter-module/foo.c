#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "utwente.h"

struct channel_t {
	int channel;
	int state;
	bool is_override;
};

int channel_count = 0;
channel_t channels[1000] = { 0 };

int check_read(int port, const char* funcname) {
	return ut_get_port(port);
}

void check_write(int port, int value, const char* funcname) {
	ut_set_port(port, value);
}

int check_adc_read(int channel, int simulated_value, const char* funcname) {
	channel_t* entry;
	for(int i = 0; i < channel_count; i++) {
		entry = &(channels[i]);
		if (entry->channel == channel) {
			if (!entry->is_override) {
				entry->state = simulated_value;
			}
			return entry->state;
		}
	}
	channels[channel_count++] = {
		.channel = channel,
		.state = simulated_value,
		.is_override = 0,
	};
	printf("utwente adc new read %02d\n", channel);
	return simulated_value;
}

void utwente_init() {
	setbuf(stdout, NULL);
	
	ut_add_port(0x215, "test1");
	ut_add_port(0x216, "test2");
	ut_add_port(0x300, "test3");
	for (int i = 0; i < BIT_COUNT; i++) {
		ut_trigger(0x215, i, WRITE, BREAK);
		ut_trigger(0x216, i, WRITE, BREAK);
	}
	ut_lock_pin(0x300, 0, TRUE);
	ut_lock_pin(0x300, 1, TRUE);
	ut_lock_pin(0x300, 2, TRUE);
	ut_lock_pin(0x300, 3, TRUE);
	ut_lock_pin(0x300, 4, FALSE);
	ut_lock_pin(0x300, 5, FALSE);
	ut_lock_pin(0x300, 6, FALSE);
	ut_lock_pin(0x300, 7, FALSE);
	
}

void utwente_shutdown() {
	
}

int utwente_adc(int channel, int simulated_value) {
	return check_adc_read(channel, simulated_value, "adc");
}

unsigned char inb(unsigned short int port) {
	return check_read((int) port, "inb");
}

unsigned char inb_p(unsigned short int port) {
	return check_read((int) port, "inb_p");
}
unsigned short int inw(unsigned short int port) {
	return check_read((int) port, "inw");
}
unsigned short int inw_p(unsigned short int port) {
	return check_read((int) port, "inw_p");
}
unsigned int inl(unsigned short int port) {
	return check_read((int) port, "inl");
}
unsigned int inl_p(unsigned short int port) {
	return check_read((int) port, "inl_p");
}



void outb(unsigned char value, unsigned short int port) {
	check_write((int) port, value, "outb");
}

void outb_p(unsigned char value, unsigned short int port) {
	check_write((int) port, value, "outb_p");
}
void outw(unsigned short int value, unsigned short int port) {
	check_write((int) port, value, "outw");
}
void outw_p(unsigned short int value, unsigned short int port) {
	check_write((int) port, value, "outw_p");
}
void outl(unsigned int value, unsigned short int port) {
	check_write((int) port, value, "outl");
}
void outl_p(unsigned int value, unsigned short int port) {
	check_write((int) port, value, "outl_p");
}

void insb(unsigned short int port, void *addr, unsigned long int count) {
	for (unsigned long int i = 0; i < count; i ++) {
		check_read(port + i, "insb");
	}
}
void insw(unsigned short int port, void *addr, unsigned long int count) {
	for (unsigned long int i = 0; i < count; i ++) {
		check_read(port + i, "insb");
	}
	
}
void insl(unsigned short int port, void *addr, unsigned long int count) {
	for (unsigned long int i = 0; i < count; i ++) {
		check_read(port + i, "insb");
	}
	
}
void outsb(unsigned short int port, const void *addr, unsigned long int count) {
	for (unsigned long int i = 0; i < count; i ++) {
		check_write(port + i, 0, "insb");
	}	
}
void outsw(unsigned short int port, const void *addr, unsigned long int count) {
	for (unsigned long int i = 0; i < count; i ++) {
		check_write(port + i, 0, "insb");
	}
}
void outsl(unsigned short int port, const void *addr, unsigned long int count) {
	for (unsigned long int i = 0; i < count; i ++) {
		check_write(port + i, 0, "insb");
	}
}
