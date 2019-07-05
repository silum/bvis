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
usage(void)
{
    const char *prog = basename(argv0);
    printf("Usage: %s [-o<num>] [file]\n", prog);
    printf("Usage: %s [-?h]\n", prog);
    exit(EXIT_SUCCESS);
}

int
main(int argc, char *argv[])
{
    int N, n, o;

    o = 2;
    ARGBEGIN() {
    case 'o':
        o = atoi(EARGF(die("`-%c' expects an operand\n", ARGC())));
        o = (o < 1) ? 1 : o;
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
    if (*argv) {
        die("unexpected argument: `%s'", *argv);
    }

    n = pow(2, o);
    N = n * n;

    bool done = false;
    for (int y = 0; !done; y++) {
        for (int x = 0; x < n; x++) {
            if (N <= y * n + x) {
                done = true;
                break;
            }
            int d = xy2d(n, y, x);
            printf(" %4d", d);
        }
        if (!done) {
            printf("\n");
        }
    }
}
