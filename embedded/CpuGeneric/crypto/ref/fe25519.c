#define WINDOWSIZE 1 /* Should be 1,2, or 4 */
#define WINDOWMASK ((1<<WINDOWSIZE)-1)

#include <string.h>
#include "utils.h"
#include "fe25519.h"

static crypto_uint32 equal(crypto_uint32 a,crypto_uint32 b) /* 16-bit inputs */
{
  crypto_uint32 x = a ^ b; /* 0: yes; 1..65535: no */
  x -= 1; /* 4294967295: yes; 0..65534: no */
  x >>= 31; /* 1: yes; 0: no */
  return x;
}

static crypto_uint32 ge(crypto_uint32 a,crypto_uint32 b) /* 16-bit inputs */
{
  unsigned int x = a;
  x -= (unsigned int) b; /* 0..65535: yes; 4294901761..4294967295: no */
  x >>= 31; /* 0: yes; 1: no */
  x ^= 1; /* 1: yes; 0: no */
  return x;
}

static crypto_uint32 times19(crypto_uint32 a)
{
  return (a << 4) + (a << 1) + a;
}

static crypto_uint32 times38(crypto_uint32 a)
{
  return (a << 5) + (a << 2) + (a << 1);
}

static void reduce_add_sub(fe25519 *r)
{
  crypto_uint32 t;
  int i,rep;

  for(rep=0;rep<4;rep++)
  {
    t = r->v[31] >> 7;
    r->v[31] &= 127;
    t = times19(t);
    r->v[0] += t;
    for(i=0;i<31;i++)
    {
      t = r->v[i] >> 8;
      r->v[i+1] += t;
      r->v[i] &= 255;
    }
  }
}

static void reduce_mul(fe25519 *r)
{
  crypto_uint32 t;
  int i,rep;

  for(rep=0;rep<2;rep++)
  {
    t = r->v[31] >> 7;
    r->v[31] &= 127;
    t = times19(t);
    r->v[0] += t;
    for(i=0;i<31;i++)
    {
      t = r->v[i] >> 8;
      r->v[i+1] += t;
      r->v[i] &= 255;
    }
  }
}

/* reduction modulo 2^255-19 */
void fe25519_freeze(fe25519 *r) 
{
  int i;
  crypto_uint32 m = equal(r->v[31],127);
  for(i=30;i>0;i--)
    m &= equal(r->v[i],255);
  m &= ge(r->v[0],237);

  m = -m;

  r->v[31] -= m&127;
  for(i=30;i>0;i--)
    r->v[i] -= m&255;
  r->v[0] -= m&237;
}

void fe25519_unpack(fe25519 *r, const unsigned char x[32])
{
  int i;
  for(i=0;i<32;i++) r->v[i] = x[i];
  r->v[31] &= 127;
}

/* Assumes input x being reduced below 2^255 */
void fe25519_pack(unsigned char r[32], const fe25519 *x)
{
  int i;
  fe25519 y = *x;
  fe25519_freeze(&y);
  for(i=0;i<32;i++) 
    r[i] = y.v[i];
}

int fe25519_iszero(const fe25519 *x)
{
  int i;
  int r;
  fe25519 t = *x;
  fe25519_freeze(&t);
  r = equal(t.v[0],0);
  for(i=1;i<32;i++) 
    r &= equal(t.v[i],0);
  return r;
}

int fe25519_iseq_vartime(const fe25519 *x, const fe25519 *y)
{
  int i;
  fe25519 t1 = *x;
  fe25519 t2 = *y;
  fe25519_freeze(&t1);
  fe25519_freeze(&t2);
  for(i=0;i<32;i++)
    if(t1.v[i] != t2.v[i]) return 0;
  return 1;
}

void fe25519_cmov(fe25519 *r, const fe25519 *x, unsigned char b)
{
  int i;
  crypto_uint32 mask = b;
  mask = -mask;
  for(i=0;i<32;i++) r->v[i] ^= mask & (x->v[i] ^ r->v[i]);
}

unsigned char fe25519_getparity(const fe25519 *x)
{
  fe25519 t = *x;
  fe25519_freeze(&t);
  return t.v[0] & 1;
}

void fe25519_copy(fe25519 *h, const fe25519 *f)
{
  fe25519_new f_n;
  fe25519_new h_n;
  memcpy(&f_n, f, sizeof(fe25519_new));

  int32_t f0 = f_n[0];
  int32_t f1 = f_n[1];
  int32_t f2 = f_n[2];
  int32_t f3 = f_n[3];
  int32_t f4 = f_n[4];
  int32_t f5 = f_n[5];
  int32_t f6 = f_n[6];
  int32_t f7 = f_n[7];
  int32_t f8 = f_n[8];
  int32_t f9 = f_n[9];

  h_n[0] = f0;
  h_n[1] = f1;
  h_n[2] = f2;
  h_n[3] = f3;
  h_n[4] = f4;
  h_n[5] = f5;
  h_n[6] = f6;
  h_n[7] = f7;
  h_n[8] = f8;
  h_n[9] = f9;

  memcpy(&h, h_n, sizeof(fe25519_new));
}

void fe25519_setone(fe25519 *r)
{
  int i;
  r->v[0] = 1;
  for(i=1;i<32;i++) r->v[i]=0;
}

void fe25519_setzero(fe25519 *r)
{
  int i;
  for(i=0;i<32;i++) r->v[i]=0;
}

void fe25519_neg(fe25519 *r, const fe25519 *x)
{
  fe25519 t;
  int i;
  for(i=0;i<32;i++) t.v[i]=x->v[i];
  fe25519_setzero(r);
  fe25519_sub(r, r, &t);
}

void fe25519_add(fe25519 *r, const fe25519 *x, const fe25519 *y)
{
  int i;
  for(i=0;i<32;i++) r->v[i] = x->v[i] + y->v[i];
  reduce_add_sub(r);
}

void fe25519_sub(fe25519 *r, const fe25519 *x, const fe25519 *y)
{
  int i;
  crypto_uint32 t[32];
  t[0] = x->v[0] + 0x1da;
  t[31] = x->v[31] + 0xfe;
  for(i=1;i<31;i++) t[i] = x->v[i] + 0x1fe;
  for(i=0;i<32;i++) r->v[i] = t[i] - y->v[i];
  reduce_add_sub(r);
}

void fe25519_mul(fe25519 *r, const fe25519 *x, const fe25519 *y)
{
  int i,j;
  crypto_uint32 t[63];
  for(i=0;i<63;i++)t[i] = 0;

  for(i=0;i<32;i++)
    for(j=0;j<32;j++)
      t[i+j] += x->v[i] * y->v[j];

  for(i=32;i<63;i++)
    r->v[i-32] = t[i-32] + times38(t[i]); 
  r->v[31] = t[31]; /* result now in r[0]...r[31] */

  reduce_mul(r);
}

void fe25519_square(fe25519 *r, const fe25519 *x)
{
  fe25519_mul(r, x, x);
}

void fe25519_invert(fe25519 *r, const fe25519 *x)
{
	fe25519 z2;
	fe25519 z9;
	fe25519 z11;
	fe25519 z2_5_0;
	fe25519 z2_10_0;
	fe25519 z2_20_0;
	fe25519 z2_50_0;
	fe25519 z2_100_0;
	fe25519 t0;
	fe25519 t1;
	int i;
	
	/* 2 */ fe25519_square(&z2,x);
	/* 4 */ fe25519_square(&t1,&z2);
	/* 8 */ fe25519_square(&t0,&t1);
	/* 9 */ fe25519_mul(&z9,&t0,x);
	/* 11 */ fe25519_mul(&z11,&z9,&z2);
	/* 22 */ fe25519_square(&t0,&z11);
	/* 2^5 - 2^0 = 31 */ fe25519_mul(&z2_5_0,&t0,&z9);

	/* 2^6 - 2^1 */ fe25519_square(&t0,&z2_5_0);
	/* 2^7 - 2^2 */ fe25519_square(&t1,&t0);
	/* 2^8 - 2^3 */ fe25519_square(&t0,&t1);
	/* 2^9 - 2^4 */ fe25519_square(&t1,&t0);
	/* 2^10 - 2^5 */ fe25519_square(&t0,&t1);
	/* 2^10 - 2^0 */ fe25519_mul(&z2_10_0,&t0,&z2_5_0);

	/* 2^11 - 2^1 */ fe25519_square(&t0,&z2_10_0);
	/* 2^12 - 2^2 */ fe25519_square(&t1,&t0);
	/* 2^20 - 2^10 */ for (i = 2;i < 10;i += 2) { fe25519_square(&t0,&t1); fe25519_square(&t1,&t0); }
	/* 2^20 - 2^0 */ fe25519_mul(&z2_20_0,&t1,&z2_10_0);

	/* 2^21 - 2^1 */ fe25519_square(&t0,&z2_20_0);
	/* 2^22 - 2^2 */ fe25519_square(&t1,&t0);
	/* 2^40 - 2^20 */ for (i = 2;i < 20;i += 2) { fe25519_square(&t0,&t1); fe25519_square(&t1,&t0); }
	/* 2^40 - 2^0 */ fe25519_mul(&t0,&t1,&z2_20_0);

	/* 2^41 - 2^1 */ fe25519_square(&t1,&t0);
	/* 2^42 - 2^2 */ fe25519_square(&t0,&t1);
	/* 2^50 - 2^10 */ for (i = 2;i < 10;i += 2) { fe25519_square(&t1,&t0); fe25519_square(&t0,&t1); }
	/* 2^50 - 2^0 */ fe25519_mul(&z2_50_0,&t0,&z2_10_0);

	/* 2^51 - 2^1 */ fe25519_square(&t0,&z2_50_0);
	/* 2^52 - 2^2 */ fe25519_square(&t1,&t0);
	/* 2^100 - 2^50 */ for (i = 2;i < 50;i += 2) { fe25519_square(&t0,&t1); fe25519_square(&t1,&t0); }
	/* 2^100 - 2^0 */ fe25519_mul(&z2_100_0,&t1,&z2_50_0);

	/* 2^101 - 2^1 */ fe25519_square(&t1,&z2_100_0);
	/* 2^102 - 2^2 */ fe25519_square(&t0,&t1);
	/* 2^200 - 2^100 */ for (i = 2;i < 100;i += 2) { fe25519_square(&t1,&t0); fe25519_square(&t0,&t1); }
	/* 2^200 - 2^0 */ fe25519_mul(&t1,&t0,&z2_100_0);

	/* 2^201 - 2^1 */ fe25519_square(&t0,&t1);
	/* 2^202 - 2^2 */ fe25519_square(&t1,&t0);
	/* 2^250 - 2^50 */ for (i = 2;i < 50;i += 2) { fe25519_square(&t0,&t1); fe25519_square(&t1,&t0); }
	/* 2^250 - 2^0 */ fe25519_mul(&t0,&t1,&z2_50_0);

	/* 2^251 - 2^1 */ fe25519_square(&t1,&t0);
	/* 2^252 - 2^2 */ fe25519_square(&t0,&t1);
	/* 2^253 - 2^3 */ fe25519_square(&t1,&t0);
	/* 2^254 - 2^4 */ fe25519_square(&t0,&t1);
	/* 2^255 - 2^5 */ fe25519_square(&t1,&t0);
	/* 2^255 - 21 */ fe25519_mul(r,&t1,&z11);
}

void fe25519_pow2523(fe25519 *r, const fe25519 *x)
{
	fe25519 z2;
	fe25519 z9;
	fe25519 z11;
	fe25519 z2_5_0;
	fe25519 z2_10_0;
	fe25519 z2_20_0;
	fe25519 z2_50_0;
	fe25519 z2_100_0;
	fe25519 t;
	int i;
		
	/* 2 */ fe25519_square(&z2,x);
	/* 4 */ fe25519_square(&t,&z2);
	/* 8 */ fe25519_square(&t,&t);
	/* 9 */ fe25519_mul(&z9,&t,x);
	/* 11 */ fe25519_mul(&z11,&z9,&z2);
	/* 22 */ fe25519_square(&t,&z11);
	/* 2^5 - 2^0 = 31 */ fe25519_mul(&z2_5_0,&t,&z9);

	/* 2^6 - 2^1 */ fe25519_square(&t,&z2_5_0);
	/* 2^10 - 2^5 */ for (i = 1;i < 5;i++) { fe25519_square(&t,&t); }
	/* 2^10 - 2^0 */ fe25519_mul(&z2_10_0,&t,&z2_5_0);

	/* 2^11 - 2^1 */ fe25519_square(&t,&z2_10_0);
	/* 2^20 - 2^10 */ for (i = 1;i < 10;i++) { fe25519_square(&t,&t); }
	/* 2^20 - 2^0 */ fe25519_mul(&z2_20_0,&t,&z2_10_0);

	/* 2^21 - 2^1 */ fe25519_square(&t,&z2_20_0);
	/* 2^40 - 2^20 */ for (i = 1;i < 20;i++) { fe25519_square(&t,&t); }
	/* 2^40 - 2^0 */ fe25519_mul(&t,&t,&z2_20_0);

	/* 2^41 - 2^1 */ fe25519_square(&t,&t);
	/* 2^50 - 2^10 */ for (i = 1;i < 10;i++) { fe25519_square(&t,&t); }
	/* 2^50 - 2^0 */ fe25519_mul(&z2_50_0,&t,&z2_10_0);

	/* 2^51 - 2^1 */ fe25519_square(&t,&z2_50_0);
	/* 2^100 - 2^50 */ for (i = 1;i < 50;i++) { fe25519_square(&t,&t); }
	/* 2^100 - 2^0 */ fe25519_mul(&z2_100_0,&t,&z2_50_0);

	/* 2^101 - 2^1 */ fe25519_square(&t,&z2_100_0);
	/* 2^200 - 2^100 */ for (i = 1;i < 100;i++) { fe25519_square(&t,&t); }
	/* 2^200 - 2^0 */ fe25519_mul(&t,&t,&z2_100_0);

	/* 2^201 - 2^1 */ fe25519_square(&t,&t);
	/* 2^250 - 2^50 */ for (i = 1;i < 50;i++) { fe25519_square(&t,&t); }
	/* 2^250 - 2^0 */ fe25519_mul(&t,&t,&z2_50_0);

	/* 2^251 - 2^1 */ fe25519_square(&t,&t);
	/* 2^252 - 2^2 */ fe25519_square(&t,&t);
	/* 2^252 - 3 */ fe25519_mul(r,&t,x);
}

void fe25519_sq(fe25519 *h, const fe25519 *f)
{
  fe25519_new f_n;
  fe25519_new h_n;
  memcpy(&f_n, f, sizeof(fe25519_new));

  int32_t f0 = f_n[0];
  int32_t f1 = f_n[1];
  int32_t f2 = f_n[2];
  int32_t f3 = f_n[3];
  int32_t f4 = f_n[4];
  int32_t f5 = f_n[5];
  int32_t f6 = f_n[6];
  int32_t f7 = f_n[7];
  int32_t f8 = f_n[8];
  int32_t f9 = f_n[9];

  int32_t f0_2  = 2 * f0;
  int32_t f1_2  = 2 * f1;
  int32_t f2_2  = 2 * f2;
  int32_t f3_2  = 2 * f3;
  int32_t f4_2  = 2 * f4;
  int32_t f5_2  = 2 * f5;
  int32_t f6_2  = 2 * f6;
  int32_t f7_2  = 2 * f7;
  int32_t f5_38 = 38 * f5; /* 1.959375*2^30 */
  int32_t f6_19 = 19 * f6; /* 1.959375*2^30 */
  int32_t f7_38 = 38 * f7; /* 1.959375*2^30 */
  int32_t f8_19 = 19 * f8; /* 1.959375*2^30 */
  int32_t f9_38 = 38 * f9; /* 1.959375*2^30 */

  int64_t f0f0    = f0 * (int64_t) f0;
  int64_t f0f1_2  = f0_2 * (int64_t) f1;
  int64_t f0f2_2  = f0_2 * (int64_t) f2;
  int64_t f0f3_2  = f0_2 * (int64_t) f3;
  int64_t f0f4_2  = f0_2 * (int64_t) f4;
  int64_t f0f5_2  = f0_2 * (int64_t) f5;
  int64_t f0f6_2  = f0_2 * (int64_t) f6;
  int64_t f0f7_2  = f0_2 * (int64_t) f7;
  int64_t f0f8_2  = f0_2 * (int64_t) f8;
  int64_t f0f9_2  = f0_2 * (int64_t) f9;
  int64_t f1f1_2  = f1_2 * (int64_t) f1;
  int64_t f1f2_2  = f1_2 * (int64_t) f2;
  int64_t f1f3_4  = f1_2 * (int64_t) f3_2;
  int64_t f1f4_2  = f1_2 * (int64_t) f4;
  int64_t f1f5_4  = f1_2 * (int64_t) f5_2;
  int64_t f1f6_2  = f1_2 * (int64_t) f6;
  int64_t f1f7_4  = f1_2 * (int64_t) f7_2;
  int64_t f1f8_2  = f1_2 * (int64_t) f8;
  int64_t f1f9_76 = f1_2 * (int64_t) f9_38;
  int64_t f2f2    = f2 * (int64_t) f2;
  int64_t f2f3_2  = f2_2 * (int64_t) f3;
  int64_t f2f4_2  = f2_2 * (int64_t) f4;
  int64_t f2f5_2  = f2_2 * (int64_t) f5;
  int64_t f2f6_2  = f2_2 * (int64_t) f6;
  int64_t f2f7_2  = f2_2 * (int64_t) f7;
  int64_t f2f8_38 = f2_2 * (int64_t) f8_19;
  int64_t f2f9_38 = f2 * (int64_t) f9_38;
  int64_t f3f3_2  = f3_2 * (int64_t) f3;
  int64_t f3f4_2  = f3_2 * (int64_t) f4;
  int64_t f3f5_4  = f3_2 * (int64_t) f5_2;
  int64_t f3f6_2  = f3_2 * (int64_t) f6;
  int64_t f3f7_76 = f3_2 * (int64_t) f7_38;
  int64_t f3f8_38 = f3_2 * (int64_t) f8_19;
  int64_t f3f9_76 = f3_2 * (int64_t) f9_38;
  int64_t f4f4    = f4 * (int64_t) f4;
  int64_t f4f5_2  = f4_2 * (int64_t) f5;
  int64_t f4f6_38 = f4_2 * (int64_t) f6_19;
  int64_t f4f7_38 = f4 * (int64_t) f7_38;
  int64_t f4f8_38 = f4_2 * (int64_t) f8_19;
  int64_t f4f9_38 = f4 * (int64_t) f9_38;
  int64_t f5f5_38 = f5 * (int64_t) f5_38;
  int64_t f5f6_38 = f5_2 * (int64_t) f6_19;
  int64_t f5f7_76 = f5_2 * (int64_t) f7_38;
  int64_t f5f8_38 = f5_2 * (int64_t) f8_19;
  int64_t f5f9_76 = f5_2 * (int64_t) f9_38;
  int64_t f6f6_19 = f6 * (int64_t) f6_19;
  int64_t f6f7_38 = f6 * (int64_t) f7_38;
  int64_t f6f8_38 = f6_2 * (int64_t) f8_19;
  int64_t f6f9_38 = f6 * (int64_t) f9_38;
  int64_t f7f7_38 = f7 * (int64_t) f7_38;
  int64_t f7f8_38 = f7_2 * (int64_t) f8_19;
  int64_t f7f9_76 = f7_2 * (int64_t) f9_38;
  int64_t f8f8_19 = f8 * (int64_t) f8_19;
  int64_t f8f9_38 = f8 * (int64_t) f9_38;
  int64_t f9f9_38 = f9 * (int64_t) f9_38;

  int64_t h0 = f0f0 + f1f9_76 + f2f8_38 + f3f7_76 + f4f6_38 + f5f5_38;
  int64_t h1 = f0f1_2 + f2f9_38 + f3f8_38 + f4f7_38 + f5f6_38;
  int64_t h2 = f0f2_2 + f1f1_2 + f3f9_76 + f4f8_38 + f5f7_76 + f6f6_19;
  int64_t h3 = f0f3_2 + f1f2_2 + f4f9_38 + f5f8_38 + f6f7_38;
  int64_t h4 = f0f4_2 + f1f3_4 + f2f2 + f5f9_76 + f6f8_38 + f7f7_38;
  int64_t h5 = f0f5_2 + f1f4_2 + f2f3_2 + f6f9_38 + f7f8_38;
  int64_t h6 = f0f6_2 + f1f5_4 + f2f4_2 + f3f3_2 + f7f9_76 + f8f8_19;
  int64_t h7 = f0f7_2 + f1f6_2 + f2f5_2 + f3f4_2 + f8f9_38;
  int64_t h8 = f0f8_2 + f1f7_4 + f2f6_2 + f3f5_4 + f4f4 + f9f9_38;
  int64_t h9 = f0f9_2 + f1f8_2 + f2f7_2 + f3f6_2 + f4f5_2;

  int64_t carry0;
  int64_t carry1;
  int64_t carry2;
  int64_t carry3;
  int64_t carry4;
  int64_t carry5;
  int64_t carry6;
  int64_t carry7;
  int64_t carry8;
  int64_t carry9;

  carry0 = (h0 + (int64_t)(1L << 25)) >> 26;
  h1 += carry0;
  h0 -= carry0 * ((uint64_t) 1L << 26);
  carry4 = (h4 + (int64_t)(1L << 25)) >> 26;
  h5 += carry4;
  h4 -= carry4 * ((uint64_t) 1L << 26);

  carry1 = (h1 + (int64_t)(1L << 24)) >> 25;
  h2 += carry1;
  h1 -= carry1 * ((uint64_t) 1L << 25);
  carry5 = (h5 + (int64_t)(1L << 24)) >> 25;
  h6 += carry5;
  h5 -= carry5 * ((uint64_t) 1L << 25);

  carry2 = (h2 + (int64_t)(1L << 25)) >> 26;
  h3 += carry2;
  h2 -= carry2 * ((uint64_t) 1L << 26);
  carry6 = (h6 + (int64_t)(1L << 25)) >> 26;
  h7 += carry6;
  h6 -= carry6 * ((uint64_t) 1L << 26);

  carry3 = (h3 + (int64_t)(1L << 24)) >> 25;
  h4 += carry3;
  h3 -= carry3 * ((uint64_t) 1L << 25);
  carry7 = (h7 + (int64_t)(1L << 24)) >> 25;
  h8 += carry7;
  h7 -= carry7 * ((uint64_t) 1L << 25);

  carry4 = (h4 + (int64_t)(1L << 25)) >> 26;
  h5 += carry4;
  h4 -= carry4 * ((uint64_t) 1L << 26);
  carry8 = (h8 + (int64_t)(1L << 25)) >> 26;
  h9 += carry8;
  h8 -= carry8 * ((uint64_t) 1L << 26);

  carry9 = (h9 + (int64_t)(1L << 24)) >> 25;
  h0 += carry9 * 19;
  h9 -= carry9 * ((uint64_t) 1L << 25);

  carry0 = (h0 + (int64_t)(1L << 25)) >> 26;
  h1 += carry0;
  h0 -= carry0 * ((uint64_t) 1L << 26);

  h_n[0] = (int32_t) h0;
  h_n[1] = (int32_t) h1;
  h_n[2] = (int32_t) h2;
  h_n[3] = (int32_t) h3;
  h_n[4] = (int32_t) h4;
  h_n[5] = (int32_t) h5;
  h_n[6] = (int32_t) h6;
  h_n[7] = (int32_t) h7;
  h_n[8] = (int32_t) h8;
  h_n[9] = (int32_t) h9;

  memcpy(&h, h_n, sizeof(fe25519_new));
}

void fe25519_sq2(fe25519 *h, const fe25519 *f)
{
  fe25519_new f_n;
  fe25519_new h_n;
  memcpy(&f_n, f, sizeof(fe25519_new));

  int32_t f0 = f_n[0];
  int32_t f1 = f_n[1];
  int32_t f2 = f_n[2];
  int32_t f3 = f_n[3];
  int32_t f4 = f_n[4];
  int32_t f5 = f_n[5];
  int32_t f6 = f_n[6];
  int32_t f7 = f_n[7];
  int32_t f8 = f_n[8];
  int32_t f9 = f_n[9];

  int32_t f0_2  = 2 * f0;
  int32_t f1_2  = 2 * f1;
  int32_t f2_2  = 2 * f2;
  int32_t f3_2  = 2 * f3;
  int32_t f4_2  = 2 * f4;
  int32_t f5_2  = 2 * f5;
  int32_t f6_2  = 2 * f6;
  int32_t f7_2  = 2 * f7;
  int32_t f5_38 = 38 * f5; /* 1.959375*2^30 */
  int32_t f6_19 = 19 * f6; /* 1.959375*2^30 */
  int32_t f7_38 = 38 * f7; /* 1.959375*2^30 */
  int32_t f8_19 = 19 * f8; /* 1.959375*2^30 */
  int32_t f9_38 = 38 * f9; /* 1.959375*2^30 */

  int64_t f0f0    = f0 * (int64_t) f0;
  int64_t f0f1_2  = f0_2 * (int64_t) f1;
  int64_t f0f2_2  = f0_2 * (int64_t) f2;
  int64_t f0f3_2  = f0_2 * (int64_t) f3;
  int64_t f0f4_2  = f0_2 * (int64_t) f4;
  int64_t f0f5_2  = f0_2 * (int64_t) f5;
  int64_t f0f6_2  = f0_2 * (int64_t) f6;
  int64_t f0f7_2  = f0_2 * (int64_t) f7;
  int64_t f0f8_2  = f0_2 * (int64_t) f8;
  int64_t f0f9_2  = f0_2 * (int64_t) f9;
  int64_t f1f1_2  = f1_2 * (int64_t) f1;
  int64_t f1f2_2  = f1_2 * (int64_t) f2;
  int64_t f1f3_4  = f1_2 * (int64_t) f3_2;
  int64_t f1f4_2  = f1_2 * (int64_t) f4;
  int64_t f1f5_4  = f1_2 * (int64_t) f5_2;
  int64_t f1f6_2  = f1_2 * (int64_t) f6;
  int64_t f1f7_4  = f1_2 * (int64_t) f7_2;
  int64_t f1f8_2  = f1_2 * (int64_t) f8;
  int64_t f1f9_76 = f1_2 * (int64_t) f9_38;
  int64_t f2f2    = f2 * (int64_t) f2;
  int64_t f2f3_2  = f2_2 * (int64_t) f3;
  int64_t f2f4_2  = f2_2 * (int64_t) f4;
  int64_t f2f5_2  = f2_2 * (int64_t) f5;
  int64_t f2f6_2  = f2_2 * (int64_t) f6;
  int64_t f2f7_2  = f2_2 * (int64_t) f7;
  int64_t f2f8_38 = f2_2 * (int64_t) f8_19;
  int64_t f2f9_38 = f2 * (int64_t) f9_38;
  int64_t f3f3_2  = f3_2 * (int64_t) f3;
  int64_t f3f4_2  = f3_2 * (int64_t) f4;
  int64_t f3f5_4  = f3_2 * (int64_t) f5_2;
  int64_t f3f6_2  = f3_2 * (int64_t) f6;
  int64_t f3f7_76 = f3_2 * (int64_t) f7_38;
  int64_t f3f8_38 = f3_2 * (int64_t) f8_19;
  int64_t f3f9_76 = f3_2 * (int64_t) f9_38;
  int64_t f4f4    = f4 * (int64_t) f4;
  int64_t f4f5_2  = f4_2 * (int64_t) f5;
  int64_t f4f6_38 = f4_2 * (int64_t) f6_19;
  int64_t f4f7_38 = f4 * (int64_t) f7_38;
  int64_t f4f8_38 = f4_2 * (int64_t) f8_19;
  int64_t f4f9_38 = f4 * (int64_t) f9_38;
  int64_t f5f5_38 = f5 * (int64_t) f5_38;
  int64_t f5f6_38 = f5_2 * (int64_t) f6_19;
  int64_t f5f7_76 = f5_2 * (int64_t) f7_38;
  int64_t f5f8_38 = f5_2 * (int64_t) f8_19;
  int64_t f5f9_76 = f5_2 * (int64_t) f9_38;
  int64_t f6f6_19 = f6 * (int64_t) f6_19;
  int64_t f6f7_38 = f6 * (int64_t) f7_38;
  int64_t f6f8_38 = f6_2 * (int64_t) f8_19;
  int64_t f6f9_38 = f6 * (int64_t) f9_38;
  int64_t f7f7_38 = f7 * (int64_t) f7_38;
  int64_t f7f8_38 = f7_2 * (int64_t) f8_19;
  int64_t f7f9_76 = f7_2 * (int64_t) f9_38;
  int64_t f8f8_19 = f8 * (int64_t) f8_19;
  int64_t f8f9_38 = f8 * (int64_t) f9_38;
  int64_t f9f9_38 = f9 * (int64_t) f9_38;

  int64_t h0 = f0f0 + f1f9_76 + f2f8_38 + f3f7_76 + f4f6_38 + f5f5_38;
  int64_t h1 = f0f1_2 + f2f9_38 + f3f8_38 + f4f7_38 + f5f6_38;
  int64_t h2 = f0f2_2 + f1f1_2 + f3f9_76 + f4f8_38 + f5f7_76 + f6f6_19;
  int64_t h3 = f0f3_2 + f1f2_2 + f4f9_38 + f5f8_38 + f6f7_38;
  int64_t h4 = f0f4_2 + f1f3_4 + f2f2 + f5f9_76 + f6f8_38 + f7f7_38;
  int64_t h5 = f0f5_2 + f1f4_2 + f2f3_2 + f6f9_38 + f7f8_38;
  int64_t h6 = f0f6_2 + f1f5_4 + f2f4_2 + f3f3_2 + f7f9_76 + f8f8_19;
  int64_t h7 = f0f7_2 + f1f6_2 + f2f5_2 + f3f4_2 + f8f9_38;
  int64_t h8 = f0f8_2 + f1f7_4 + f2f6_2 + f3f5_4 + f4f4 + f9f9_38;
  int64_t h9 = f0f9_2 + f1f8_2 + f2f7_2 + f3f6_2 + f4f5_2;

  int64_t carry0;
  int64_t carry1;
  int64_t carry2;
  int64_t carry3;
  int64_t carry4;
  int64_t carry5;
  int64_t carry6;
  int64_t carry7;
  int64_t carry8;
  int64_t carry9;

  h0 += h0;
  h1 += h1;
  h2 += h2;
  h3 += h3;
  h4 += h4;
  h5 += h5;
  h6 += h6;
  h7 += h7;
  h8 += h8;
  h9 += h9;

  carry0 = (h0 + (int64_t)(1L << 25)) >> 26;
  h1 += carry0;
  h0 -= carry0 * ((uint64_t) 1L << 26);
  carry4 = (h4 + (int64_t)(1L << 25)) >> 26;
  h5 += carry4;
  h4 -= carry4 * ((uint64_t) 1L << 26);

  carry1 = (h1 + (int64_t)(1L << 24)) >> 25;
  h2 += carry1;
  h1 -= carry1 * ((uint64_t) 1L << 25);
  carry5 = (h5 + (int64_t)(1L << 24)) >> 25;
  h6 += carry5;
  h5 -= carry5 * ((uint64_t) 1L << 25);

  carry2 = (h2 + (int64_t)(1L << 25)) >> 26;
  h3 += carry2;
  h2 -= carry2 * ((uint64_t) 1L << 26);
  carry6 = (h6 + (int64_t)(1L << 25)) >> 26;
  h7 += carry6;
  h6 -= carry6 * ((uint64_t) 1L << 26);

  carry3 = (h3 + (int64_t)(1L << 24)) >> 25;
  h4 += carry3;
  h3 -= carry3 * ((uint64_t) 1L << 25);
  carry7 = (h7 + (int64_t)(1L << 24)) >> 25;
  h8 += carry7;
  h7 -= carry7 * ((uint64_t) 1L << 25);

  carry4 = (h4 + (int64_t)(1L << 25)) >> 26;
  h5 += carry4;
  h4 -= carry4 * ((uint64_t) 1L << 26);
  carry8 = (h8 + (int64_t)(1L << 25)) >> 26;
  h9 += carry8;
  h8 -= carry8 * ((uint64_t) 1L << 26);

  carry9 = (h9 + (int64_t)(1L << 24)) >> 25;
  h0 += carry9 * 19;
  h9 -= carry9 * ((uint64_t) 1L << 25);

  carry0 = (h0 + (int64_t)(1L << 25)) >> 26;
  h1 += carry0;
  h0 -= carry0 * ((uint64_t) 1L << 26);

  h_n[0] = (int32_t) h0;
  h_n[1] = (int32_t) h1;
  h_n[2] = (int32_t) h2;
  h_n[3] = (int32_t) h3;
  h_n[4] = (int32_t) h4;
  h_n[5] = (int32_t) h5;
  h_n[6] = (int32_t) h6;
  h_n[7] = (int32_t) h7;
  h_n[8] = (int32_t) h8;
  h_n[9] = (int32_t) h9;

  memcpy(&h, h_n, sizeof(fe25519_new));
}

void fe25519_0(fe25519 *h)
{
  fe25519_new h_n;

  memset(&h_n[0], 0, 10 * sizeof h_n[0]);

  memcpy(&h_n, h, sizeof(fe25519_new));
}

void fe25519_1(fe25519 *h)
{
  fe25519_new h_n;

  h_n[0] = 1;
  h_n[1] = 0;
  memset(&h_n[2], 0, 8 * sizeof h_n[0]);

  memcpy(&h_n, h, sizeof(fe25519_new));
}

void fe25519_frombytes(fe25519 *h, const unsigned char *s)
{
  fe25519_new h_n;

  int64_t h0 = load_4(s);
  int64_t h1 = load_3(s + 4) << 6;
  int64_t h2 = load_3(s + 7) << 5;
  int64_t h3 = load_3(s + 10) << 3;
  int64_t h4 = load_3(s + 13) << 2;
  int64_t h5 = load_4(s + 16);
  int64_t h6 = load_3(s + 20) << 7;
  int64_t h7 = load_3(s + 23) << 5;
  int64_t h8 = load_3(s + 26) << 4;
  int64_t h9 = (load_3(s + 29) & 8388607) << 2;

  int64_t carry0;
  int64_t carry1;
  int64_t carry2;
  int64_t carry3;
  int64_t carry4;
  int64_t carry5;
  int64_t carry6;
  int64_t carry7;
  int64_t carry8;
  int64_t carry9;

  carry9 = (h9 + (int64_t)(1L << 24)) >> 25;
  h0 += carry9 * 19;
  h9 -= carry9 * ((uint64_t) 1L << 25);
  carry1 = (h1 + (int64_t)(1L << 24)) >> 25;
  h2 += carry1;
  h1 -= carry1 * ((uint64_t) 1L << 25);
  carry3 = (h3 + (int64_t)(1L << 24)) >> 25;
  h4 += carry3;
  h3 -= carry3 * ((uint64_t) 1L << 25);
  carry5 = (h5 + (int64_t)(1L << 24)) >> 25;
  h6 += carry5;
  h5 -= carry5 * ((uint64_t) 1L << 25);
  carry7 = (h7 + (int64_t)(1L << 24)) >> 25;
  h8 += carry7;
  h7 -= carry7 * ((uint64_t) 1L << 25);

  carry0 = (h0 + (int64_t)(1L << 25)) >> 26;
  h1 += carry0;
  h0 -= carry0 * ((uint64_t) 1L << 26);
  carry2 = (h2 + (int64_t)(1L << 25)) >> 26;
  h3 += carry2;
  h2 -= carry2 * ((uint64_t) 1L << 26);
  carry4 = (h4 + (int64_t)(1L << 25)) >> 26;
  h5 += carry4;
  h4 -= carry4 * ((uint64_t) 1L << 26);
  carry6 = (h6 + (int64_t)(1L << 25)) >> 26;
  h7 += carry6;
  h6 -= carry6 * ((uint64_t) 1L << 26);
  carry8 = (h8 + (int64_t)(1L << 25)) >> 26;
  h9 += carry8;
  h8 -= carry8 * ((uint64_t) 1L << 26);

  h_n[0] = (int32_t) h0;
  h_n[1] = (int32_t) h1;
  h_n[2] = (int32_t) h2;
  h_n[3] = (int32_t) h3;
  h_n[4] = (int32_t) h4;
  h_n[5] = (int32_t) h5;
  h_n[6] = (int32_t) h6;
  h_n[7] = (int32_t) h7;
  h_n[8] = (int32_t) h8;
  h_n[9] = (int32_t) h9;

  memcpy(h, &h_n, sizeof(fe25519_new));
}

static void fe25519_reduce(fe25519_new h, const fe25519_new f)
{
  int32_t h0 = f[0];
  int32_t h1 = f[1];
  int32_t h2 = f[2];
  int32_t h3 = f[3];
  int32_t h4 = f[4];
  int32_t h5 = f[5];
  int32_t h6 = f[6];
  int32_t h7 = f[7];
  int32_t h8 = f[8];
  int32_t h9 = f[9];

  int32_t q;
  int32_t carry0, carry1, carry2, carry3, carry4, carry5, carry6, carry7, carry8, carry9;

  q = (19 * h9 + ((uint32_t) 1L << 24)) >> 25;
  q = (h0 + q) >> 26;
  q = (h1 + q) >> 25;
  q = (h2 + q) >> 26;
  q = (h3 + q) >> 25;
  q = (h4 + q) >> 26;
  q = (h5 + q) >> 25;
  q = (h6 + q) >> 26;
  q = (h7 + q) >> 25;
  q = (h8 + q) >> 26;
  q = (h9 + q) >> 25;

  /* Goal: Output h-(2^255-19)q, which is between 0 and 2^255-20. */
  h0 += 19 * q;
  /* Goal: Output h-2^255 q, which is between 0 and 2^255-20. */

  carry0 = h0 >> 26;
  h1 += carry0;
  h0 -= carry0 * ((uint32_t) 1L << 26);
  carry1 = h1 >> 25;
  h2 += carry1;
  h1 -= carry1 * ((uint32_t) 1L << 25);
  carry2 = h2 >> 26;
  h3 += carry2;
  h2 -= carry2 * ((uint32_t) 1L << 26);
  carry3 = h3 >> 25;
  h4 += carry3;
  h3 -= carry3 * ((uint32_t) 1L << 25);
  carry4 = h4 >> 26;
  h5 += carry4;
  h4 -= carry4 * ((uint32_t) 1L << 26);
  carry5 = h5 >> 25;
  h6 += carry5;
  h5 -= carry5 * ((uint32_t) 1L << 25);
  carry6 = h6 >> 26;
  h7 += carry6;
  h6 -= carry6 * ((uint32_t) 1L << 26);
  carry7 = h7 >> 25;
  h8 += carry7;
  h7 -= carry7 * ((uint32_t) 1L << 25);
  carry8 = h8 >> 26;
  h9 += carry8;
  h8 -= carry8 * ((uint32_t) 1L << 26);
  carry9 = h9 >> 25;
  h9 -= carry9 * ((uint32_t) 1L << 25);

  h[0] = h0;
  h[1] = h1;
  h[2] = h2;
  h[3] = h3;
  h[4] = h4;
  h[5] = h5;
  h[6] = h6;
  h[7] = h7;
  h[8] = h8;
  h[9] = h9;
}

void fe25519_tobytes(unsigned char *s, const fe25519 *h)
{
  fe25519_new t;
  fe25519_new h_n;

  memcpy(&h_n, h, sizeof(fe25519_new));

  fe25519_reduce(t, h_n);
  s[0]  = t[0] >> 0;
  s[1]  = t[0] >> 8;
  s[2]  = t[0] >> 16;
  s[3]  = (t[0] >> 24) | (t[1] * ((uint32_t) 1 << 2));
  s[4]  = t[1] >> 6;
  s[5]  = t[1] >> 14;
  s[6]  = (t[1] >> 22) | (t[2] * ((uint32_t) 1 << 3));
  s[7]  = t[2] >> 5;
  s[8]  = t[2] >> 13;
  s[9]  = (t[2] >> 21) | (t[3] * ((uint32_t) 1 << 5));
  s[10] = t[3] >> 3;
  s[11] = t[3] >> 11;
  s[12] = (t[3] >> 19) | (t[4] * ((uint32_t) 1 << 6));
  s[13] = t[4] >> 2;
  s[14] = t[4] >> 10;
  s[15] = t[4] >> 18;
  s[16] = t[5] >> 0;
  s[17] = t[5] >> 8;
  s[18] = t[5] >> 16;
  s[19] = (t[5] >> 24) | (t[6] * ((uint32_t) 1 << 1));
  s[20] = t[6] >> 7;
  s[21] = t[6] >> 15;
  s[22] = (t[6] >> 23) | (t[7] * ((uint32_t) 1 << 3));
  s[23] = t[7] >> 5;
  s[24] = t[7] >> 13;
  s[25] = (t[7] >> 21) | (t[8] * ((uint32_t) 1 << 4));
  s[26] = t[8] >> 4;
  s[27] = t[8] >> 12;
  s[28] = (t[8] >> 20) | (t[9] * ((uint32_t) 1 << 6));
  s[29] = t[9] >> 2;
  s[30] = t[9] >> 10;
  s[31] = t[9] >> 18;
}


void fe25519_pow22523(fe25519 *out, const fe25519 *z)
{
  fe25519 t0, t1, t2;
  int     i;

  fe25519_sq(&t0, z);
  fe25519_sq(&t1, &t0);
  fe25519_sq(&t1, &t1);
  fe25519_mul(&t1, z, &t1);
  fe25519_mul(&t0, &t0, &t1);
  fe25519_sq(&t0, &t0);
  fe25519_mul(&t0, &t1, &t0);
  fe25519_sq(&t1, &t0);
  for (i = 1; i < 5; ++i) {
      fe25519_sq(&t1, &t1);
  }
  fe25519_mul(&t0, &t1, &t0);
  fe25519_sq(&t1, &t0);
  for (i = 1; i < 10; ++i) {
      fe25519_sq(&t1, &t1);
  }
  fe25519_mul(&t1, &t1, &t0);
  fe25519_sq(&t2, &t1);
  for (i = 1; i < 20; ++i) {
      fe25519_sq(&t2, &t2);
  }
  fe25519_mul(&t1, &t2, &t1);
  fe25519_sq(&t1, &t1);
  for (i = 1; i < 10; ++i) {
      fe25519_sq(&t1, &t1);
  }
  fe25519_mul(&t0, &t1, &t0);
  fe25519_sq(&t1, &t0);
  for (i = 1; i < 50; ++i) {
      fe25519_sq(&t1, &t1);
  }
  fe25519_mul(&t1, &t1, &t0);
  fe25519_sq(&t2, &t1);
  for (i = 1; i < 100; ++i) {
      fe25519_sq(&t2, &t2);
  }
  fe25519_mul(&t1, &t2, &t1);
  fe25519_sq(&t1, &t1);
  for (i = 1; i < 50; ++i) {
      fe25519_sq(&t1, &t1);
  }
  fe25519_mul(&t0, &t1, &t0);
  fe25519_sq(&t0, &t0);
  fe25519_sq(&t0, &t0);
  fe25519_mul(out, &t0, z);
}

int fe25519_isnegative(const fe25519 *f)
{
  unsigned char s[32];

  fe25519_tobytes(s, f);

  return s[0] & 1;
}
