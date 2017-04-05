#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include "utwente.h"

int known_ports_count = 0;
digital_port known_ports[PORT_CAPACITY] = { 0 };
digital_port* find_port(int port) {
	for (int i = 0; i < known_ports_count; i++) {
		if (known_ports[i].port == port) {
			return &(known_ports[i]);
		}
	}
	return NULL;
}
digital_port* create_port() {
	int index = known_ports_count++;
	known_ports[index] = { 0 };
	return &(known_ports[index]);
}

void ut_add_port(int port_num)
{
	if (find_port(port_num) == NULL) {
		digital_port* port = create_port();
		port->port = port_num;
		port->state_raw = DEFAULT_DIGITAL;
		port->state_reset = DEFAULT_DIGITAL;
		port->forced_low = 0x00;
		port->forced_high = 0x00;
	}
}
void command_status(void);
void command_loop() {
	while (TRUE) {
		// Get line
		char user_string[1000];
		if (fgets(user_string, 1000, stdin) == NULL) { continue; }
		char *pos = strchr(user_string, '\n');
		if (pos == NULL) { continue; }
		// Trim newline from input
		*pos = '\0';
		// Split by spaces and parse
		const char* delimiters = " ";
		for (char* part = strtok(user_string, delimiters); part != NULL; part = strtok(NULL, delimiters)) {
			if (part == NULL) { break; }
			if (STRING_EQUALS(part, "continue")) {
				return;
			}
			if (STRING_EQUALS(part, "status")) {
				command_status();
			}
		}
	}
}

const char* binary(int value) {
	static const char binnums[16][5] = {
		"0000","0001","0010","0011",
		"0100","0101","0110","0111",
		"1000","1001","1010","1011",
		"1100","1101","1110","1111"
	};
	static const char* hexnums = "0123456789abcdef";
	static char inbuffer[16], outbuffer[4 * 16];
	const char *i;
	sprintf(inbuffer, "%x", value); // hexadecimal n -> inbuffer
	for(i = inbuffer; *i != 0; ++i) { // for each hexadecimal cipher
		int d = strchr(hexnums, *i) - hexnums; // store its decimal value to d
		char* o = outbuffer + (i - inbuffer) * 4; // shift four characters in outbuffer
		sprintf(o, "%s", binnums[d]); // place binary value of d there
	}
	return strchr(outbuffer, '1'); // omit leading zeros
}

void print_port(digital_port* port) {
	if (port == NULL) { return; }
	printf("utwente port(%x)\n", port->port);
	printf("utwente port raw         %02x %8s\n", port->state_raw, binary(port->state_raw));
	printf("utwente port reset       %02x %8s\n", port->state_raw, binary(port->state_reset));
	printf("utwente port forced-low  %02x %8s\n", port->state_raw, binary(port->forced_low));
	printf("utwente port forced-high %02x %8s\n", port->state_raw, binary(port->forced_high));
}

void command_status() {
	printf("utwente status start\n");
	for (int i = 0; i < known_ports_count; i++) {
		print_port(&(known_ports[i]));
	}
	printf("utwente status end\n");
}



int ut_get_port(int port_num) {
	digital_port* port = find_port(port_num);
	if (port == NULL) { return DEFAULT_DIGITAL; }
	// Check triggers
	bool breakpoint = FALSE;
	bool notify = FALSE;
	for (int i = 0; i < BIT_COUNT; i++) {
		trigger_mode trig = port->read_triggers[i];
		notify |= ((trig & NOTIFY) > 0);
		breakpoint |= ((trig & BREAK) > 0);
	}
	if (notify || breakpoint) {
		if (breakpoint) {
			printf("utwente trigger read break %02x %d\n", port_num, THREAD_ID);
			command_loop();
		} else {
			printf("utwente trigger read notify %02x %d\n", port_num, THREAD_ID);
		}
	}
	// Return value (can be changed by triggers)
	int value = port->state_raw;
	value &= ~(port->forced_low);
	value |= (port->forced_high);
	return value;
}

void ut_set_port(int port_num, int value) {
	digital_port* port = find_port(port_num);
	if (port == NULL) { return; }
	int changed_bits = port->state_raw ^ value;
	port->state_raw = value;
	bool breakpoint = FALSE;
	bool notify = FALSE;
	for (int i = 0; i < BIT_COUNT; i++) {
		if (HAS_FLAG(changed_bits, i)) {
			trigger_mode trig = port->write_triggers[i];
			notify |= ((trig & NOTIFY) > 0);
			breakpoint |= ((trig & BREAK) > 0);
		}
	}
	if (notify || breakpoint) {
		if (breakpoint) {
			printf("utwente trigger write break %02x %d\n", port_num, THREAD_ID);
			command_loop();
		} else {
			printf("utwente trigger write notify %02x %d\n", port_num, THREAD_ID);
		}
	}
}

bool ut_get_pin(int port_num, int bit) {
	return TRUE;
}
void ut_set_pin(int port_num, int bit, bool value) {
	digital_port* port = find_port(port_num);
	if (port != NULL) {
	}
}

void ut_lock_pin(int port_num, int bit, bool value) {
	digital_port* port = find_port(port_num);
	if (port != NULL) {
		if (value) {
			port->forced_low = REMOVE_FLAG(port->forced_low, bit);
			port->forced_high = ADD_FLAG(port->forced_high, bit);
		} else {
			port->forced_low = ADD_FLAG(port->forced_low, bit);
			port->forced_high = REMOVE_FLAG(port->forced_high, bit);
		}
	}
}
void ut_unlock_pin(int port_num, int bit) {
	digital_port* port = find_port(port_num);
	if (port != NULL) {
		port->forced_low = REMOVE_FLAG(port->forced_low, bit);
		port->forced_high = REMOVE_FLAG(port->forced_high, bit);
	}
}

void ut_trigger(int port_num, int bit, trigger_type type, trigger_mode mode) {
	digital_port* port = find_port(port_num);
	if (port != NULL) {
		if (type & READ) {
			port->read_triggers[bit] = mode;
		}
		if (type & WRITE) {
			port->write_triggers[bit] = mode;
		}
	}
}
