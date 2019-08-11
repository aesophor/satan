# .ycm_extra_conf.py for linux kernel
NAME = 'ycm4linux'

# Config developed with the following setup:
# Target kernel: Linux 4.9.11
# Target arch: x86
# Host distribution: Arch Linux 4.9.11-1-ARCH
# Host gcc: 6.3.1

import os
import subprocess
import glob

# You can set kernel source path by setting KDIR envvar.
# If absent, we'll try to use: /lib/modules/`uname -r`/build
KDIR = os.environ.get('KDIR')
if not KDIR:
    KERNEL_RELEASE = subprocess.check_output(['uname', '-r']).decode('utf-8').strip()
    KDIR = '/lib/modules/' + KERNEL_RELEASE + '/build/'
if not os.path.isdir(KDIR):
    raise Exception(NAME + ': could not find kernel headers at ' + KDIR)

# You can set target arch by setting KARCH envvar.
# If absent, defaults on x86.
# KARCH is used for kernel include paths e.g.: $KDIR/arch/$KARCH/include
KARCH = os.environ.get('KARCH')
if not KARCH:
    KARCH = 'x86'

# Determining GCC path.
GCCPATH = subprocess.check_output(['gcc', '-print-file-name=']).decode('utf-8').strip()
if not os.path.isdir(GCCPATH):
    raise Exception(NAME + ': could not find GCC installation at ' + GCCPATH)

# Flags determined through a verbose build of a simple kernel module.
flags = [
    # Define your per project flags here, flags starting with '.' will be expanded

    # Language flags
    '-xc',
    '-std=gnu89',
    # Defines
    '-D__KERNEL__',
    '-DMODULE',
    '-include', KDIR + '/include/linux/kconfig.h',
    # Include paths
    '-nostdinc',
    '-isystem', GCCPATH + '/include',
    '-I', KDIR + '/arch/' + KARCH + '/include',
    '-I', KDIR + '/arch/' + KARCH + '/include/generated/uapi',
    '-I', KDIR + '/arch/' + KARCH + '/include/generated',
    '-I', KDIR + '/include',
    '-I', KDIR + '/arch/' + KARCH + '/include/uapi',
    '-I', KDIR + '/include/uapi',
    '-I', KDIR + '/include/generated/uapi',
    # Warning flags
    '-Wall',
    '-Wundef',
    '-Wstrict-prototypes',
    '-Wno-trigraphs',
    '-fno-strict-aliasing',
    '-fno-common',
    '-Werror-implicit-function-declaration',
    '-Wno-format-security',
    '-Wdeclaration-after-statement',
]

def getExpandedFlags():
    wd = os.path.dirname(os.path.abspath(__file__))
    ret = []
    for flag in flags:
        if flag.startswith('.'):
            ret.append(os.path.join(wd, flag))
        else:
            ret.append(flag)
    return ret
print(getExpandedFlags())

def FlagsForFile(filename):
    return {
        'flags': getExpandedFlags(),
        'do_cache': True
    }
