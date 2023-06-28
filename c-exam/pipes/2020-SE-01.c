foo.
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#define PIPENAME "baz.pipe"

int main(const int argc, const char* const argv[]) {
        if (argc != 2) {
                errx(1,"Usage: %s <filename>", argv[0]);
        }

        struct stat sb;
        int str = stat(PIPENAME, &sb);

        if ((str == -1) && (errno == ENOENT)){
                if (mkfifo(PIPENAME, S_IRUSR | S_IWUSR) == -1) {
                        err(2, "Cannot mkfifo() from %s", argv[0]);
                }

        } else if (str == -1) {
                err(3, "General stat() failure");
        }

        int fd = open(PIPENAME, O_WRONLY);
        if (fd == -1) {
                err(4, "Cannot open pipe %s from %s", PIPENAME, argv[0]);
        }

        if (dup2(fd, 1) == -1) {
                err(5, "Cannot dup fd from %d to 1", fd);
        }

        if (execlp("cat", "cat", argv[1], (char*) NULL) == -1) {
                err(5, "Cannot exec from %s", argv[0]);
        }
        exit(0);
}

bar.c

#include <stdlib.h>
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define PIPENAME "baz.pipe"

int main(const int argc, const char* const argv[]) {
        if (argc != 2) {
                errx(1,"Usage: %s <command>", argv[0]);
        }

    struct stat sb;
    int str = stat(PIPENAME, &sb);

        if ((str == -1) && (errno == ENOENT)){
        if (mkfifo(PIPENAME, S_IRUSR | S_IWUSR) == -1) {
            err(2, "Cannot mkfifo() from %s", argv[0]);
        }

    } else if (str == -1) {
        err(3, "General stat() failure");
    }

        int fd = open(PIPENAME, O_RDONLY);
        if (fd == -1) {
                err(4, "Cannot open pipe %s from %s", PIPENAME, argv[0]);
        }

        if (dup2(fd, 0) == -1) {
                err(5, "Cannot dup fd from %d to 0", fd);
        }

        if (execl(argv[1], argv[1], (char*) NULL) == -1) {
                err(6, "Cannot exec from %s", argv[0]);
        }
        exit(0);
}
