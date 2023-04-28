# Tree Network Client

## License

All code is under the GPL-v3

## Abstract

The Tree Network Client is a very simple client for a Tree Network (without loops)

It makes use of a UDP session to connect to the registrar server and TCP sessions to communicate between hosts.

**ATENTION:** This work is far from optimized and includes some bad practices such as the unfreeing of memory and instead passing that job to the OS at the exit of the program. This assignment was more of a proof-of-concept than a programming test.

## Grade
17 out of 20 (with penalization (1) for late delivery)

## Instructions

To compile the code use:

```make clean```
