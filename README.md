![ASIX](https://www.asix.com.tw/upload/2019_12_061/20191206160042ixfpyg5Wz1.jpg)

# [ASIX USB Ethernet ICs](https://www.asix.com.tw/en/product/USBEthernet)
* ASIX AX88772A USB 2.0 Fast Ethernet Network Adapter
* ASIX AX88772B USB 2.0 Fast Ethernet Network Adapter
* ASIX AX88772C USB 2.0 Fast Ethernet Network Adapter

## Prerequisites

Prepare to build the driver, you need the Linux kernel sources installed on the 
build machine, and make sure that the version of the running kernel must match 
the installed kernel sources. If you don't have the kernel sources, you can get 
it from [The Linux Kernel Archives](www.kernel.org) or contact to your Linux 
 distributor. If you don't know how to do, please refer to KERNEL-HOWTO.

**Note**: Please make sure the kernel is built with one of the "Support for 
Host-side, EHCI, OHCI, or UHCI" option support.

## Conditional Compilation Flag

* AX_FORCE_BUFF_ALIGN 

There are alignment issues of USB buffer in some USB host controllers. 
Turn on this flag if the implementation of your USB host controller 
cannot handle non-double word aligned buffer. 
When turn on this flag, driver will fixup egress packet aligned on double 
word boundary before deliver to USB host controller. 
Setting: 
   1 -> Enable TX buffers forced on double word alignment. 
   0 -> Disable TX buffers forced on double word alignment. 
Default:
   0

## Getting Start
1. Extract the compressed driver source file to your template directory by the 
 following command:
``` shell=
	$tar -xf DRIVER_SOURCE_PACKAGE.tar.bz2
```

2. Now, the driver source files should be extracted under the current directory. 
Executing the following command to compile the driver:
``` shell=
	$make
```
			
3. If the compilation is well, the asix.ko will be created under the current 
 directory.
 
4. If you want to use modprobe command to mount the driver, executing the 
 following command to install the driver into your Linux:
``` shell=
	$make install
```

## Usage
1. If you want to load the driver manually, go to the driver directory and 
 execute the following commands:
``` shell=
	$sudo insmod asix.ko
```

2. If you had installed the driver during driver compilation, then you can use 
 the following command to load the driver automatically.
``` shell=
	$sudo modprobe asix
```

If you want to unload the driver, just executing the following command:
``` shell=
	$sudo rmmod asix
```

## Special define
There is a RX_SKB_COPY preprocessor define in asix.h can solve rx_throttle problem 
in some version of 3.4 Linux kernel. Removing the comment before the define can 
 enable this feature.

-------------------------------------------------------------------------------
# Programmer
ASIX USB Ethernet Linux Command Line Programming Tool

##  Getting Start

1. Extract the compressed driver source file to your temporary directory by the
following command:
``` shell=
	$tar -xf DRIVER_SOURCE_PACKAGE.tar.bz2
```

2. Now, the driver source files should be extracted under the current directory.
Executing the following command to compile the driver:
```shell=
	$make
```

3. If the compilation is well, the ioctl will be created under the current 
 directory.

**Note**: The default way to find the interface is to scan the ASIX device using
 the ethx (x: 0~255).It is defined in the file, **command.h**.
```c=
	(As follows)
	...
	// DEFAULT_SCAN   : scan "eth0" - "eth255"
	// INTERFACE_SCAN : scan all available network interfaces
	#define NET_INTERFACE	DEFAULT_SCAN
	#define	DEFAULT_SCAN	0x00
	#define	INTERFACE_SCAN	0x01
	...
```
Adjust the contents of #define NET_INTERFACE to select the method you want.

## AX88772x EEPROM Programmer

1. If you want to read out values of the EEPROM to a file, go to the driver directory 
 and execute the following command:
``` shell=
	$./ioctl reeprom file_name eeprom_size
```

2. If you want to write values of a file to the EEPROM, go to the driver directory 
 and execute the following command:
``` shell=
	$./ioctl weeprom file_name eeprom_size
```

3. If you want to change the MAC address of a dongle, go to the driver directory 
 and execute the following command:
``` shell=
	$./ioctl chgmac mac_addr eeprom_size
```

4. If you need more information about the instructions, go to the driver directory 
 and execute the following commands:
``` shell=
	$./ioctl reeprom help
```
or
``` shell=
	$./ioctl weeprom help
```