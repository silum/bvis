#include <err.h>
#include <libgen.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "arg.h"
#include "cmap.h"
#include "hilbert.h"

char *argv0;
const uint32_t *cmap;

void
die(const char *fmt, ...)
{
    va_list ap;

    fprintf(stderr, "%s: ", basename(argv0));
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");
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

uint32_t const *
map(unsigned char index[NUM_CHARS], uint32_t color[NUM_COLORS])
{
    static uint32_t cmap[NUM_CHARS];
    for (unsigned i = 0; i < NUM_CHARS; i++) {
        cmap[i] = color[index[i]];
    }

    return cmap;
}

void
pchar(unsigned char *buf, int len, int n, int x, int y)
{
    int d = xy2d(n, y, x);
    if (d < len) {
        unsigned char c = buf[d];
        uint32_t col = cmap[c];
        printf("%c%c%c", col >> 16 & 0xff, col >> 8 & 0xff, col & 0xff);
    } else {
        printf("%c%c%c", 0, 0, 0);
    }
}

double
shannon(unsigned char *buf, int len)
{
    int histogram[NUM_CHARS] = { 0 };
    for (int i = 0; i < len; i++) {
        int c = buf[i];
        histogram[c] += 1;
    }

    double entropy = 0;
    for (int i = 0; i < NUM_CHARS; i++) {
        if (0 < histogram[i]) {
            double p_x = (double)histogram[i] / len;
            entropy -= p_x * log2(p_x);
        }
    }
    return entropy;
}

void
usage(void)
{
    const char *prog = basename(argv0);
    printf("Usage: %s [-c<A|a|d|e|m|r>] [-e|-E[num]] [-o<num>] [file]\n", prog);
    printf("Usage: %s [-?h]\n", prog);
    exit(EXIT_SUCCESS);
}

void
bvis(FILE *fp, int order, bool do_ent, unsigned entlen)
{
    int n = pow(2, order);
    int N = n * n;
    off_t flen = fsize(fp);
    entlen = (0 == entlen) ? flen : entlen;

    int blen = (do_ent) ? entlen : 1;
    off_t size = flen / blen + (0 != (flen % blen));
    int blocks = size / N + (0 != (size % N));
    blocks = (0 == blocks) ? 1 : blocks;
    int rows = blocks * n;
    printf("P6 %d %d 255\n", n, rows);

    for (int b = 0; b < blocks; b++) {
        unsigned char buf[N];
        int len = 0;
        if (do_ent) {
            for (int i = 0; i < N; i++) {
                unsigned char entbuf[entlen];
                size_t nread = fread(entbuf, sizeof *entbuf, entlen, fp);
                if (0 == nread) {
                    break;
                }
                double ent = shannon(entbuf, nread);
                fprintf(stderr, "%f\n", ent);
                buf[i] = round(ent / 8 * 255);
                len = i + 1;
            }
        } else {
            len = fread(buf, sizeof *buf, N, fp);
        }
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
    bool do_ent = false;
    int entlen = 2048;
    int order = 2;

    ARGBEGIN() {
    case 'c':
        if (NULL == ARGF()) {
            die("`-%c' expects a colormap selector", ARGC()); break;
            /* unreachable */
        } else {
            char *c = ARGF();
            switch (*c) {
            case 'A': cmap = map(asc_index, Asc_color); break;
            case 'a': cmap = map(asc_index, asc_color); break;
            case 'd': cmap = det; break;
            case 'e': cmap = ent; break;
            case 'm': cmap = mag; break;
            case 'r': cmap = map(rad_index, rad_color); break;
            default: die("invalid colormap selector: `%c'", *c);
            }
        }
        break;
    case 'e':
        do_ent = true;
        cmap = ent;
        break;
    case 'E':
        do_ent = true;
        cmap = ent;
        entlen = atoi(EARGF(die("`-%c' expects a block size", ARGC())));
        entlen = (entlen < 0) ? 0 : entlen;
        break;
    case 'o':
        order = atoi(EARGF(die("`-%c' expects an order value", ARGC())));
        order = (order < 0) ? 0 : order;
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
        die("filename expected");
    }

    if (NULL == cmap) {
        cmap = map(asc_index, Asc_color);
    }

    bvis(fp, order, do_ent, entlen);

    argv++;
    if (*argv) {
        die("unexpected argument: %s", *argv);
    }

    return EXIT_SUCCESS;
}
