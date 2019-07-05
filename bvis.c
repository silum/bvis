#include <err.h>
#include <libgen.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "arg.h"
#include "hilbert.h"

char *argv0;

void
die(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    fprintf(stderr, "%s: ", basename(argv0));
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}

off_t
fsize(FILE *fp)
{
    int fd = fileno(fp);
    struct stat buf;
    fstat(fd, &buf);
    off_t size = buf.st_size;

    return size;
}

void
pchar(unsigned char *buf, int len, int n, int x, int y)
{
    int d = xy2d(n, y, x);
    if (d < len) {
        unsigned char c = buf[d];
        if (0 == c) {  /* 0x00 - black */
            printf("%d %d %d", 0x00, 0x2b, 0x36);
        } else if (c < ' '
                   && !('\t' == c
                        || '\r' == c
                        || '\n' == c)) {  /* low - green */
            printf("%d %d %d", 0x85, 0x99, 0x00);
        } else if (c < 127) {  /* ascii - blue */
            printf("%d %d %d", 0x26, 0x8b, 0xd2);
        } else if (c < 255) {  /* high - red */
            printf("%d %d %d", 0xdc, 0x32, 0x2f);
        } else {  /* 0xff - white */
            printf("%d %d %d", 0xfd, 0xf6, 0xe3);
        }
    } else {
        printf("0 0 0");
    }
}

void
usage(void)
{
    const char *prog = basename(argv0);
    printf("Usage: %s [-o<num>] [file]\n", prog);
    printf("Usage: %s [-?h]\n", prog);
    exit(EXIT_SUCCESS);
}

void
bvis(FILE *fp, int order)
{
    int n = pow(2, order);
    int N = n * n;

    off_t size = fsize(fp);
    int blocks = (size / N) + ((size % N) ? 1 : 0);
    int rows = blocks * n;
    printf("P3 %d %d 255\n", n, rows);

    for (int b = 0; b < blocks; b++) {
        unsigned char buf[N];
        int len = fread(buf, sizeof *buf, N, fp);
        for (int y = 0; y < n; y++) {
            for (int x = 0; x < n; x++) {
                pchar(buf, len, n, x, y);
                printf("%c", (x < n - 1) ? ' ' : '\n');
            }
        }
    }
}

int
main(int argc, char *argv[])
{
    int order = 2;

    ARGBEGIN() {
    case 'o':
        order = atoi(EARGF(die("`-%c' expects an operand\n", ARGC())));
        order = (order < 1) ? 1 : order;
        break;
    case 'h':
        /* fall */
    case '?':
        usage();
        /* unreached */
        break;
    default:
        die("unexpected option: `%c'", ARGC());
    } ARGEND();

    FILE *fp = NULL;
    if (*argv) {
        fp = fopen(*argv, "rb");
        if (NULL == fp) {
            err(EXIT_FAILURE, "cannot open `%s' for reading", *argv);
        }
    } else {
        die("filename expected\n");
    }

    bvis(fp, order);

    argv++;
    if (*argv) {
        die("unexpected argument: %s\n", *argv);
    }

    return EXIT_SUCCESS;
}
