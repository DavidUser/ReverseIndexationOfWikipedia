unsigned int RSHash(char* str, unsigned int len)
{
   unsigned int b    = 378551;
   unsigned int a    = 63689;
   unsigned int hash = 0;
   unsigned int i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash = hash * a + (*str);
      a    = a * b;
   }

   return hash;
}


unsigned int JSHash(char* str, unsigned int len)
{
   unsigned int hash = 1315423911;
   unsigned int i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash ^= ((hash << 5) + (*str) + (hash >> 2));
   }

   return hash;
}

unsigned int BKDRHash(char* str, unsigned int len)
{
   unsigned int seed = 131; /* 31 131 1313 13131 131313 etc.. */
   unsigned int hash = 0;
   unsigned int i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash = (hash * seed) + (*str);
   }

   return hash;
}

unsigned int DJBHash(char* str, unsigned int len)
{
   unsigned int hash = 5381;
   unsigned int i    = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash = ((hash << 5) + hash) + (*str);
   }

   return hash;
}

unsigned int ELFHash(char* str, unsigned int len)
{
   const unsigned int BitsInUnsignedInt = (unsigned int)(sizeof(unsigned int) * 8);
   const unsigned int ThreeQuarters     = (unsigned int)((BitsInUnsignedInt  * 3) / 4);
   const unsigned int OneEighth         = (unsigned int)(BitsInUnsignedInt / 8);
   const unsigned int HighBits          = (unsigned int)(0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
   unsigned int hash              = 0;
   unsigned int test              = 0;
   unsigned int i                 = 0;

   for(i = 0; i < len; str++, i++)
   {
      hash = (hash << OneEighth) + (*str);

      if((test = hash & HighBits)  != 0)
      {
         hash = (( hash ^ (test >> ThreeQuarters)) & (~HighBits));
      }
   }

   return hash;
}

//Exploit the Central Limit Theorem (``law of large numbers'') and add up several uniformly-distributed random numbers:

#include <stdlib.h>
#include <math.h>

#define NSUM 25

double gaussrand()
{
	double x = 0;
	int i;
	for(i = 0; i < NSUM; i++)
		x += (double)rand() / RAND_MAX;

	x -= NSUM / 2.0;
	x /= sqrt(NSUM / 12.0);

	return x;
}


#include <time.h>
#include <stdlib.h>
#include <stdio.h>

void main(char **argv, int argc) {
	int MAX_LEGTH = 2;
	char key[MAX_LEGTH];
	int EXPERIMENTS = 2000;
	srand(time(0));
	for (int experiment = 1; experiment < EXPERIMENTS; ++experiment) {
		for (int i = 0; i < MAX_LEGTH; ++i) 
			key[i] = (char) rand() % 255;
		;
		printf("RS(%u) = %u;\n", experiment, RSHash(key, MAX_LEGTH));
		printf("JS(%u) = %u;\n", experiment, JSHash(key, MAX_LEGTH));
		printf("BKDR(%u) = %u;\n", experiment, BKDRHash(key, MAX_LEGTH));
		printf("DJB(%u) = %u;\n", experiment, DJBHash(key, MAX_LEGTH));
		printf("ELF(%u) = %u;\n", experiment, ELFHash(key, MAX_LEGTH));
	}
}
