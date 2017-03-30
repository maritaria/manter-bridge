#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

struct port_t {
	int port;
	int state;
	int initial_state;
	bool is_pending;
	int pending_state;
};



int ports_count = 0;
port_t ports[1000] = { 0 };
pthread_t my_thread;
int show_reads = 0;
int show_writes = 0;

int check_read(int port, const char* funcname) {
	if (show_reads) printf("utwente io read %x\n", port);
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
	printf("utwente io new read %x %x\n", port, state);
	return state;
}

void check_write(int port, int value, const char* funcname) {
	if (show_writes) printf("utwente io write %x %x\n", port, value);
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
	printf("utwente io new write %x %x\n", port, value);
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
		#define PRINT_IO(port, value, pending) printf("utwente_io(%x, %x, %x)\n", port, value, pending)
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
	int err = pthread_create(&my_thread, NULL, &utwente_thread, NULL);
	if (err != 0)
	{
		printf("utwente init() Thread failed [%s]\n", strerror(err));
		exit(-1);
	}
	else
	{
		printf("utwente init() Thread created\n");
	}
}

void utwente_shutdown() {
	
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
