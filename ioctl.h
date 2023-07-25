#ifndef ioctl_h
#define ioctl_h

/* INCLUDE FILE DECLARATIONS */
#include "command.h"

/* CHANGE NETWORK INTERFACE WAY */
// DEFAULT_SCAN   : scan "eth0" - "eth255"
// INTERFACE_SCAN : scan all available network interfaces
#define NET_INTERFACE	DEFAULT_SCAN
#define	DEFAULT_SCAN	0x00
#define	INTERFACE_SCAN	0x01

/* NAMING CONSTANT DECLARATIONS */
struct ax_command_info { 
	int inet_sock;
	struct ifreq *ifr;
	int argc;
	char **argv;
	unsigned short ioctl_cmd;
	const char *help_ins;
	const char *help_desc;
};

const char help_str1[] =
"./ioctl help [command]\n"
"    -- command description\n";
const char help_str2[] =
"        [command] - Display usage of specified command\n";

const char readeeprom_str1[] =
"./ioctl reeprom [file] [size]\n"
"    -- AX88772B EEPROM read tool\n";
const char readeeprom_str2[] =
"        [file]    - Output file\n"
"        [size]    - EEPROM SIZE in bytes\n";

const char writeeeprom_str1[] =
"./ioctl weeprom [file] [size]\n"
"    -- AX88772B EEPROM write tool\n";
const char writeeeprom_str2[] =
"        [file]    - Input file\n"
"        [size]    - EEPROM SIZE in bytes\n";

const char chgmac_str1[] =
"./ioctl chgmac [mac_addr] [size]\n"
"    -- AX88772B EEPROM write tool (specify MAC address)\n";
const char chgmac_str2[] =
"        [mac_addr]- MAC address (xx:xx:xx:xx:xx:xx)\n"
"        [size]    - EEPROM SIZE in bytes\n";

/* EXPORTED SUBPROGRAM SPECIFICATIONS */
void help_func (struct ax_command_info *info);
void readeeprom_func(struct ax_command_info *info);
void writeeeprom_func(struct ax_command_info *info);
void chgmac_func(struct ax_command_info *info);
/* TYPE DECLARATIONS */

typedef void (*MENU_FUNC)(struct ax_command_info *info);

struct {
	char *cmd;
	unsigned short ioctl_cmd;
	MENU_FUNC OptFunc;
	const char *help_ins;
	const char *help_desc;
} command_list[] = {
	{"help",	AX_SIGNATURE,		help_func,		help_str1,		help_str2},
	{"reeprom",	AX_READ_EEPROM,		readeeprom_func,	readeeprom_str1,	readeeprom_str2},
	{"weeprom", 	AX_WRITE_EEPROM, 	writeeeprom_func,	writeeeprom_str1,	writeeeprom_str2},
	{"chgmac", 	AX_WRITE_EEPROM, 	chgmac_func,		chgmac_str1,		chgmac_str2},
	{NULL},
};

#endif /* End of console_debug_h */
