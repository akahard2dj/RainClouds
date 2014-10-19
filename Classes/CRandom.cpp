#include "CRandom.h"

float Random::randomF = 0.0f;
int Random::randomI = 0;
Random* Random::createRandom()
{
	Random* p = new Random();
	return p;
}
Random* Random::createRandom(unsigned long initKey[], int keyLength)
{
	Random* p = new Random(initKey, keyLength);
	return p;
}
Random::Random()
{
	mti = N + 1;
	unsigned long init[4] = { 0x123, 0x234, 0x345, 0x456 }, length = 4;
	initByArray(init, length);
}
Random::Random(unsigned long initKey[], int keyLength)
{
	mti = N + 1;
	initByArray(initKey, keyLength);
}
void Random::initByArray(unsigned long initKey[], int keyLength)
{
	int i, j, k;
	initGenRand(19650218UL);
	i = 1; j = 0;
	k = (N>keyLength ? N : keyLength);
	for (; k; k--) {
		mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1664525UL))
			+ initKey[j] + j; /* non linear */
		mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
		i++; j++;
		if (i >= N) { mt[0] = mt[N - 1]; i = 1; }
		if (j >= keyLength) j = 0;
	}
	for (k = N - 1; k; k--) {
		mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1566083941UL))
			- i; /* non linear */
		mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
		i++;
		if (i >= N) { mt[0] = mt[N - 1]; i = 1; }
	}
	mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */
}
unsigned long Random::genRandINT32(void)
{
	unsigned long y;
	static unsigned long mag01[2] = { 0x0UL, MATRIX_A };
	/* mag01[x] = x * MATRIX_A for x=0,1 */
	if (mti >= N) { /* generate N words at one time */
		int kk;
		if (mti == N + 1) /* if init_genrand() has not been called, */
			initGenRand(5489UL); /* a default initial seed is used */
		for (kk = 0; kk<N - M; kk++) {
			y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
			mt[kk] = mt[kk + M] ^ (y >> 1) ^ mag01[y & 0x1UL];
		}
		for (; kk<N - 1; kk++) {
			y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
			mt[kk] = mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
		}
		y = (mt[N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
		mt[N - 1] = mt[M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];
		mti = 0;
	}
	y = mt[mti++];
	/* Tempering */
	y ^= (y >> 11);
	y ^= (y << 7) & 0x9d2c5680UL;
	y ^= (y << 15) & 0xefc60000UL;
	y ^= (y >> 18);
	return y;
}
float Random::getUniformRandom()
{
	randomF = (float)genRandINT32()*(1.0f / 4294967296.0);
	return randomF;
}
int Random::getUniformRandomAB(int A, int B)
{
	randomI = (int)(genRandINT32() >> 1) % ((B + 1) - A) + A;
	return randomI;
}
float Random::getUniformRandomABFloat(float A, float B)
{
	randomF = (((float)genRandINT32()) + 0.5)*(1.0 / 4294967296.0) * (B - A) + A;
	return randomF;
}
float Random::getNormalRandom(float mean, float sigma)
{
	float x = (((float)genRandINT32()) + 0.5)*(1.0 / 4294967296.0);
	float y = (((float)genRandINT32()) + 0.5)*(1.0 / 4294967296.0);
	randomF = sigma * sqrt(-2.0 * log(x)) * cos(2.0*PI*y) + mean;
	return randomF;
}
float Random::getNormalRandom()
{
	float x = (((float)genRandINT32()) + 0.5)*(1.0 / 4294967296.0);
	float y = (((float)genRandINT32()) + 0.5)*(1.0 / 4294967296.0);
	randomF = sqrt(-2.0 * log(x)) * cos(2.0*PI*y);
	return randomF;
}
float Random::getExponentialRandom()
{
	randomF = -log((((float)genRandINT32()) + 0.5)*(1.0 / 4294967296.0));
	return randomF;
}
float Random::getExponentialRandom(float lambda)
{
	randomF = -log((((float)genRandINT32()) + 0.5)*(1.0 / 4294967296.0)) / lambda;
	return randomF;
}
int Random::getPoissonRandom()
{
	float L = exp(-1.0);
	int k = 0;
	float p = 1.0;
	while (p > L) {
		k = k + 1;
		p = p * (((float)genRandINT32()) + 0.5)*(1.0 / 4294967296.0);
	}
	return k - 1;
}
int Random::getPoissonRandom(float lambda)
{
	float L = exp(-lambda);
	int k = 0;
	float p = 1.0;
	while (p > L) {
		k = k + 1;
		p = p * (((float)genRandINT32()) + 0.5)*(1.0 / 4294967296.0);
	}
	return k - 1;
}
void Random::initGenRand(unsigned long s)
{
	mt[0] = s & 0xffffffffUL;
	for (mti = 1; mti<N; mti++) {
		mt[mti] =
			(1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti);
		/* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
		/* In the previous versions, MSBs of the seed affect */
		/* only MSBs of the array mt[]. */
		/* 2002/01/09 modified by Makoto Matsumoto */
		mt[mti] &= 0xffffffffUL;
		/* for >32 bit machines */
	}
}