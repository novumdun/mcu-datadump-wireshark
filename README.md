# WiresharkDump

* Software Ver: v1.0.0
* Intro: A tool that dumps data from mcu to pc and puts data into Wireshark to be analysed

## Brief

This project was created by me as a component for [OneOS](https://gitee.com/cmcc-oneos/OneOS) to dump data from mcu to pc and analyse data in Wireshark. It can be ported to other RTOS or bare metal system.

## Purpose

Sometimes we need to analyse protocol (as BT/ETH/TCPIP/...) used in Mcu, but it is annoying to analyse protocol data by just printing them out in Hecadecimal format or other format. We are used to analyse protocol in GUI tools that can display protocol data with data bit definition. As Wireshark is a great protocol analysing tool and supports many protocols, I decided to build this project to achieve this goal.

## Structure

```shell
└── wiresharkdump  
    ├── device  
    └── host  
```

### device

Code running on devices that you want to dump data from. It outputs data you want to dump in some frame format to host (PC).

### host

Code running on PC. It receives data from a device and transports them to wireshark through the Pipe created in the initialization process.

### frame format

All protocol data are transported in following frame format, the 'data' is the actual protocol data.

|  1B  |  2B  |  4B  |  1B  |  NB  |  1B  |
| ---  | ---  | ---  | ---  | ---  | ---  |
| 0xAA | len  | time | dir  | data | 0x55 |

## How to use 

### device example

This example is based on the demo (zephyr/samples/bluetooth/beacon) using Zepher BT stack with Nordic-pca10056 board.  

1. Add codes in folder 'device' to demo. I put them in 'zephyr/my_components/wiresharkdump/device' and change 'zephyr/bluetooth/beacon/CMakeLists.txt' to the following.

```cmake
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.13.1)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(beacon)

include_directories(
	../../my_components/wiresharkdump/device
)

FILE(GLOB app_sources src/*.c)
target_sources(app PRIVATE 
    ${app_sources}
    ../../my_components/wiresharkdump/device/misc_evt.c
    ../../my_components/wiresharkdump/device/trans-dev.c
    ../../my_components/wiresharkdump/device/wiresharkdump.c
)

target_sources(app PRIVATE src/main.c)
```

2. Patch the 'hci_driver.c.diff' to 'zephyr' repository.

3. Add the following code to 'bluetooth/beacon/src/main.c'.

```c
#include "misc_evt.h"
#include "trans-dev.h"
#include "wiresharkdump.h"

void main(void)
{
	int err;

	struct device *dev;
	misc_evt_init();
	dev = wsk_trans_dev_init("UART_0");
	wsk_dump_init(dev);

	printk("Starting Beacon Demo\n");

	/* Initialize the Bluetooth Subsystem */
	err = bt_enable(bt_ready);
	if (err) {
		printk("Bluetooth init failed (err %d)\n", err);
	}
}
```

4. Config target using menuconfig or other tools.

* Unselect 'Use UART for console' in '(Top) → Device Drivers → Console drivers'.
* Select 'Use RTT console' in '(Top) → Device Drivers → Console drivers'.
* Select 'Enable new asynchronous UART API' in '(Top) → Device Drivers → Serial Drivers'.
* Set 'Size of the minimal libc malloc arena' in '(Top) → C Library'. I set this to '16384'.

5. Compile the target.  

### host

#### Requirement

* python3 (in wsl, you need to use the python installed on Windows as uart can't be found in wsl. )
* pyserial
* wireshark
* pywin32 ( Windows )

#### Cmd

```shell
python serial_trans.py -h   # Use this cmd to get more information.
```

## Repository & Demo

[github.com/novumdun/mcu-datadump-wireshark](https://github.com/novumdun/mcu-datadump-wireshark)

Check out the demo [here](https://youtu.be/AR5nKpMRGrU).
