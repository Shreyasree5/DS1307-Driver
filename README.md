#project
DS1307-Driver

#Overview

The DS1307-Driver repository contains code for interfacing with the DS1307 Real-Time Clock (RTC) module using various communication mechanisms. It includes implementations in Python, FIFO-based IPC, message queues, shared memory, and kernel-level drivers.

#Repository Structure

1. ds1307.py

- This Python script is responsible for setting and reading the date and time from the DS1307 RTC module using the I2C interface.
- Establishes connection to the RTC hardware via SMBus.
- Converts decimal values to BCD format before writing to the RTC using write_register.
- Reads the current date and time from the RTC and converts BCD values back to decimal using read_register.

2. FIFO

- This directory contains an implementation of RTC communication using FIFO IPC mechanisms.
- client.c: Requests data from the device via the server using FIFO system calls.
- server.c: Acts as an intermediary, fetching data from the driver and sending it to the client.
- DS1307_drv.c: The driver code for the DS1307 RTC, communicating with the user application through ioctl calls.
- ds1307.h: Header file linking user applications with the driver code.

3. Message Queue & Shared Memory

- Similar to the FIFO implementation, this directory contains code for RTC communication using message queues and shared memory.

4. kernel coding

- Contains kernel-level driver code for DS1307 RTC.
- Implements ioctl calls using copy_from_user and copy_to_user for data transfer between user space and the driver.
- Uses i2c_master_send and i2c_master_recv for communication with the RTC hardware.

#How to Use

1. Python Interface
- Run ds1307.py to read or set the RTC time using I2C.

2. FIFO-Based Communication 
- Compile and run server.c first.
- Then, execute client.c to retrieve RTC data.

3. Message Queue & Shared Memory
- Compile and execute the programs to interact with the RTC using IPC mechanisms.

4. Kernel Driver Implementation
- Compile and load the kernel module.
- Use ioctl system calls to communicate with the DS1307 RTC.

#Dependencies
- Python 3 with smbus library
- GCC for compiling C programs
- Linux system with kernel support for I2C
