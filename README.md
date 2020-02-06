<div align="center">
  
<h3>SATANIC ROOTKIT</h3>

<img src="https://github.com/aesophor/satanic-rootkit/blob/master/.meta/banner.jpg">
</div>

## Features
* Local privilege escalation (grant root shell)
* Hide directories or files **by absolute path**
* Hide processes by PID
* ...

## Build Requirements
* gcc
* Linux headers

## Installation
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

## License
Available under [GPL-2.0 License](https://github.com/aesophor/satanic-rootkit/blob/master/LICENSE)
