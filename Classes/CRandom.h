#ifndef RANDOM_H
#define RANDOM_H
#include <math.h>
#define PI 3.14159265358979323846264338327950288419716939937510
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */
class Random
{
public:
	Random();
	Random(unsigned long initKey[], int keyLength);
	~Random();
protected:
	unsigned long mt[N];
	int mti;
	unsigned long genRandINT32();
public:
	static Random* createRandom();
	static Random* createRandom(unsigned long initKey[], int keyLength);
	float getUniformRandom();
	int getUniformRandomAB(int A, int B);
	float getUniformRandomABFloat(float A, float B);
	float getNormalRandom();
	float getNormalRandom(float mean, float sigma);
	float getExponentialRandom();
	float getExponentialRandom(float lambda);
	int getPoissonRandom();
	int getPoissonRandom(float lambda);
public:
	static float randomF;
	static int randomI;
private:
	void initByArray(unsigned long initKye[], int keyLength);
	void initGenRand(unsigned long s);
};
#endif