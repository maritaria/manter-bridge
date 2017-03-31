#ifndef test
#define test

#ifdef _SYS_IO_H
#error "Already included sys/io.h"
#endif

#define inb storage_inb
#define outb storage_outb
#include <sys/io.h>
#undef inb
#undef outb

extern void utwente_init();
extern void utwente_shutdown();
extern int utwente_adc(int channel, int simulated_value);

extern unsigned char inb(unsigned short int port);
extern unsigned char inb_p(unsigned short int port);
extern unsigned short int inw(unsigned short int port);
extern unsigned short int inw_p(unsigned short int port);
extern unsigned int inl(unsigned short int port);
extern unsigned int inl_p(unsigned short int port);

extern void outb(unsigned char value, unsigned short int port);
extern void outb_p(unsigned char value, unsigned short int port);
extern void outw(unsigned short int value, unsigned short int port);
extern void outw_p(unsigned short int value, unsigned short int port);
extern void outl(unsigned int value, unsigned short int port);
extern void outl_p(unsigned int value, unsigned short int port);

extern void insb(unsigned short int port, void *addr, unsigned long int count);
extern void insw(unsigned short int port, void *addr, unsigned long int count);
extern void insl(unsigned short int port, void *addr, unsigned long int count);
extern void outsb(unsigned short int port, const void *addr, unsigned long int count);
extern void outsw(unsigned short int port, const void *addr, unsigned long int count);
extern void outsl(unsigned short int port, const void *addr, unsigned long int count);

#endif
