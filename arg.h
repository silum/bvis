/*
 * Uses EXIT_FAILURE (from stdlib.h), and NULL (from stddef.h).
 */

/*
 * The following files served as inspiration for the code below:
 *
 * - https://git.suckless.org/st/file/arg.h.html
 * - https://git.suckless.org/farbfeld/file/arg.h.html
 */

#ifndef ARG_H
#define ARG_H

extern char *argv0;

#define ARGBEGIN() \
do { \
	for (argv0 = *argv, argc--, argv++; \
	     NULL != *argv \
	     && '-' == (*argv)[0] \
	     && '\0' != (*argv)[1]; \
	     argc--, argv++) { \
		int i_, used_; \
		if ('-' == (*argv)[1] && '\0' == (*argv)[2]) { \
			argc--, argv++; \
			break; \
		} \
		for (i_ = 1, used_ = 0; \
		     '\0' != (*argv)[i_]; \
		     i_++) { \
			char argc_; \
			argc_ = (*argv)[i_]; \
			switch (argc_)
#define ARGEND() \
			if (used_) { \
				if ((*argv)[i_ + 1]) { \
					break; \
				} else { \
					argc--, argv++; \
					break; \
				} \
			} \
		} \
	} \
} while (0)

#define ARGC()  argc_
#define ARGF_(x) \
  (((*argv)[i_ + 1]) \
   ? (used_ = 1, &((*argv)[i_ + 1])) \
   : (*(argv + 1)) \
     ? (used_ = 1, *(argv + 1)) \
     : (x))
#define EARGF(x)  ARGF_(((x), exit(EXIT_FAILURE), (char *)NULL))
#define ARGF()  ARGF_((char *)NULL)

#endif  /* ARG_H */
