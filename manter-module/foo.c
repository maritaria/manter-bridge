#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "utwente.h"

struct port_t {
	int port;
	int state;
	int initial_state;
	bool is_pending;
	int pending_state;
};

struct channel_t {
	int channel;
	int state;
	bool is_override;
};

int adc_count = 0;

int ports_count = 0;
port_t ports[1000] = { 0 };
pthread_t my_thread;
int show_reads = 0;
int show_writes = 0;

int channel_count = 0;
channel_t channels[1000] = { 0 };

int check_read(int port, const char* funcname) {
	ut_get_port(port);
	if (show_reads) printf("utwente io read %02x\n", port);
	port_t* entry;
	for(int i = 0; i < ports_count; i++) {
		entry = &(ports[i]);
		if (entry->port == port) {
			return entry->state;
		}
	}
	int state = 0xFF;
	ports[ports_count++] = {
		.port = port,
		.state = state,
		.initial_state = state
	};
	printf("utwente io new read %x %02x\n", port, state);
	return state;
}

void check_write(int port, int value, const char* funcname) {
	ut_set_port(port, value);
	if (show_writes) printf("utwente io write %02x %02x\n", port, value);
	bool pending_write = (funcname && strcmp("pending", funcname) == 0);
	port_t* entry;
	for(int i = 0; i < ports_count; i++) {
		entry = &(ports[i]);
		if (entry->port == port) {
			if (pending_write) {
				entry->pending_state = value;
				entry->is_pending = 1;
			} else {
				entry->state = value;
			}
			return;
			
		}
	}
	ports[ports_count++] = {
		.port = port,
		.state = value,
		.initial_state = value,
		.is_pending = 0,
	};
	printf("utwente io new write %x %02x\n", port, value);
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

void* utwente_thread(void* arg) {
	puts("utwente thread hello");
	
	while(1) {
		char user_string[1000];
		if(fgets(user_string, 1000, stdin) == NULL) 
		{
			continue;
		}
		char *pos = strchr(user_string, '\n');
		if (pos == NULL) { puts("\\n not found"); continue; }
		
		/* Trim newline from fgets */
		*pos = '\0';
		
		if (strcmp(user_string, "quit") == 0) {
			break;
		}
		
		const char* delimiters = " ";
		char* part = strtok(user_string, delimiters);
		
		#define MODE_GET 1
		#define MODE_SET 2
		#define MODE_PENDING 3
		#define PRINT_IO(port, value, pending) printf("utwente_io(%02x, %02x, %x)\n", port, value, pending)
		#define PRINT_ADC(channel, value, override) printf("utwente_adc(%d, %d, %s)\n", channel, value, override ? "override" : "simulated")
		int mode = 0;
		
		while (part != NULL) {
			if (strcmp(part, "reset") == 0) {
				for(int i = 0; i < ports_count;i++) {
					port_t* port = &(ports[i]);
					port->state = port->initial_state;
				}
			}
			else if (strcmp(part, "apply") == 0) {
				for(int i = 0; i < ports_count;i++) {
					port_t* port = &(ports[i]);
					if (!port->is_pending) continue;
					port->is_pending = 0;
					port->state = port->pending_state;
				}
			}
			else if (strcmp(part, "show_reads") == 0) {
				show_reads = 1;
			}
			else if (strcmp(part, "show_writes") == 0) {
				show_writes = 1;
			}
			else if (strcmp(part, "hide_reads") == 0) {
				show_reads = 0;
			}
			else if (strcmp(part, "hide_writes") == 0) {
				show_writes = 0;
			}
			else if (strcmp(part, "get") == 0) {
				printf("utwente command now reading\n");
				mode = MODE_GET;
			}
			else if (strcmp(part, "set") == 0) {
				printf("utwente command now writing\n");
				mode = MODE_SET;
			}
			else if (strcmp(part, "preset") == 0) {
				printf("utwente command now preparing\n");
				mode = MODE_PENDING;
			}
			else if (strcmp(part, "status") == 0) {
				for (int i = 0; i < ports_count; i++) {
					port_t port = ports[i];
					PRINT_IO(port.port, port.state, (port.is_pending ? port.pending_state : -1));
				}
				for (int i = 0; i < channel_count; i++){
					channel_t channel = channels[i];
					PRINT_ADC(channel.channel, channel.state, channel.is_override);
				}
			}
			else if (strcmp(part, "adc_count") == 0) {
				printf("utwente_adc_count(%d)\n", adc_count);
				adc_count = 0;
			}
			else if (strcmp(part, "status2") == 0) {
				printf("utwente_status2:");
				for (int i = 0; i < ports_count; i++) {
					port_t port = ports[i];
					printf(" %02x,%02x", port.port, port.state);
				}
				printf("\n");
			}
			else if (mode == MODE_GET) {
				int port = 0;
				sscanf(part, "%x", &port);
				int value = check_read(port, NULL);
				PRINT_IO(port, value, -1);
			}
			else if (mode == MODE_SET) {
				int port = 0;
				int value = 0;
				sscanf(part, "%x_%x", &port, &value);
				check_write(port, value, NULL);
			}
			else if (mode == MODE_PENDING) {
				int port = 0;
				int value = 0;
				sscanf(part, "%x_%x", &port, &value);
				check_write(port, value, "pending");
			}
			part = strtok(NULL, delimiters);
		}
	}
	
	exit(0);
	return NULL;
}

void utwente_init() {
	setbuf(stdout, NULL);
	
	ut_add_port(0x215);
	ut_add_port(0x216);
	ut_add_port(0x300);
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
	
	int before = 0x00;
	printf("utwente adding flag %02x -> %02x\n", before, ADD_FLAG(before, 3));
	printf("utwente adding flag %02x -> %02x\n", before, ADD_FLAG(before, 4));
	printf("utwente adding flag %02x -> %02x\n", before, ADD_FLAG(before, 5));
	printf("utwente adding flag %02x -> %02x\n", before, ADD_FLAG(before, 6));
	
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
