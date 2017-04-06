#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

#define PORT_CAPACITY (1000)
#define CHANNEL_CAPACITY (1000)
#define COMMAND_CAPACITY (128)
#define DEFAULT_DIGITAL (0xFF)
#define BIT_COUNT (8)
#define FALSE (0)
#define TRUE (1)
#define ADD_FLAG(X, BIT) (X | (1 << BIT))
#define REMOVE_FLAG(X, BIT) (X & ~(1 << BIT))
#define HAS_FLAG(X, BIT) HAS_MASK(X, 1 << BIT)
#define HAS_MASK(X, MASK) ((X | MASK) > 0)
#define STRING_EQUALS(INPUT, COMMAND) (strcmp(INPUT, COMMAND) == 0)
#define THREAD_ID ((pid_t)syscall(SYS_gettid))

typedef enum trigger_type {
	READ = 1,
	WRITE = 2,
} trigger_type;

typedef enum trigger_mode {
	DISABLED = 0,
	NOTIFY = 1,
	BREAK = 2,
} trigger_mode;

typedef struct digital_port {
	int port;
	int state_raw;
	int state_reset;
	int forced_low;
	int forced_high;
	const char* name;
	trigger_mode read_triggers[BIT_COUNT];
	trigger_mode write_triggers[BIT_COUNT];
} digital_port;

typedef struct analog_port {
	int channel;
	int state;
	bool is_override;
} analog_port;

typedef struct command_entry {
	const char* name;
	bool (*callback)(void);
} command_entry;

void ut_add_port(int port_num, const char* name);

int ut_get_port(int port_num);
void ut_set_port(int port_num, int value);

bool ut_get_pin(int port_num, int bit);
void ut_set_pin(int port_num, int bit, bool value);

void ut_lock_pin(int port_num, int bit, bool value);
void ut_unlock_pin(int port_num, int bit);

void ut_trigger(int port_num, int bit, trigger_type type, trigger_mode mode);

void ut_add_command(const char* name, bool (*callback)(void));
char* ut_command_next_word();