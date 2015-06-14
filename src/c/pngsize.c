/* Prints dimentions for a given PNG file (e.g., 720x480) */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <err.h>
#define oops(syscall) { printf("error processing %s: ", argv[i]); \
        fflush(0); perror(syscall"()"); continue; }
int main(int argc, char **argv) {
    int fd, i;
    uint32_t h, w;
    if (argc < 2) { printf("%s <pngfile> [pngfile ...]\n", argv[0]); exit(0); }
    for (i = 1; i < argc; i++) {
        if (argc > 2) printf("%s: ", argv[i]);
        if ((fd = open(argv[i], O_RDONLY)) == -1) oops("open");
        if (lseek(fd, 16, SEEK_SET) == -1) oops("lseek");
        if (read(fd, &w, 4) < 1) oops("read");
        if (read(fd, &h, 4) < 1) oops("read");
        printf("%dx%d\n", htonl(w), htonl(h));
        if (close(fd) == -1) oops("close");
    }
    return 0;
}
