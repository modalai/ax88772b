/* INCLUDE FILE DECLARATIONS */
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/sockios.h>
#include <ctype.h>
#include <stdlib.h>
#if NET_INTERFACE == INTERFACE_SCAN
#include <ifaddrs.h>
#endif
#include "ioctl.h"

/* STATIC VARIABLE DECLARATIONS */
#define AX88772C_IOCTL_VERSION "AX88772C/AX88772B/AX88772A/AX88760/AX88772/AX88178 Linux SROM IOCTL Tool version 1.7.0"

/* LOCAL SUBPROGRAM DECLARATIONS */
static unsigned long STR_TO_U32(const char *cp,char **endp,unsigned int base);


/* LOCAL SUBPROGRAM BODIES */
static void debug_func(char *func_name, unsigned short *buf, unsigned long wLen)
{
#if (DEBUG_PARAM & DEB_TOOL)
	int i;
	char str_buf[50];
	printf("%s :\n", func_name);
	printf("---------------------------------------\n");
	for (i = 0; i < wLen / 8; i++) {
		int j = 8 * i;
		snprintf(str_buf, 50,
				 "%04x %04x %04x %04x %04x %04x "
				 "%04x %04x\n", 
				 *(buf + j + 0), *(buf + j + 1),
				 *(buf + j + 2), *(buf + j + 3),
				 *(buf + j + 4), *(buf + j + 5),
				 *(buf + j + 6), *(buf + j + 7));
		printf("%s", str_buf);
	}
	printf("------------------------------------%3ld\n", wLen);
#endif
}


static void show_usage(void)
{
	int i;
	printf ("\n%s\n",AX88772C_IOCTL_VERSION);
	printf ("Usage:\n");
	for (i = 0; command_list[i].cmd != NULL; i++)
		printf ("%s\n", command_list[i].help_ins);
}

static unsigned long STR_TO_U32(const char *cp,char **endp,unsigned int base)
{
	unsigned long result = 0,value;

	if (*cp == '0') {
		cp++;
		if ((*cp == 'x') && isxdigit(cp[1])) {
			base = 16;
			cp++;
		}
		if (!base) {
			base = 8;
		}
	}
	if (!base) {
		base = 10;
	}
	while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
	    ? toupper(*cp) : *cp)-'A'+10) < base) {
		result = result*base + value;
		cp++;
	}
	if (endp)
		*endp = (char *)cp;

	return result;
}

void help_func (struct ax_command_info *info)
{
	int i;

	if (info->argv[2] == NULL) {
		for(i=0; command_list[i].cmd != NULL; i++) {
			printf ("%s%s\n", command_list[i].help_ins, command_list[i].help_desc);
		}
	}

	for (i = 0; command_list[i].cmd != NULL; i++)
	{
		if (strncmp(info->argv[1], command_list[i].cmd, strlen(command_list[i].cmd)) == 0 ) {
			printf ("%s%s\n", command_list[i].help_ins, command_list[i].help_desc);
			return;
		}
	}

}

int compare_file(struct ax_command_info *info)
{
	struct ifreq *ifr = (struct ifreq *)info->ifr;
	unsigned short *rout_buf;
	unsigned short *ori_buf;
	AX_IOCTL_COMMAND *ioctl_cmd = (AX_IOCTL_COMMAND *)(ifr->ifr_data);
	int i;

	rout_buf = malloc(sizeof(unsigned short) * ioctl_cmd->size);

	ori_buf = ioctl_cmd->buf;	

	ioctl_cmd->ioctl_cmd = AX_READ_EEPROM;
	ioctl_cmd->buf = rout_buf;

	if (ioctl(info->inet_sock, AX_PRIVATE, ifr) < 0) {
		perror("ioctl");
		return -1;
	}

	

	for (i = 0; i < ioctl_cmd->size; i++) {
		if (*(ioctl_cmd->buf + i) != *(ori_buf + i)) {
			debug_func("compare_file reeprom", rout_buf, ioctl_cmd->size);	
			ioctl_cmd->buf = ori_buf;
			free(rout_buf);				
			return -2;
		}
	}

	ioctl_cmd->buf = ori_buf;
	free(rout_buf);
	return 0;
}

void readeeprom_func(struct ax_command_info *info)
{
	struct ifreq *ifr = (struct ifreq *)info->ifr;
	AX_IOCTL_COMMAND ioctl_cmd;
	unsigned short *buf;
	unsigned short wLen;
	char str_buf[50];
	FILE *pFile;
	int i;	

	if (info->argc < 4) {
		for(i=0; command_list[i].cmd != NULL; i++) {
			if (strncmp(info->argv[1], command_list[i].cmd, 
					strlen(command_list[i].cmd)) == 0 ) {
				printf ("%s%s\n", command_list[i].help_ins, 
						command_list[i].help_desc);
				return;
			}
		}
	}	

	wLen = STR_TO_U32(info->argv[3], NULL, 0) / 2;

	pFile = fopen(info->argv[2],"w");
	buf = (unsigned short *)malloc((sizeof(unsigned short) * wLen));

	ioctl_cmd.ioctl_cmd = info->ioctl_cmd;
	ioctl_cmd.size = wLen;
	ioctl_cmd.buf = buf;
	ioctl_cmd.delay = 0;

	ifr->ifr_data = (caddr_t)&ioctl_cmd;
	if (ioctl(info->inet_sock, AX_PRIVATE, ifr) < 0) {
		perror("ioctl");
		free(buf);
		fclose(pFile);
		return;
	}
	for (i = 0; i < wLen / 8; i++) {
		int j = 8 * i;
		snprintf(str_buf, 50,
				 "%04x %04x %04x %04x %04x %04x "
				 "%04x %04x\n", 
				 *(buf + j + 0), *(buf + j + 1),
				 *(buf + j + 2), *(buf + j + 3),
				 *(buf + j + 4), *(buf + j + 5),
				 *(buf + j + 6), *(buf + j + 7));
		fputs(str_buf, pFile);
	}
	printf("Read completely\n\n");
	free(buf);
	fclose(pFile);
	return;
}

void writeeeprom_func(struct ax_command_info *info)
{
	struct ifreq *ifr = (struct ifreq *)info->ifr;
	AX_IOCTL_COMMAND ioctl_cmd;
	int i;
	unsigned short *buf;
	unsigned short wLen;
	char c[2] = {'\0'};
	FILE *pFile;
	unsigned char retried = 0;

	if (info->argc < 4) {
		for(i=0; command_list[i].cmd != NULL; i++) {
			if (strncmp(info->argv[1], command_list[i].cmd,
					strlen(command_list[i].cmd)) == 0) {
				printf ("%s%s\n", command_list[i].help_ins,
						command_list[i].help_desc);
				return;
			}
		}
	}

	pFile = fopen(info->argv[2], "r");

	wLen = STR_TO_U32(info->argv[3], NULL, 0) / 2;

	buf = (unsigned short *)malloc(sizeof(unsigned short) * wLen);

	for (i = 0; i < wLen / 8; i++) {
		int j = 8 * i;
		fscanf(pFile, "%04X %04X %04X %04X %04X %04X %04X %04X%c", 
				(unsigned int *)&buf[j + 0], (unsigned int *)&buf[j + 1],
				(unsigned int *)&buf[j + 2], (unsigned int *)&buf[j + 3],
				(unsigned int *)&buf[j + 4], (unsigned int *)&buf[j + 5],
				(unsigned int *)&buf[j + 6], (unsigned int *)&buf[j + 7], c);
	}

	ioctl_cmd.ioctl_cmd = info->ioctl_cmd;
	ioctl_cmd.size = wLen;
	ioctl_cmd.buf = buf;
	ioctl_cmd.delay = 5;
io:	
	ifr->ifr_data = (caddr_t)&ioctl_cmd;

debug_func("writeeeprom_func", buf, wLen);

 	if (ioctl(info->inet_sock, AX_PRIVATE, ifr) < 0) {
		free(buf);
		fclose(pFile);
		perror("ioctl");
		return;
	}
	else {
		if (compare_file(info) && retried < 3) { 
			ioctl_cmd.delay += 5;
			ioctl_cmd.ioctl_cmd = info->ioctl_cmd;
			retried++;
			goto io;
		}
		if (retried == 3) {
			printf("Failure to write\n\n");
			free(buf);
			fclose(pFile);
			return;
		}
	}

	printf("Write completely\n\n");
	free(buf);
	fclose(pFile);	
	return;
}

void chgmac_func(struct ax_command_info *info)
{
	struct ifreq *ifr = (struct ifreq *)info->ifr;
	AX_IOCTL_COMMAND ioctl_cmd;
	int i;
	unsigned short *buf;
	unsigned int tmp;
	unsigned short wLen;
	unsigned char retried = 0;
	char * pch;
	unsigned int MAC[6] = {0};
	int ret = 0;

	if (info->argc < 4) {
		for(i=0; command_list[i].cmd != NULL; i++) {
			if (strncmp(info->argv[1], command_list[i].cmd,
					strlen(command_list[i].cmd)) == 0) {
				printf ("%s%s\n", command_list[i].help_ins,
						command_list[i].help_desc);
				return;
			}
		}
	}

	wLen = STR_TO_U32(info->argv[3], NULL, 0) / 2;

	buf = (unsigned short *)malloc(sizeof(unsigned short) * wLen);

	ioctl_cmd.ioctl_cmd = AX_READ_EEPROM;
	ioctl_cmd.size = wLen;
	ioctl_cmd.buf = buf;
	ioctl_cmd.delay = 0;

	ifr->ifr_data = (caddr_t)&ioctl_cmd;

	if (ioctl(info->inet_sock, AX_PRIVATE, ifr) < 0) {
		perror("ioctl");
		free(buf);
		return;
	}

	ret = sscanf(info->argv[2], "%02X:%02X:%02X:%02X:%02X:%02X",(unsigned int*)&MAC[0],
								(unsigned int*)&MAC[1],
								(unsigned int*)&MAC[2],
								(unsigned int*)&MAC[3],
								(unsigned int*)&MAC[4],
								(unsigned int*)&MAC[5]);
	if (6 != ret) {
		printf("Invalid MAC address\n\n");
		return;
	}
	
	if (*buf == 0x1500) {
		printf("No eeprom exists!\n");
		printf("Or you must burn the default value first!\n\n");
		return;
	}

	*(((char*)buf) + 8) = (unsigned char)MAC[1];
	*(((char*)buf) + 9) = (unsigned char)MAC[0];
	*(((char*)buf) + 10) = (unsigned char)MAC[3];
	*(((char*)buf) + 11) = (unsigned char)MAC[2];
	*(((char*)buf) + 12) = (unsigned char)MAC[5];
	*(((char*)buf) + 13) = (unsigned char)MAC[4]; 

	ioctl_cmd.ioctl_cmd = info->ioctl_cmd;
	ioctl_cmd.size = wLen;
	ioctl_cmd.buf = buf;
	ioctl_cmd.delay = 5;

io:	
	ifr->ifr_data = (caddr_t)&ioctl_cmd;

debug_func("chgmac_func", buf, wLen);

  	if (ioctl(info->inet_sock, AX_PRIVATE, ifr) < 0) {
		perror("ioctl");
		free(buf);
		return;
	}

	else {
		if (compare_file(info) && retried < 3) { 
			ioctl_cmd.delay += 5;
			ioctl_cmd.ioctl_cmd = info->ioctl_cmd;
			retried++;
			goto io;
		}
		if (retried == 3) {
			printf("Failure to write\n\n");
			free(buf);
			return;
		}
	}

	printf("Write completely\n\n");
	free(buf);
	return;
}

/* EXPORTED SUBPROGRAM BODIES */
int main(int argc, char **argv)
{
#if NET_INTERFACE == INTERFACE_SCAN
	struct ifaddrs *addrs, *tmp;
	unsigned char	dev_exist;
#endif	
	struct ifreq ifr;
	struct ax_command_info info;
	AX_IOCTL_COMMAND ioctl_cmd;
	int inet_sock;
	unsigned char i;	

	if (argc < 2) {
		show_usage();
		return 0;
	}

	inet_sock = socket(AF_INET, SOCK_DGRAM, 0);

#if NET_INTERFACE == INTERFACE_SCAN
	/* Get Device */
	getifaddrs(&addrs);
	tmp = addrs;
	dev_exist = 0;

	while (tmp) {
		memset (&ioctl_cmd, 0, sizeof (AX_IOCTL_COMMAND));
		ioctl_cmd.ioctl_cmd = AX_SIGNATURE;
		// get network interface name
		sprintf (ifr.ifr_name, "%s", tmp->ifa_name);

		ifr.ifr_data = (caddr_t)&ioctl_cmd;
		tmp = tmp->ifa_next;


		if (ioctl (inet_sock, AX_PRIVATE, &ifr) < 0) {			
			continue;
		}
		
		if (strncmp (ioctl_cmd.sig, AX88772B_DRV_NAME, strlen(AX88772B_DRV_NAME)) == 0 ) {
			dev_exist = 1;
			break;
		}			
	}

	freeifaddrs(addrs);

	if (dev_exist == 0) {
		printf ("\n%s\n",AX88772C_IOCTL_VERSION);
		printf("No %s found\n\n", AX88772B_SIGNATURE);
		return 0;
	}
#else
	for (i = 0; i < 255; i++) {

		memset (&ioctl_cmd, 0, sizeof (AX_IOCTL_COMMAND));
		ioctl_cmd.ioctl_cmd = AX_SIGNATURE;

		sprintf (ifr.ifr_name, "eth%d", i);
		ifr.ifr_data = (caddr_t)&ioctl_cmd;
		
		if (ioctl (inet_sock, AX_PRIVATE, &ifr) < 0) {
			continue;
		}

		if (strncmp (ioctl_cmd.sig, AX88772B_DRV_NAME, strlen(AX88772B_DRV_NAME)) == 0 ) {
			break;
		}

	}

	if (i == 255) {
		printf ("\n%s\n",AX88772C_IOCTL_VERSION);
		printf ("No %s found\n\n", AX88772B_SIGNATURE);
		return 0;
	}
#endif
	for(i=0; command_list[i].cmd != NULL; i++)
	{
		if (strncmp(argv[1], command_list[i].cmd, strlen(command_list[i].cmd)) == 0 ) {
			printf ("\n%s\n",AX88772C_IOCTL_VERSION);
			info.help_ins = command_list[i].help_ins;
			info.help_desc = command_list[i].help_desc;
			info.ifr = &ifr;
			info.argc = argc;
			info.argv = argv;
			info.inet_sock = inet_sock;
			info.ioctl_cmd = command_list[i].ioctl_cmd;
			(command_list[i].OptFunc)(&info);
			return 0;
		}
	}

	printf ("Wrong command\n\n");

	return 0;
}

