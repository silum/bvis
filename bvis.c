#include <err.h>
#include <libgen.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "arg.h"
#include "cmap.h"
#include "hilbert.h"

char *argv0;
const uint32_t *map = Asc;

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
        uint32_t col = map[c];
        printf("%c%c%c", col >> 16 & 0xff, col >> 8 & 0xff, col & 0xff);
    } else {
        printf("%c%c%c", 0, 0, 0);
    }
}

void
usage(void)
{
    const char *prog = basename(argv0);
    printf("Usage: %s [-c<A|a|d|e|m>] [-o<num>] [file]\n", prog);
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
    printf("P6 %d %d 255\n", n, rows);

    for (int b = 0; b < blocks; b++) {
        unsigned char buf[N];
        int len = fread(buf, sizeof *buf, N, fp);
        for (int y = 0; y < n; y++) {
            for (int x = 0; x < n; x++) {
                pchar(buf, len, n, x, y);
            }
        }
    }
}

int
main(int argc, char *argv[])
{
    int order = 2;

    ARGBEGIN() {
    case 'c':
        if (NULL == ARGF()) {
            die("`-%c' expects a colormap selector\n", ARGC()); break;
            /* unreachable */
        } else {
            char *c = ARGF();
            switch (*c) {
            case 'A': map = Asc; break;
            case 'a': map = asc; break;
            case 'd': map = det; break;
            case 'e': map = ent; break;
            case 'm': map = mag; break;
            default: die("invalid colormap selector: `%c'", *c);
            }
        }
        break;
    case 'o':
        order = atoi(EARGF(die("`-%c' expects an order value\n", ARGC())));
        order = (order < 1) ? 1 : order;
        break;
    case 'h':
        /* fall */
    case '?':
        usage();
        /* unreachable */
        break;
    default:
        die("unexpected option: `%c'", ARGC());
        /* unreachable */
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
