/*
 * random.c - Copyright © 2011 Szabolcs Nagy
 * Permission to use, copy, modify, and/or distribute this code
 * for any purpose with or without fee is hereby granted.
 * There is no warranty.
*/

#include <stdlib.h>
#include <stdint.h>

/*
this code uses the same lagged fibonacci generator as the
original bsd random implementation except for the seeding

different seeds produce different sequences with long period
(other libcs seed the state with a park-miller generator
when seed=0 some fail to produce good random sequence
others produce the same sequence as another seed)
*/

static uint32_t init[] = {
0x00000000,0x5851f42d,0xc0b18ccf,0xcbb5f646,
0xc7033129,0x30705b04,0x20fd5db4,0x9a8b7f78,
0x502959d8,0xab894868,0x6c0356a7,0x88cdb7ff,
0xb477d43f,0x70a3a52b,0xa8e4baf1,0xfd8341fc,
0x8ae16fd9,0x742d2f7a,0x0d1f0796,0x76035e09,
0x40f7702c,0x6fa72ca5,0xaaa84157,0x58a0df74,
0xc74a0364,0xae533cc4,0x04185faf,0x6de3b115,
0x0cab8628,0xf043bfa4,0x398150e9,0x37521657};

static int n = 31;
static int i = 3;
static int j = 0;
static uint32_t *x = init+1;

static uint32_t lcg31(uint32_t x) {
	return (1103515245*x + 12345) & 0x7fffffff;
}

static uint64_t lcg64(uint64_t x) {
	return 6364136223846793005ull*x + 1;
}

static void *savestate() {
	x[-1] = (n<<16)|(i<<8)|j;
	return x-1;
}

static void loadstate(uint32_t *state) {
	x = state+1;
	n = x[-1]>>16;
	i = (x[-1]>>8)&0xff;
	j = x[-1]&0xff;
}

void srandom(unsigned seed) {
	int k;
	uint64_t s = seed;

	if (n == 0) {
		x[0] = s;
		return;
	}
	i = n == 31 || n == 7 ? 3 : 1;
	j = 0;
	for (k = 0; k < n; k++) {
		s = lcg64(s);
		x[k] = s>>32;
	}
	/* make sure x contains at least one odd number */
	x[0] |= 1;
}

char *initstate(unsigned seed, char *state, size_t size) {
	void *old = savestate();
	if (size < 8)
		return 0;
	else if (size < 32)
		n = 0;
	else if (size < 64)
		n = 7;
	else if (size < 128)
		n = 15;
	else if (size < 256)
		n = 31;
	else
		n = 63;
	x = (uint32_t*)state + 1;
	srandom(seed);
	return old;
}

char *setstate(char *state) {
	void *old = savestate();
	loadstate((uint32_t*)state);
	return old;
}

long random(void) {
	long k;

	if (n == 0)
		return x[0] = lcg31(x[0]);
	x[i] += x[j];
	k = x[i]>>1;
	if (++i == n)
		i = 0;
	if (++j == n)
		j = 0;
	return k;
}
