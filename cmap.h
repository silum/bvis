#ifndef CMAP_H
#define CMAP_H

#define NUM_CHARS  256
#define NUM_COLORS  5

extern uint32_t Asc_color[NUM_COLORS];
extern uint32_t asc_color[NUM_COLORS];
extern unsigned char asc_index[NUM_CHARS];

extern uint32_t det[NUM_CHARS];
extern uint32_t ent[NUM_CHARS];
extern uint32_t mag[NUM_CHARS];

extern uint32_t rad_color[NUM_COLORS];
extern unsigned char rad_index[NUM_CHARS];

#endif  /* CMAP_H */
