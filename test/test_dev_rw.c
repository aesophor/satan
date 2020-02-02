#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define DEVICE_NAME "/dev/tty99"

int main(int argc, char* argv[]) {
        int ret = 0;
        int fd = -1;
        char buf[100];

        fd = open(DEVICE_NAME, O_RDWR);
        if (fd == -1) {
                printf("cannot open device.");
                goto end;
        }

        if (argc >= 3 && !strcmp(argv[1], "w")) {
                write(fd, argv[2], strlen(argv[2]));
        } else if (argc >= 2 && !strcmp(argv[1], "r")) {
                read(fd, buf, sizeof(buf));
                printf("received: %s\n", buf);
        } else {
                printf("usage: %s {r,w} <message>\n", argv[0]);
                goto end;
        }

end:
        if (fd != -1) {
                close(DEVICE_NAME);
        }
        return ret;
}
