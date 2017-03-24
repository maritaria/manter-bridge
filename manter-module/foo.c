#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

struct port_t {
	int port;
	int state;
};

int ports_count = 0;
port_t ports[1000] = { 0 };
pthread_t my_thread;

int check_read(int port, const char* funcname) {
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
		.state = state
	};
	printf("utwente io new read %x %x\n", port, state);
	return state;
}

void check_write(int port, int value, const char* funcname) {
	port_t* entry;
	for(int i = 0; i < ports_count; i++) {
		entry = &(ports[i]);
		if (entry->port == port) {
			entry->state = value;
			return;
		}
	}
	ports[ports_count++] = {
		.port = port,
		.state = value
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
		if (pos == NULL) { continue; }
		
		/* Trim newline from fgets */
		*pos = '\0';
		
		if (strcmp(user_string, "quit") == 0) {
			break;
		}
		const char* delimiters = " ";
		char* part = strtok(user_string, delimiters);
		
		#define MODE_GET 1
		#define MODE_SET 2
		#define PRINT_IO(port, value) printf("utwente_io(%x, %x)\n", port, value)
		int mode = 0;
		
		while (part != NULL) {
			
			if (strcmp(part, "get") == 0) {
				printf("utwente command now reading\n");
				mode = MODE_GET;
			}
			else if (strcmp(part, "set") == 0) {
				printf("utwente command now writing\n");
				mode = MODE_SET;
			}
			else if (strcmp(part, "status") == 0) {
				for (int i = 0; i < ports_count; i++) {
					port_t port = ports[i];
					PRINT_IO(port.port, port.state);
				}
			}
			else if (mode == MODE_GET) {
				int port = 0;
				sscanf(part, "%x", &port);
				int value = check_read(port, NULL);
				PRINT_IO(port, value);
			}
			else if (mode == MODE_SET) {
				int port = 0;
				int value = 0;
				sscanf(part, "%x_%x", &port, &value);
				check_write(port, value, NULL);
			}
			part = strtok(NULL, delimiters);
		}
	}
	
	exit(0);
	return NULL;
}

void utwente_init() {
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
