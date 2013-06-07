/**
 * @File: serial.h
 * @Author: Joe Shang
 * @Brief: The module of serial communication drivers.
 */

#ifndef _SERIAL_H_
#define _SERIAL_H_

int serial_open_device(char *dev_name);

int serial_set_raw_mode(int fd);
int serial_set_speed(int fd, int speed);
int serial_set_parity(int fd, int data_bits, char parity_type, int stop_bits);

#endif
