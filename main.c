#include <libgen.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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

void
err_operand(char c)
{
    fprintf(stderr, "`%c' expects an operand\n", c);
}

void
usage(int ret)
{
    FILE *fp = (0 == ret) ? stdout : stderr;
    fprintf(fp, "Usage: %s [-o<num>] [file]\n", basename(argv0));
    fprintf(fp, "Usage: %s [-?h]\n", basename(argv0));
    exit(ret);
}

int
main(int argc, char *argv[])
{
    int N, n, o;

    o = 2;
    ARGBEGIN() {
    case 'o':
        o = atoi(EARGF(err_operand(ARGC())));
        o = (o < 1) ? 1 : o;
        break;
    case 'h':
        /* fall */
    case '?':
        usage(EXIT_SUCCESS);
        /* unreached */
        break;
    default:
        die("unexpected option: `%c'", ARGC());
    } ARGEND();
    if (*argv) {
        die("unexpected argument: `%s'", *argv);
    }

    n = pow(2, o);
    N = n * n;

    for (int d = 0; d < N; d++) {
        int x, y;
        d2xy(n, d, &x, &y);
        printf("d: %d -> (%d, %d)\n", d, x, y);
    }

    bool done = false;
    for (int y = 0; !done; y++) {
        for (int x = 0; x < n; x++) {
            if (N <= y * n + x) {
                done = true;
                break;
            }
            int d = xy2d(n, y, x);
            printf(" (%2d, %2d): %4d", y, x, d);
        }
        if (!done) {
            printf("\n");
        }
    }
}
