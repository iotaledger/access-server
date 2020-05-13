#include "utils.h"
#include "sc25519.h"

/*Arithmetic modulo the group order m = 2^252 +  27742317777372353535851937790883648493 = 7237005577332262213973186563042994240857116359379907606001950938285454250989 */

static const crypto_uint32 m[32] = {0xED, 0xD3, 0xF5, 0x5C, 0x1A, 0x63, 0x12, 0x58, 0xD6, 0x9C, 0xF7, 0xA2, 0xDE, 0xF9, 0xDE, 0x14, 
                                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10};

static const crypto_uint32 mu[33] = {0x1B, 0x13, 0x2C, 0x0A, 0xA3, 0xE5, 0x9C, 0xED, 0xA7, 0x29, 0x63, 0x08, 0x5D, 0x21, 0x06, 0x21, 
                                     0xEB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x0F};

static crypto_uint32 lt(crypto_uint32 a,crypto_uint32 b) /* 16-bit inputs */
{
  unsigned int x = a;
  x -= (unsigned int) b; /* 0..65535: no; 4294901761..4294967295: yes */
  x >>= 31; /* 0: no; 1: yes */
  return x;
}

/* Reduce coefficients of r before calling reduce_add_sub */
static void reduce_add_sub(sc25519 *r)
{
  crypto_uint32 pb = 0;
  crypto_uint32 b;
  crypto_uint32 mask;
  int i;
  unsigned char t[32];

  for(i=0;i<32;i++) 
  {
    pb += m[i];
    b = lt(r->v[i],pb);
    t[i] = r->v[i]-pb+(b<<8);
    pb = b;
  }
  mask = b - 1;
  for(i=0;i<32;i++) 
    r->v[i] ^= mask & (r->v[i] ^ t[i]);
}

/* Reduce coefficients of x before calling barrett_reduce */
static void barrett_reduce(sc25519 *r, const crypto_uint32 x[64])
{
  /* See HAC, Alg. 14.42 */
  int i,j;
  crypto_uint32 q2[66];
  crypto_uint32 *q3 = q2 + 33;
  crypto_uint32 r1[33];
  crypto_uint32 r2[33];
  crypto_uint32 carry;
  crypto_uint32 pb = 0;
  crypto_uint32 b;

  for (i = 0;i < 66;++i) q2[i] = 0;
  for (i = 0;i < 33;++i) r2[i] = 0;

  for(i=0;i<33;i++)
    for(j=0;j<33;j++)
      if(i+j >= 31) q2[i+j] += mu[i]*x[j+31];
  carry = q2[31] >> 8;
  q2[32] += carry;
  carry = q2[32] >> 8;
  q2[33] += carry;

  for(i=0;i<33;i++)r1[i] = x[i];
  for(i=0;i<32;i++)
    for(j=0;j<33;j++)
      if(i+j < 33) r2[i+j] += m[i]*q3[j];

  for(i=0;i<32;i++)
  {
    carry = r2[i] >> 8;
    r2[i+1] += carry;
    r2[i] &= 0xff;
  }

  for(i=0;i<32;i++) 
  {
    pb += r2[i];
    b = lt(r1[i],pb);
    r->v[i] = r1[i]-pb+(b<<8);
    pb = b;
  }

  /* XXX: Can it really happen that r<0?, See HAC, Alg 14.42, Step 3 
   * If so: Handle  it here!
   */

  reduce_add_sub(r);
  reduce_add_sub(r);
}

void sc25519_from32bytes(sc25519 *r, const unsigned char x[32])
{
  int i;
  crypto_uint32 t[64];
  for(i=0;i<32;i++) t[i] = x[i];
  for(i=32;i<64;++i) t[i] = 0;
  barrett_reduce(r, t);
}

void shortsc25519_from16bytes(shortsc25519 *r, const unsigned char x[16])
{
  int i;
  for(i=0;i<16;i++) r->v[i] = x[i];
}

void sc25519_from64bytes(sc25519 *r, const unsigned char x[64])
{
  int i;
  crypto_uint32 t[64];
  for(i=0;i<64;i++) t[i] = x[i];
  barrett_reduce(r, t);
}

void sc25519_from_shortsc(sc25519 *r, const shortsc25519 *x)
{
  int i;
  for(i=0;i<16;i++)
    r->v[i] = x->v[i];
  for(i=0;i<16;i++)
    r->v[16+i] = 0;
}

void sc25519_to32bytes(unsigned char r[32], const sc25519 *x)
{
  int i;
  for(i=0;i<32;i++) r[i] = x->v[i];
}

int sc25519_iszero_vartime(const sc25519 *x)
{
  int i;
  for(i=0;i<32;i++)
    if(x->v[i] != 0) return 0;
  return 1;
}

int sc25519_isshort_vartime(const sc25519 *x)
{
  int i;
  for(i=31;i>15;i--)
    if(x->v[i] != 0) return 0;
  return 1;
}

int sc25519_lt_vartime(const sc25519 *x, const sc25519 *y)
{
  int i;
  for(i=31;i>=0;i--)
  {
    if(x->v[i] < y->v[i]) return 1;
    if(x->v[i] > y->v[i]) return 0;
  }
  return 0;
}

void sc25519_add(sc25519 *r, const sc25519 *x, const sc25519 *y)
{
  int i, carry;
  for(i=0;i<32;i++) r->v[i] = x->v[i] + y->v[i];
  for(i=0;i<31;i++)
  {
    carry = r->v[i] >> 8;
    r->v[i+1] += carry;
    r->v[i] &= 0xff;
  }
  reduce_add_sub(r);
}

void sc25519_sub_nored(sc25519 *r, const sc25519 *x, const sc25519 *y)
{
  crypto_uint32 b = 0;
  crypto_uint32 t;
  int i;
  for(i=0;i<32;i++)
  {
    t = x->v[i] - y->v[i] - b;
    r->v[i] = t & 255;
    b = (t >> 8) & 1;
  }
}

void sc25519_mul(sc25519 *r, const sc25519 *x, const sc25519 *y)
{
  int i,j,carry;
  crypto_uint32 t[64];
  for(i=0;i<64;i++)t[i] = 0;

  for(i=0;i<32;i++)
    for(j=0;j<32;j++)
      t[i+j] += x->v[i] * y->v[j];

  /* Reduce coefficients */
  for(i=0;i<63;i++)
  {
    carry = t[i] >> 8;
    t[i+1] += carry;
    t[i] &= 0xff;
  }

  barrett_reduce(r, t);
}

void sc25519_mul_shortsc(sc25519 *r, const sc25519 *x, const shortsc25519 *y)
{
  sc25519 t;
  sc25519_from_shortsc(&t, y);
  sc25519_mul(r, x, &t);
}

void sc25519_window3(signed char r[85], const sc25519 *s)
{
  char carry;
  int i;
  for(i=0;i<10;i++)
  {
    r[8*i+0]  =  s->v[3*i+0]       & 7;
    r[8*i+1]  = (s->v[3*i+0] >> 3) & 7;
    r[8*i+2]  = (s->v[3*i+0] >> 6) & 7;
    r[8*i+2] ^= (s->v[3*i+1] << 2) & 7;
    r[8*i+3]  = (s->v[3*i+1] >> 1) & 7;
    r[8*i+4]  = (s->v[3*i+1] >> 4) & 7;
    r[8*i+5]  = (s->v[3*i+1] >> 7) & 7;
    r[8*i+5] ^= (s->v[3*i+2] << 1) & 7;
    r[8*i+6]  = (s->v[3*i+2] >> 2) & 7;
    r[8*i+7]  = (s->v[3*i+2] >> 5) & 7;
  }
  r[8*i+0]  =  s->v[3*i+0]       & 7;
  r[8*i+1]  = (s->v[3*i+0] >> 3) & 7;
  r[8*i+2]  = (s->v[3*i+0] >> 6) & 7;
  r[8*i+2] ^= (s->v[3*i+1] << 2) & 7;
  r[8*i+3]  = (s->v[3*i+1] >> 1) & 7;
  r[8*i+4]  = (s->v[3*i+1] >> 4) & 7;

  /* Making it signed */
  carry = 0;
  for(i=0;i<84;i++)
  {
    r[i] += carry;
    r[i+1] += r[i] >> 3;
    r[i] &= 7;
    carry = r[i] >> 2;
    r[i] -= carry<<3;
  }
  r[84] += carry;
}

void sc25519_window5(signed char r[51], const sc25519 *s)
{
  char carry;
  int i;
  for(i=0;i<6;i++)
  {
    r[8*i+0]  =  s->v[5*i+0]       & 31;
    r[8*i+1]  = (s->v[5*i+0] >> 5) & 31;
    r[8*i+1] ^= (s->v[5*i+1] << 3) & 31;
    r[8*i+2]  = (s->v[5*i+1] >> 2) & 31;
    r[8*i+3]  = (s->v[5*i+1] >> 7) & 31;
    r[8*i+3] ^= (s->v[5*i+2] << 1) & 31;
    r[8*i+4]  = (s->v[5*i+2] >> 4) & 31;
    r[8*i+4] ^= (s->v[5*i+3] << 4) & 31;
    r[8*i+5]  = (s->v[5*i+3] >> 1) & 31;
    r[8*i+6]  = (s->v[5*i+3] >> 6) & 31;
    r[8*i+6] ^= (s->v[5*i+4] << 2) & 31;
    r[8*i+7]  = (s->v[5*i+4] >> 3) & 31;
  }
  r[8*i+0]  =  s->v[5*i+0]       & 31;
  r[8*i+1]  = (s->v[5*i+0] >> 5) & 31;
  r[8*i+1] ^= (s->v[5*i+1] << 3) & 31;
  r[8*i+2]  = (s->v[5*i+1] >> 2) & 31;

  /* Making it signed */
  carry = 0;
  for(i=0;i<50;i++)
  {
    r[i] += carry;
    r[i+1] += r[i] >> 5;
    r[i] &= 31;
    carry = r[i] >> 4;
    r[i] -= carry<<5;
  }
  r[50] += carry;
}

void sc25519_2interleave2(unsigned char r[127], const sc25519 *s1, const sc25519 *s2)
{
  int i;
  for(i=0;i<31;i++)
  {
    r[4*i]   = ( s1->v[i]       & 3) ^ (( s2->v[i]       & 3) << 2);
    r[4*i+1] = ((s1->v[i] >> 2) & 3) ^ (((s2->v[i] >> 2) & 3) << 2);
    r[4*i+2] = ((s1->v[i] >> 4) & 3) ^ (((s2->v[i] >> 4) & 3) << 2);
    r[4*i+3] = ((s1->v[i] >> 6) & 3) ^ (((s2->v[i] >> 6) & 3) << 2);
  }
  r[124] = ( s1->v[31]       & 3) ^ (( s2->v[31]       & 3) << 2);
  r[125] = ((s1->v[31] >> 2) & 3) ^ (((s2->v[31] >> 2) & 3) << 2);
  r[126] = ((s1->v[31] >> 4) & 3) ^ (((s2->v[31] >> 4) & 3) << 2);
}

int sc25519_is_canonical(const unsigned char s[32])
{
  /* 2^252+27742317777372353535851937790883648493 */
  static const unsigned char L[32] = {
      0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58, 0xd6, 0x9c, 0xf7,
      0xa2, 0xde, 0xf9, 0xde, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10
  };
  unsigned char c = 0;
  unsigned char n = 1;
  unsigned int  i = 32;

  do {
      i--;
      c |= ((s[i] - L[i]) >> 8) & n;
      n &= ((s[i] ^ L[i]) - 1) >> 8;
  } while (i != 0);

  return (c != 0);
}

void sc25519_reduce(unsigned char s[64])
{
  int64_t s0  = 2097151 & load_3(s);
  int64_t s1  = 2097151 & (load_4(s + 2) >> 5);
  int64_t s2  = 2097151 & (load_3(s + 5) >> 2);
  int64_t s3  = 2097151 & (load_4(s + 7) >> 7);
  int64_t s4  = 2097151 & (load_4(s + 10) >> 4);
  int64_t s5  = 2097151 & (load_3(s + 13) >> 1);
  int64_t s6  = 2097151 & (load_4(s + 15) >> 6);
  int64_t s7  = 2097151 & (load_3(s + 18) >> 3);
  int64_t s8  = 2097151 & load_3(s + 21);
  int64_t s9  = 2097151 & (load_4(s + 23) >> 5);
  int64_t s10 = 2097151 & (load_3(s + 26) >> 2);
  int64_t s11 = 2097151 & (load_4(s + 28) >> 7);
  int64_t s12 = 2097151 & (load_4(s + 31) >> 4);
  int64_t s13 = 2097151 & (load_3(s + 34) >> 1);
  int64_t s14 = 2097151 & (load_4(s + 36) >> 6);
  int64_t s15 = 2097151 & (load_3(s + 39) >> 3);
  int64_t s16 = 2097151 & load_3(s + 42);
  int64_t s17 = 2097151 & (load_4(s + 44) >> 5);
  int64_t s18 = 2097151 & (load_3(s + 47) >> 2);
  int64_t s19 = 2097151 & (load_4(s + 49) >> 7);
  int64_t s20 = 2097151 & (load_4(s + 52) >> 4);
  int64_t s21 = 2097151 & (load_3(s + 55) >> 1);
  int64_t s22 = 2097151 & (load_4(s + 57) >> 6);
  int64_t s23 = (load_4(s + 60) >> 3);

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
  int64_t carry10;
  int64_t carry11;
  int64_t carry12;
  int64_t carry13;
  int64_t carry14;
  int64_t carry15;
  int64_t carry16;

  s11 += s23 * 666643;
  s12 += s23 * 470296;
  s13 += s23 * 654183;
  s14 -= s23 * 997805;
  s15 += s23 * 136657;
  s16 -= s23 * 683901;

  s10 += s22 * 666643;
  s11 += s22 * 470296;
  s12 += s22 * 654183;
  s13 -= s22 * 997805;
  s14 += s22 * 136657;
  s15 -= s22 * 683901;

  s9 += s21 * 666643;
  s10 += s21 * 470296;
  s11 += s21 * 654183;
  s12 -= s21 * 997805;
  s13 += s21 * 136657;
  s14 -= s21 * 683901;

  s8 += s20 * 666643;
  s9 += s20 * 470296;
  s10 += s20 * 654183;
  s11 -= s20 * 997805;
  s12 += s20 * 136657;
  s13 -= s20 * 683901;

  s7 += s19 * 666643;
  s8 += s19 * 470296;
  s9 += s19 * 654183;
  s10 -= s19 * 997805;
  s11 += s19 * 136657;
  s12 -= s19 * 683901;

  s6 += s18 * 666643;
  s7 += s18 * 470296;
  s8 += s18 * 654183;
  s9 -= s18 * 997805;
  s10 += s18 * 136657;
  s11 -= s18 * 683901;

  carry6 = (s6 + (int64_t) (1L << 20)) >> 21;
  s7 += carry6;
  s6 -= carry6 * ((uint64_t) 1L << 21);
  carry8 = (s8 + (int64_t) (1L << 20)) >> 21;
  s9 += carry8;
  s8 -= carry8 * ((uint64_t) 1L << 21);
  carry10 = (s10 + (int64_t) (1L << 20)) >> 21;
  s11 += carry10;
  s10 -= carry10 * ((uint64_t) 1L << 21);
  carry12 = (s12 + (int64_t) (1L << 20)) >> 21;
  s13 += carry12;
  s12 -= carry12 * ((uint64_t) 1L << 21);
  carry14 = (s14 + (int64_t) (1L << 20)) >> 21;
  s15 += carry14;
  s14 -= carry14 * ((uint64_t) 1L << 21);
  carry16 = (s16 + (int64_t) (1L << 20)) >> 21;
  s17 += carry16;
  s16 -= carry16 * ((uint64_t) 1L << 21);

  carry7 = (s7 + (int64_t) (1L << 20)) >> 21;
  s8 += carry7;
  s7 -= carry7 * ((uint64_t) 1L << 21);
  carry9 = (s9 + (int64_t) (1L << 20)) >> 21;
  s10 += carry9;
  s9 -= carry9 * ((uint64_t) 1L << 21);
  carry11 = (s11 + (int64_t) (1L << 20)) >> 21;
  s12 += carry11;
  s11 -= carry11 * ((uint64_t) 1L << 21);
  carry13 = (s13 + (int64_t) (1L << 20)) >> 21;
  s14 += carry13;
  s13 -= carry13 * ((uint64_t) 1L << 21);
  carry15 = (s15 + (int64_t) (1L << 20)) >> 21;
  s16 += carry15;
  s15 -= carry15 * ((uint64_t) 1L << 21);

  s5 += s17 * 666643;
  s6 += s17 * 470296;
  s7 += s17 * 654183;
  s8 -= s17 * 997805;
  s9 += s17 * 136657;
  s10 -= s17 * 683901;

  s4 += s16 * 666643;
  s5 += s16 * 470296;
  s6 += s16 * 654183;
  s7 -= s16 * 997805;
  s8 += s16 * 136657;
  s9 -= s16 * 683901;

  s3 += s15 * 666643;
  s4 += s15 * 470296;
  s5 += s15 * 654183;
  s6 -= s15 * 997805;
  s7 += s15 * 136657;
  s8 -= s15 * 683901;

  s2 += s14 * 666643;
  s3 += s14 * 470296;
  s4 += s14 * 654183;
  s5 -= s14 * 997805;
  s6 += s14 * 136657;
  s7 -= s14 * 683901;

  s1 += s13 * 666643;
  s2 += s13 * 470296;
  s3 += s13 * 654183;
  s4 -= s13 * 997805;
  s5 += s13 * 136657;
  s6 -= s13 * 683901;

  s0 += s12 * 666643;
  s1 += s12 * 470296;
  s2 += s12 * 654183;
  s3 -= s12 * 997805;
  s4 += s12 * 136657;
  s5 -= s12 * 683901;
  s12 = 0;

  carry0 = (s0 + (int64_t) (1L << 20)) >> 21;
  s1 += carry0;
  s0 -= carry0 * ((uint64_t) 1L << 21);
  carry2 = (s2 + (int64_t) (1L << 20)) >> 21;
  s3 += carry2;
  s2 -= carry2 * ((uint64_t) 1L << 21);
  carry4 = (s4 + (int64_t) (1L << 20)) >> 21;
  s5 += carry4;
  s4 -= carry4 * ((uint64_t) 1L << 21);
  carry6 = (s6 + (int64_t) (1L << 20)) >> 21;
  s7 += carry6;
  s6 -= carry6 * ((uint64_t) 1L << 21);
  carry8 = (s8 + (int64_t) (1L << 20)) >> 21;
  s9 += carry8;
  s8 -= carry8 * ((uint64_t) 1L << 21);
  carry10 = (s10 + (int64_t) (1L << 20)) >> 21;
  s11 += carry10;
  s10 -= carry10 * ((uint64_t) 1L << 21);

  carry1 = (s1 + (int64_t) (1L << 20)) >> 21;
  s2 += carry1;
  s1 -= carry1 * ((uint64_t) 1L << 21);
  carry3 = (s3 + (int64_t) (1L << 20)) >> 21;
  s4 += carry3;
  s3 -= carry3 * ((uint64_t) 1L << 21);
  carry5 = (s5 + (int64_t) (1L << 20)) >> 21;
  s6 += carry5;
  s5 -= carry5 * ((uint64_t) 1L << 21);
  carry7 = (s7 + (int64_t) (1L << 20)) >> 21;
  s8 += carry7;
  s7 -= carry7 * ((uint64_t) 1L << 21);
  carry9 = (s9 + (int64_t) (1L << 20)) >> 21;
  s10 += carry9;
  s9 -= carry9 * ((uint64_t) 1L << 21);
  carry11 = (s11 + (int64_t) (1L << 20)) >> 21;
  s12 += carry11;
  s11 -= carry11 * ((uint64_t) 1L << 21);

  s0 += s12 * 666643;
  s1 += s12 * 470296;
  s2 += s12 * 654183;
  s3 -= s12 * 997805;
  s4 += s12 * 136657;
  s5 -= s12 * 683901;
  s12 = 0;

  carry0 = s0 >> 21;
  s1 += carry0;
  s0 -= carry0 * ((uint64_t) 1L << 21);
  carry1 = s1 >> 21;
  s2 += carry1;
  s1 -= carry1 * ((uint64_t) 1L << 21);
  carry2 = s2 >> 21;
  s3 += carry2;
  s2 -= carry2 * ((uint64_t) 1L << 21);
  carry3 = s3 >> 21;
  s4 += carry3;
  s3 -= carry3 * ((uint64_t) 1L << 21);
  carry4 = s4 >> 21;
  s5 += carry4;
  s4 -= carry4 * ((uint64_t) 1L << 21);
  carry5 = s5 >> 21;
  s6 += carry5;
  s5 -= carry5 * ((uint64_t) 1L << 21);
  carry6 = s6 >> 21;
  s7 += carry6;
  s6 -= carry6 * ((uint64_t) 1L << 21);
  carry7 = s7 >> 21;
  s8 += carry7;
  s7 -= carry7 * ((uint64_t) 1L << 21);
  carry8 = s8 >> 21;
  s9 += carry8;
  s8 -= carry8 * ((uint64_t) 1L << 21);
  carry9 = s9 >> 21;
  s10 += carry9;
  s9 -= carry9 * ((uint64_t) 1L << 21);
  carry10 = s10 >> 21;
  s11 += carry10;
  s10 -= carry10 * ((uint64_t) 1L << 21);
  carry11 = s11 >> 21;
  s12 += carry11;
  s11 -= carry11 * ((uint64_t) 1L << 21);

  s0 += s12 * 666643;
  s1 += s12 * 470296;
  s2 += s12 * 654183;
  s3 -= s12 * 997805;
  s4 += s12 * 136657;
  s5 -= s12 * 683901;

  carry0 = s0 >> 21;
  s1 += carry0;
  s0 -= carry0 * ((uint64_t) 1L << 21);
  carry1 = s1 >> 21;
  s2 += carry1;
  s1 -= carry1 * ((uint64_t) 1L << 21);
  carry2 = s2 >> 21;
  s3 += carry2;
  s2 -= carry2 * ((uint64_t) 1L << 21);
  carry3 = s3 >> 21;
  s4 += carry3;
  s3 -= carry3 * ((uint64_t) 1L << 21);
  carry4 = s4 >> 21;
  s5 += carry4;
  s4 -= carry4 * ((uint64_t) 1L << 21);
  carry5 = s5 >> 21;
  s6 += carry5;
  s5 -= carry5 * ((uint64_t) 1L << 21);
  carry6 = s6 >> 21;
  s7 += carry6;
  s6 -= carry6 * ((uint64_t) 1L << 21);
  carry7 = s7 >> 21;
  s8 += carry7;
  s7 -= carry7 * ((uint64_t) 1L << 21);
  carry8 = s8 >> 21;
  s9 += carry8;
  s8 -= carry8 * ((uint64_t) 1L << 21);
  carry9 = s9 >> 21;
  s10 += carry9;
  s9 -= carry9 * ((uint64_t) 1L << 21);
  carry10 = s10 >> 21;
  s11 += carry10;
  s10 -= carry10 * ((uint64_t) 1L << 21);

  s[0]  = s0 >> 0;
  s[1]  = s0 >> 8;
  s[2]  = (s0 >> 16) | (s1 * ((uint64_t) 1 << 5));
  s[3]  = s1 >> 3;
  s[4]  = s1 >> 11;
  s[5]  = (s1 >> 19) | (s2 * ((uint64_t) 1 << 2));
  s[6]  = s2 >> 6;
  s[7]  = (s2 >> 14) | (s3 * ((uint64_t) 1 << 7));
  s[8]  = s3 >> 1;
  s[9]  = s3 >> 9;
  s[10] = (s3 >> 17) | (s4 * ((uint64_t) 1 << 4));
  s[11] = s4 >> 4;
  s[12] = s4 >> 12;
  s[13] = (s4 >> 20) | (s5 * ((uint64_t) 1 << 1));
  s[14] = s5 >> 7;
  s[15] = (s5 >> 15) | (s6 * ((uint64_t) 1 << 6));
  s[16] = s6 >> 2;
  s[17] = s6 >> 10;
  s[18] = (s6 >> 18) | (s7 * ((uint64_t) 1 << 3));
  s[19] = s7 >> 5;
  s[20] = s7 >> 13;
  s[21] = s8 >> 0;
  s[22] = s8 >> 8;
  s[23] = (s8 >> 16) | (s9 * ((uint64_t) 1 << 5));
  s[24] = s9 >> 3;
  s[25] = s9 >> 11;
  s[26] = (s9 >> 19) | (s10 * ((uint64_t) 1 << 2));
  s[27] = s10 >> 6;
  s[28] = (s10 >> 14) | (s11 * ((uint64_t) 1 << 7));
  s[29] = s11 >> 1;
  s[30] = s11 >> 9;
  s[31] = s11 >> 17;
}

void sc25519_muladd(unsigned char s[32], const unsigned char a[32],
               const unsigned char b[32], const unsigned char c[32])
{
    int64_t a0  = 2097151 & load_3(a);
    int64_t a1  = 2097151 & (load_4(a + 2) >> 5);
    int64_t a2  = 2097151 & (load_3(a + 5) >> 2);
    int64_t a3  = 2097151 & (load_4(a + 7) >> 7);
    int64_t a4  = 2097151 & (load_4(a + 10) >> 4);
    int64_t a5  = 2097151 & (load_3(a + 13) >> 1);
    int64_t a6  = 2097151 & (load_4(a + 15) >> 6);
    int64_t a7  = 2097151 & (load_3(a + 18) >> 3);
    int64_t a8  = 2097151 & load_3(a + 21);
    int64_t a9  = 2097151 & (load_4(a + 23) >> 5);
    int64_t a10 = 2097151 & (load_3(a + 26) >> 2);
    int64_t a11 = (load_4(a + 28) >> 7);

    int64_t b0  = 2097151 & load_3(b);
    int64_t b1  = 2097151 & (load_4(b + 2) >> 5);
    int64_t b2  = 2097151 & (load_3(b + 5) >> 2);
    int64_t b3  = 2097151 & (load_4(b + 7) >> 7);
    int64_t b4  = 2097151 & (load_4(b + 10) >> 4);
    int64_t b5  = 2097151 & (load_3(b + 13) >> 1);
    int64_t b6  = 2097151 & (load_4(b + 15) >> 6);
    int64_t b7  = 2097151 & (load_3(b + 18) >> 3);
    int64_t b8  = 2097151 & load_3(b + 21);
    int64_t b9  = 2097151 & (load_4(b + 23) >> 5);
    int64_t b10 = 2097151 & (load_3(b + 26) >> 2);
    int64_t b11 = (load_4(b + 28) >> 7);

    int64_t c0  = 2097151 & load_3(c);
    int64_t c1  = 2097151 & (load_4(c + 2) >> 5);
    int64_t c2  = 2097151 & (load_3(c + 5) >> 2);
    int64_t c3  = 2097151 & (load_4(c + 7) >> 7);
    int64_t c4  = 2097151 & (load_4(c + 10) >> 4);
    int64_t c5  = 2097151 & (load_3(c + 13) >> 1);
    int64_t c6  = 2097151 & (load_4(c + 15) >> 6);
    int64_t c7  = 2097151 & (load_3(c + 18) >> 3);
    int64_t c8  = 2097151 & load_3(c + 21);
    int64_t c9  = 2097151 & (load_4(c + 23) >> 5);
    int64_t c10 = 2097151 & (load_3(c + 26) >> 2);
    int64_t c11 = (load_4(c + 28) >> 7);

    int64_t s0;
    int64_t s1;
    int64_t s2;
    int64_t s3;
    int64_t s4;
    int64_t s5;
    int64_t s6;
    int64_t s7;
    int64_t s8;
    int64_t s9;
    int64_t s10;
    int64_t s11;
    int64_t s12;
    int64_t s13;
    int64_t s14;
    int64_t s15;
    int64_t s16;
    int64_t s17;
    int64_t s18;
    int64_t s19;
    int64_t s20;
    int64_t s21;
    int64_t s22;
    int64_t s23;

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
    int64_t carry10;
    int64_t carry11;
    int64_t carry12;
    int64_t carry13;
    int64_t carry14;
    int64_t carry15;
    int64_t carry16;
    int64_t carry17;
    int64_t carry18;
    int64_t carry19;
    int64_t carry20;
    int64_t carry21;
    int64_t carry22;

    s0 = c0 + a0 * b0;
    s1 = c1 + a0 * b1 + a1 * b0;
    s2 = c2 + a0 * b2 + a1 * b1 + a2 * b0;
    s3 = c3 + a0 * b3 + a1 * b2 + a2 * b1 + a3 * b0;
    s4 = c4 + a0 * b4 + a1 * b3 + a2 * b2 + a3 * b1 + a4 * b0;
    s5 = c5 + a0 * b5 + a1 * b4 + a2 * b3 + a3 * b2 + a4 * b1 + a5 * b0;
    s6 = c6 + a0 * b6 + a1 * b5 + a2 * b4 + a3 * b3 + a4 * b2 + a5 * b1 +
         a6 * b0;
    s7 = c7 + a0 * b7 + a1 * b6 + a2 * b5 + a3 * b4 + a4 * b3 + a5 * b2 +
         a6 * b1 + a7 * b0;
    s8 = c8 + a0 * b8 + a1 * b7 + a2 * b6 + a3 * b5 + a4 * b4 + a5 * b3 +
         a6 * b2 + a7 * b1 + a8 * b0;
    s9 = c9 + a0 * b9 + a1 * b8 + a2 * b7 + a3 * b6 + a4 * b5 + a5 * b4 +
         a6 * b3 + a7 * b2 + a8 * b1 + a9 * b0;
    s10 = c10 + a0 * b10 + a1 * b9 + a2 * b8 + a3 * b7 + a4 * b6 + a5 * b5 +
          a6 * b4 + a7 * b3 + a8 * b2 + a9 * b1 + a10 * b0;
    s11 = c11 + a0 * b11 + a1 * b10 + a2 * b9 + a3 * b8 + a4 * b7 + a5 * b6 +
          a6 * b5 + a7 * b4 + a8 * b3 + a9 * b2 + a10 * b1 + a11 * b0;
    s12 = a1 * b11 + a2 * b10 + a3 * b9 + a4 * b8 + a5 * b7 + a6 * b6 +
          a7 * b5 + a8 * b4 + a9 * b3 + a10 * b2 + a11 * b1;
    s13 = a2 * b11 + a3 * b10 + a4 * b9 + a5 * b8 + a6 * b7 + a7 * b6 +
          a8 * b5 + a9 * b4 + a10 * b3 + a11 * b2;
    s14 = a3 * b11 + a4 * b10 + a5 * b9 + a6 * b8 + a7 * b7 + a8 * b6 +
          a9 * b5 + a10 * b4 + a11 * b3;
    s15 = a4 * b11 + a5 * b10 + a6 * b9 + a7 * b8 + a8 * b7 + a9 * b6 +
          a10 * b5 + a11 * b4;
    s16 =
        a5 * b11 + a6 * b10 + a7 * b9 + a8 * b8 + a9 * b7 + a10 * b6 + a11 * b5;
    s17 = a6 * b11 + a7 * b10 + a8 * b9 + a9 * b8 + a10 * b7 + a11 * b6;
    s18 = a7 * b11 + a8 * b10 + a9 * b9 + a10 * b8 + a11 * b7;
    s19 = a8 * b11 + a9 * b10 + a10 * b9 + a11 * b8;
    s20 = a9 * b11 + a10 * b10 + a11 * b9;
    s21 = a10 * b11 + a11 * b10;
    s22 = a11 * b11;
    s23 = 0;

    carry0 = (s0 + (int64_t) (1L << 20)) >> 21;
    s1 += carry0;
    s0 -= carry0 * ((uint64_t) 1L << 21);
    carry2 = (s2 + (int64_t) (1L << 20)) >> 21;
    s3 += carry2;
    s2 -= carry2 * ((uint64_t) 1L << 21);
    carry4 = (s4 + (int64_t) (1L << 20)) >> 21;
    s5 += carry4;
    s4 -= carry4 * ((uint64_t) 1L << 21);
    carry6 = (s6 + (int64_t) (1L << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry8 = (s8 + (int64_t) (1L << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry10 = (s10 + (int64_t) (1L << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);
    carry12 = (s12 + (int64_t) (1L << 20)) >> 21;
    s13 += carry12;
    s12 -= carry12 * ((uint64_t) 1L << 21);
    carry14 = (s14 + (int64_t) (1L << 20)) >> 21;
    s15 += carry14;
    s14 -= carry14 * ((uint64_t) 1L << 21);
    carry16 = (s16 + (int64_t) (1L << 20)) >> 21;
    s17 += carry16;
    s16 -= carry16 * ((uint64_t) 1L << 21);
    carry18 = (s18 + (int64_t) (1L << 20)) >> 21;
    s19 += carry18;
    s18 -= carry18 * ((uint64_t) 1L << 21);
    carry20 = (s20 + (int64_t) (1L << 20)) >> 21;
    s21 += carry20;
    s20 -= carry20 * ((uint64_t) 1L << 21);
    carry22 = (s22 + (int64_t) (1L << 20)) >> 21;
    s23 += carry22;
    s22 -= carry22 * ((uint64_t) 1L << 21);

    carry1 = (s1 + (int64_t) (1L << 20)) >> 21;
    s2 += carry1;
    s1 -= carry1 * ((uint64_t) 1L << 21);
    carry3 = (s3 + (int64_t) (1L << 20)) >> 21;
    s4 += carry3;
    s3 -= carry3 * ((uint64_t) 1L << 21);
    carry5 = (s5 + (int64_t) (1L << 20)) >> 21;
    s6 += carry5;
    s5 -= carry5 * ((uint64_t) 1L << 21);
    carry7 = (s7 + (int64_t) (1L << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry9 = (s9 + (int64_t) (1L << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry11 = (s11 + (int64_t) (1L << 20)) >> 21;
    s12 += carry11;
    s11 -= carry11 * ((uint64_t) 1L << 21);
    carry13 = (s13 + (int64_t) (1L << 20)) >> 21;
    s14 += carry13;
    s13 -= carry13 * ((uint64_t) 1L << 21);
    carry15 = (s15 + (int64_t) (1L << 20)) >> 21;
    s16 += carry15;
    s15 -= carry15 * ((uint64_t) 1L << 21);
    carry17 = (s17 + (int64_t) (1L << 20)) >> 21;
    s18 += carry17;
    s17 -= carry17 * ((uint64_t) 1L << 21);
    carry19 = (s19 + (int64_t) (1L << 20)) >> 21;
    s20 += carry19;
    s19 -= carry19 * ((uint64_t) 1L << 21);
    carry21 = (s21 + (int64_t) (1L << 20)) >> 21;
    s22 += carry21;
    s21 -= carry21 * ((uint64_t) 1L << 21);

    s11 += s23 * 666643;
    s12 += s23 * 470296;
    s13 += s23 * 654183;
    s14 -= s23 * 997805;
    s15 += s23 * 136657;
    s16 -= s23 * 683901;

    s10 += s22 * 666643;
    s11 += s22 * 470296;
    s12 += s22 * 654183;
    s13 -= s22 * 997805;
    s14 += s22 * 136657;
    s15 -= s22 * 683901;

    s9 += s21 * 666643;
    s10 += s21 * 470296;
    s11 += s21 * 654183;
    s12 -= s21 * 997805;
    s13 += s21 * 136657;
    s14 -= s21 * 683901;

    s8 += s20 * 666643;
    s9 += s20 * 470296;
    s10 += s20 * 654183;
    s11 -= s20 * 997805;
    s12 += s20 * 136657;
    s13 -= s20 * 683901;

    s7 += s19 * 666643;
    s8 += s19 * 470296;
    s9 += s19 * 654183;
    s10 -= s19 * 997805;
    s11 += s19 * 136657;
    s12 -= s19 * 683901;

    s6 += s18 * 666643;
    s7 += s18 * 470296;
    s8 += s18 * 654183;
    s9 -= s18 * 997805;
    s10 += s18 * 136657;
    s11 -= s18 * 683901;

    carry6 = (s6 + (int64_t) (1L << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry8 = (s8 + (int64_t) (1L << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry10 = (s10 + (int64_t) (1L << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);
    carry12 = (s12 + (int64_t) (1L << 20)) >> 21;
    s13 += carry12;
    s12 -= carry12 * ((uint64_t) 1L << 21);
    carry14 = (s14 + (int64_t) (1L << 20)) >> 21;
    s15 += carry14;
    s14 -= carry14 * ((uint64_t) 1L << 21);
    carry16 = (s16 + (int64_t) (1L << 20)) >> 21;
    s17 += carry16;
    s16 -= carry16 * ((uint64_t) 1L << 21);

    carry7 = (s7 + (int64_t) (1L << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry9 = (s9 + (int64_t) (1L << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry11 = (s11 + (int64_t) (1L << 20)) >> 21;
    s12 += carry11;
    s11 -= carry11 * ((uint64_t) 1L << 21);
    carry13 = (s13 + (int64_t) (1L << 20)) >> 21;
    s14 += carry13;
    s13 -= carry13 * ((uint64_t) 1L << 21);
    carry15 = (s15 + (int64_t) (1L << 20)) >> 21;
    s16 += carry15;
    s15 -= carry15 * ((uint64_t) 1L << 21);

    s5 += s17 * 666643;
    s6 += s17 * 470296;
    s7 += s17 * 654183;
    s8 -= s17 * 997805;
    s9 += s17 * 136657;
    s10 -= s17 * 683901;

    s4 += s16 * 666643;
    s5 += s16 * 470296;
    s6 += s16 * 654183;
    s7 -= s16 * 997805;
    s8 += s16 * 136657;
    s9 -= s16 * 683901;

    s3 += s15 * 666643;
    s4 += s15 * 470296;
    s5 += s15 * 654183;
    s6 -= s15 * 997805;
    s7 += s15 * 136657;
    s8 -= s15 * 683901;

    s2 += s14 * 666643;
    s3 += s14 * 470296;
    s4 += s14 * 654183;
    s5 -= s14 * 997805;
    s6 += s14 * 136657;
    s7 -= s14 * 683901;

    s1 += s13 * 666643;
    s2 += s13 * 470296;
    s3 += s13 * 654183;
    s4 -= s13 * 997805;
    s5 += s13 * 136657;
    s6 -= s13 * 683901;

    s0 += s12 * 666643;
    s1 += s12 * 470296;
    s2 += s12 * 654183;
    s3 -= s12 * 997805;
    s4 += s12 * 136657;
    s5 -= s12 * 683901;
    s12 = 0;

    carry0 = (s0 + (int64_t) (1L << 20)) >> 21;
    s1 += carry0;
    s0 -= carry0 * ((uint64_t) 1L << 21);
    carry2 = (s2 + (int64_t) (1L << 20)) >> 21;
    s3 += carry2;
    s2 -= carry2 * ((uint64_t) 1L << 21);
    carry4 = (s4 + (int64_t) (1L << 20)) >> 21;
    s5 += carry4;
    s4 -= carry4 * ((uint64_t) 1L << 21);
    carry6 = (s6 + (int64_t) (1L << 20)) >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry8 = (s8 + (int64_t) (1L << 20)) >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry10 = (s10 + (int64_t) (1L << 20)) >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);

    carry1 = (s1 + (int64_t) (1L << 20)) >> 21;
    s2 += carry1;
    s1 -= carry1 * ((uint64_t) 1L << 21);
    carry3 = (s3 + (int64_t) (1L << 20)) >> 21;
    s4 += carry3;
    s3 -= carry3 * ((uint64_t) 1L << 21);
    carry5 = (s5 + (int64_t) (1L << 20)) >> 21;
    s6 += carry5;
    s5 -= carry5 * ((uint64_t) 1L << 21);
    carry7 = (s7 + (int64_t) (1L << 20)) >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry9 = (s9 + (int64_t) (1L << 20)) >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry11 = (s11 + (int64_t) (1L << 20)) >> 21;
    s12 += carry11;
    s11 -= carry11 * ((uint64_t) 1L << 21);

    s0 += s12 * 666643;
    s1 += s12 * 470296;
    s2 += s12 * 654183;
    s3 -= s12 * 997805;
    s4 += s12 * 136657;
    s5 -= s12 * 683901;
    s12 = 0;

    carry0 = s0 >> 21;
    s1 += carry0;
    s0 -= carry0 * ((uint64_t) 1L << 21);
    carry1 = s1 >> 21;
    s2 += carry1;
    s1 -= carry1 * ((uint64_t) 1L << 21);
    carry2 = s2 >> 21;
    s3 += carry2;
    s2 -= carry2 * ((uint64_t) 1L << 21);
    carry3 = s3 >> 21;
    s4 += carry3;
    s3 -= carry3 * ((uint64_t) 1L << 21);
    carry4 = s4 >> 21;
    s5 += carry4;
    s4 -= carry4 * ((uint64_t) 1L << 21);
    carry5 = s5 >> 21;
    s6 += carry5;
    s5 -= carry5 * ((uint64_t) 1L << 21);
    carry6 = s6 >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry7 = s7 >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry8 = s8 >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry9 = s9 >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry10 = s10 >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);
    carry11 = s11 >> 21;
    s12 += carry11;
    s11 -= carry11 * ((uint64_t) 1L << 21);

    s0 += s12 * 666643;
    s1 += s12 * 470296;
    s2 += s12 * 654183;
    s3 -= s12 * 997805;
    s4 += s12 * 136657;
    s5 -= s12 * 683901;

    carry0 = s0 >> 21;
    s1 += carry0;
    s0 -= carry0 * ((uint64_t) 1L << 21);
    carry1 = s1 >> 21;
    s2 += carry1;
    s1 -= carry1 * ((uint64_t) 1L << 21);
    carry2 = s2 >> 21;
    s3 += carry2;
    s2 -= carry2 * ((uint64_t) 1L << 21);
    carry3 = s3 >> 21;
    s4 += carry3;
    s3 -= carry3 * ((uint64_t) 1L << 21);
    carry4 = s4 >> 21;
    s5 += carry4;
    s4 -= carry4 * ((uint64_t) 1L << 21);
    carry5 = s5 >> 21;
    s6 += carry5;
    s5 -= carry5 * ((uint64_t) 1L << 21);
    carry6 = s6 >> 21;
    s7 += carry6;
    s6 -= carry6 * ((uint64_t) 1L << 21);
    carry7 = s7 >> 21;
    s8 += carry7;
    s7 -= carry7 * ((uint64_t) 1L << 21);
    carry8 = s8 >> 21;
    s9 += carry8;
    s8 -= carry8 * ((uint64_t) 1L << 21);
    carry9 = s9 >> 21;
    s10 += carry9;
    s9 -= carry9 * ((uint64_t) 1L << 21);
    carry10 = s10 >> 21;
    s11 += carry10;
    s10 -= carry10 * ((uint64_t) 1L << 21);

    s[0]  = s0 >> 0;
    s[1]  = s0 >> 8;
    s[2]  = (s0 >> 16) | (s1 * ((uint64_t) 1 << 5));
    s[3]  = s1 >> 3;
    s[4]  = s1 >> 11;
    s[5]  = (s1 >> 19) | (s2 * ((uint64_t) 1 << 2));
    s[6]  = s2 >> 6;
    s[7]  = (s2 >> 14) | (s3 * ((uint64_t) 1 << 7));
    s[8]  = s3 >> 1;
    s[9]  = s3 >> 9;
    s[10] = (s3 >> 17) | (s4 * ((uint64_t) 1 << 4));
    s[11] = s4 >> 4;
    s[12] = s4 >> 12;
    s[13] = (s4 >> 20) | (s5 * ((uint64_t) 1 << 1));
    s[14] = s5 >> 7;
    s[15] = (s5 >> 15) | (s6 * ((uint64_t) 1 << 6));
    s[16] = s6 >> 2;
    s[17] = s6 >> 10;
    s[18] = (s6 >> 18) | (s7 * ((uint64_t) 1 << 3));
    s[19] = s7 >> 5;
    s[20] = s7 >> 13;
    s[21] = s8 >> 0;
    s[22] = s8 >> 8;
    s[23] = (s8 >> 16) | (s9 * ((uint64_t) 1 << 5));
    s[24] = s9 >> 3;
    s[25] = s9 >> 11;
    s[26] = (s9 >> 19) | (s10 * ((uint64_t) 1 << 2));
    s[27] = s10 >> 6;
    s[28] = (s10 >> 14) | (s11 * ((uint64_t) 1 << 7));
    s[29] = s11 >> 1;
    s[30] = s11 >> 9;
    s[31] = s11 >> 17;
}
