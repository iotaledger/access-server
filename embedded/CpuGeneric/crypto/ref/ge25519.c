#include <stddef.h>
#include "fe25519.h"
#include "sc25519.h"
#include "ge25519.h"
#include "constants.h"

/* 
 * Arithmetic on the twisted Edwards curve -x^2 + y^2 = 1 + dx^2y^2 
 * with d = -(121665/121666) = 37095705934669439343138083508754565189542113879843219016388785533085940283555
 * Base point: (15112221349535400772501151409588531511454012693041857206046113283949847762202,46316835694926478169428394003475163141307993866256225615783033603165251855960);
 */

/* d */
static const fe25519 ge25519_ecd = {{0xA3, 0x78, 0x59, 0x13, 0xCA, 0x4D, 0xEB, 0x75, 0xAB, 0xD8, 0x41, 0x41, 0x4D, 0x0A, 0x70, 0x00, 
                      0x98, 0xE8, 0x79, 0x77, 0x79, 0x40, 0xC7, 0x8C, 0x73, 0xFE, 0x6F, 0x2B, 0xEE, 0x6C, 0x03, 0x52}};
/* 2*d */
static const fe25519 ge25519_ec2d = {{0x59, 0xF1, 0xB2, 0x26, 0x94, 0x9B, 0xD6, 0xEB, 0x56, 0xB1, 0x83, 0x82, 0x9A, 0x14, 0xE0, 0x00, 
                       0x30, 0xD1, 0xF3, 0xEE, 0xF2, 0x80, 0x8E, 0x19, 0xE7, 0xFC, 0xDF, 0x56, 0xDC, 0xD9, 0x06, 0x24}};
/* sqrt(-1) */
static const fe25519 ge25519_sqrtm1 = {{0xB0, 0xA0, 0x0E, 0x4A, 0x27, 0x1B, 0xEE, 0xC4, 0x78, 0xE4, 0x2F, 0xAD, 0x06, 0x18, 0x43, 0x2F, 
                         0xA7, 0xD7, 0xFB, 0x3D, 0x99, 0x00, 0x4D, 0x2B, 0x0B, 0xDF, 0xC1, 0x4F, 0x80, 0x24, 0x83, 0x2B}};

typedef struct
{
  fe25519 x;
  fe25519 y;
} ge25519_aff;


static const ge25519_aff ge25519_base_multiples_affine[425] = {
#include "ge25519_base.data"
};

static void ge25519_p2_0(ge25519_p2 *h)
{
  fe25519_0(&h->x);
  fe25519_1(&h->y);
  fe25519_1(&h->z);
}

static void p1p1_to_p2(ge25519_p2 *r, const ge25519_p1p1 *p)
{
  fe25519_mul(&r->x, &p->x, &p->t);
  fe25519_mul(&r->y, &p->y, &p->z);
  fe25519_mul(&r->z, &p->z, &p->t);
}

static void p1p1_to_p3(ge25519_p3 *r, const ge25519_p1p1 *p)
{
  p1p1_to_p2((ge25519_p2 *)r, p);
  fe25519_mul(&r->t, &p->x, &p->y);
}

static void ge25519_madd(ge25519_p1p1 *r, const ge25519_p3 *p, const ge25519_precomp *q)
{
  fe25519 t0;

  fe25519_add(&r->x, &p->y, &p->x);
  fe25519_sub(&r->y, &p->y, &p->x);
  fe25519_mul(&r->z, &r->x, &q->yplusx);
  fe25519_mul(&r->y, &r->x, &q->yminusx);
  fe25519_mul(&r->t, &q->xy2d, &p->t);
  fe25519_add(&t0, &p->z, &p->z);
  fe25519_sub(&r->x, &r->z, &r->y);
  fe25519_add(&r->y, &r->z, &r->y);
  fe25519_add(&r->z, &t0, &r->t);
  fe25519_sub(&r->t, &t0, &r->t);
}

static void ge25519_msub(ge25519_p1p1 *r, const ge25519_p3 *p, const ge25519_precomp *q)
{
  fe25519 t0;

  fe25519_add(&r->x, &p->y, &p->x);
  fe25519_sub(&r->y, &p->y, &p->x);
  fe25519_mul(&r->z, &r->x, &q->yminusx);
  fe25519_mul(&r->y, &r->y, &q->yplusx);
  fe25519_mul(&r->t, &q->xy2d, &p->t);
  fe25519_add(&t0, &p->z, &p->z);
  fe25519_sub(&r->x, &r->z, &r->y);
  fe25519_add(&r->y, &r->z, &r->y);
  fe25519_sub(&r->z, &t0, &r->t);
  fe25519_add(&r->t, &t0, &r->t);
}

static void ge25519_mixadd2(ge25519_p3 *r, const ge25519_aff *q)
{
  fe25519 a,b,t1,t2,c,d,e,f,g,h,qt;
  fe25519_mul(&qt, &q->x, &q->y);
  fe25519_sub(&a, &r->y, &r->x); /* A = (Y1-X1)*(Y2-X2) */
  fe25519_add(&b, &r->y, &r->x); /* B = (Y1+X1)*(Y2+X2) */
  fe25519_sub(&t1, &q->y, &q->x);
  fe25519_add(&t2, &q->y, &q->x);
  fe25519_mul(&a, &a, &t1);
  fe25519_mul(&b, &b, &t2);
  fe25519_sub(&e, &b, &a); /* E = B-A */
  fe25519_add(&h, &b, &a); /* H = B+A */
  fe25519_mul(&c, &r->t, &qt); /* C = T1*k*T2 */
  fe25519_mul(&c, &c, &ge25519_ec2d);
  fe25519_add(&d, &r->z, &r->z); /* D = Z1*2 */
  fe25519_sub(&f, &d, &c); /* F = D-C */
  fe25519_add(&g, &d, &c); /* G = D+C */
  fe25519_mul(&r->x, &e, &f);
  fe25519_mul(&r->y, &h, &g);
  fe25519_mul(&r->z, &g, &f);
  fe25519_mul(&r->t, &e, &h);
}

static void add_p1p1(ge25519_p1p1 *r, const ge25519_p3 *p, const ge25519_p3 *q)
{
  fe25519 a, b, c, d, t;
  
  fe25519_sub(&a, &p->y, &p->x); /* A = (Y1-X1)*(Y2-X2) */
  fe25519_sub(&t, &q->y, &q->x);
  fe25519_mul(&a, &a, &t);
  fe25519_add(&b, &p->x, &p->y); /* B = (Y1+X1)*(Y2+X2) */
  fe25519_add(&t, &q->x, &q->y);
  fe25519_mul(&b, &b, &t);
  fe25519_mul(&c, &p->t, &q->t); /* C = T1*k*T2 */
  fe25519_mul(&c, &c, &ge25519_ec2d);
  fe25519_mul(&d, &p->z, &q->z); /* D = Z1*2*Z2 */
  fe25519_add(&d, &d, &d);
  fe25519_sub(&r->x, &b, &a); /* E = B-A */
  fe25519_sub(&r->t, &d, &c); /* F = D-C */
  fe25519_add(&r->z, &d, &c); /* G = D+C */
  fe25519_add(&r->y, &b, &a); /* H = B+A */
}

/* See http://www.hyperelliptic.org/EFD/g1p/auto-twisted-extended-1.html#doubling-dbl-2008-hwcd */
static void dbl_p1p1(ge25519_p1p1 *r, const ge25519_p2 *p)
{
  fe25519 a,b,c,d;
  fe25519_square(&a, &p->x);
  fe25519_square(&b, &p->y);
  fe25519_square(&c, &p->z);
  fe25519_add(&c, &c, &c);
  fe25519_neg(&d, &a);

  fe25519_add(&r->x, &p->x, &p->y);
  fe25519_square(&r->x, &r->x);
  fe25519_sub(&r->x, &r->x, &a);
  fe25519_sub(&r->x, &r->x, &b);
  fe25519_add(&r->z, &d, &b);
  fe25519_sub(&r->t, &r->z, &c);
  fe25519_sub(&r->y, &d, &b);
}

/* Constant-time version of: if(b) r = p */
static void cmov_aff(ge25519_aff *r, const ge25519_aff *p, unsigned char b)
{
  fe25519_cmov(&r->x, &p->x, b);
  fe25519_cmov(&r->y, &p->y, b);
}

static unsigned char equal(signed char b,signed char c)
{
  unsigned char ub = b;
  unsigned char uc = c;
  unsigned char x = ub ^ uc; /* 0: yes; 1..255: no */
  crypto_uint32 y = x; /* 0: yes; 1..255: no */
  y -= 1; /* 4294967295: yes; 0..254: no */
  y >>= 31; /* 1: yes; 0: no */
  return y;
}

static unsigned char negative(signed char b)
{
  unsigned long long x = b; /* 18446744073709551361..18446744073709551615: yes; 0..255: no */
  x >>= 63; /* 1: yes; 0: no */
  return x;
}

static void choose_t(ge25519_aff *t, unsigned long long pos, signed char b)
{
  /* constant time */
  fe25519 v;
  *t = ge25519_base_multiples_affine[5*pos+0];
  cmov_aff(t, &ge25519_base_multiples_affine[5*pos+1],equal(b,1) | equal(b,-1));
  cmov_aff(t, &ge25519_base_multiples_affine[5*pos+2],equal(b,2) | equal(b,-2));
  cmov_aff(t, &ge25519_base_multiples_affine[5*pos+3],equal(b,3) | equal(b,-3));
  cmov_aff(t, &ge25519_base_multiples_affine[5*pos+4],equal(b,-4));
  fe25519_neg(&v, &t->x);
  fe25519_cmov(&t->x, &v, negative(b));
}

static void setneutral(ge25519 *r)
{
  fe25519_setzero(&r->x);
  fe25519_setone(&r->y);
  fe25519_setone(&r->z);
  fe25519_setzero(&r->t);
}

static void slide_vartime(signed char *r, const unsigned char *a)
{
  int i;
  int b;
  int k;
  int ribs;
  int cmp;

  for (i = 0; i < 256; ++i) {
      r[i] = 1 & (a[i >> 3] >> (i & 7));
  }
  for (i = 0; i < 256; ++i) {
      if (! r[i]) {
          continue;
      }
      for (b = 1; b <= 6 && i + b < 256; ++b) {
          if (! r[i + b]) {
              continue;
          }
          ribs = r[i + b] << b;
          cmp = r[i] + ribs;
          if (cmp <= 15) {
              r[i] = cmp;
              r[i + b] = 0;
          } else {
              cmp = r[i] - ribs;
              if (cmp < -15) {
                  break;
              }
              r[i] = cmp;
              for (k = i + b; k < 256; ++k) {
                  if (! r[k]) {
                      r[k] = 1;
                      break;
                  }
                  r[k] = 0;
              }
          }
      }
  }
}

void ge25519_p3_to_cached(ge25519_cached *r, const ge25519_p3 *p)
{
  fe25519_add(&r->YplusX, &p->y, &p->x);
  fe25519_sub(&r->YminusX, &p->y, &p->x);
  fe25519_copy(&r->Z, &p->z);
  fe25519_mul(&r->T2d, &p->t, &d2);
}

static void ge25519_p3_to_p2(ge25519_p2 *r, const ge25519_p3 *p)
{
  fe25519_copy(&r->x, &p->x);
  fe25519_copy(&r->y, &p->y);
  fe25519_copy(&r->z, &p->z);
}

static void
ge25519_p2_dbl(ge25519_p1p1 *r, const ge25519_p2 *p)
{
  fe25519 t0;

  fe25519_sq(&r->x, &p->x);
  fe25519_sq(&r->z, &p->y);
  fe25519_sq2(&r->t, &p->z);
  fe25519_add(&r->y, &p->x, &p->y);
  fe25519_sq(&t0, &r->y);
  fe25519_add(&r->y, &r->z, &r->x);
  fe25519_sub(&r->z, &r->z, &r->x);
  fe25519_sub(&r->x, &t0, &r->y);
  fe25519_sub(&r->t, &r->t, &r->z);
}

static void ge25519_p3_dbl(ge25519_p1p1 *r, const ge25519_p3 *p)
{
  ge25519_p2 q;
  ge25519_p3_to_p2(&q, p);
  ge25519_p2_dbl(r, &q);
}

/* ********************************************************************
 *                    EXPORTED FUNCTIONS
 ******************************************************************** */

/* return 0 on success, -1 otherwise */
int ge25519_unpackneg_vartime(ge25519_p3 *r, const unsigned char p[32])
{
  unsigned char par;
  fe25519 t, chk, num, den, den2, den4, den6;
  fe25519_setone(&r->z);
  par = p[31] >> 7;
  fe25519_unpack(&r->y, p); 
  fe25519_square(&num, &r->y); /* x = y^2 */
  fe25519_mul(&den, &num, &ge25519_ecd); /* den = dy^2 */
  fe25519_sub(&num, &num, &r->z); /* x = y^2-1 */
  fe25519_add(&den, &r->z, &den); /* den = dy^2+1 */

  /* Computation of sqrt(num/den) */
  /* 1.: computation of num^((p-5)/8)*den^((7p-35)/8) = (num*den^7)^((p-5)/8) */
  fe25519_square(&den2, &den);
  fe25519_square(&den4, &den2);
  fe25519_mul(&den6, &den4, &den2);
  fe25519_mul(&t, &den6, &num);
  fe25519_mul(&t, &t, &den);

  fe25519_pow2523(&t, &t);
  /* 2. computation of r->x = t * num * den^3 */
  fe25519_mul(&t, &t, &num);
  fe25519_mul(&t, &t, &den);
  fe25519_mul(&t, &t, &den);
  fe25519_mul(&r->x, &t, &den);

  /* 3. Check whether sqrt computation gave correct result, multiply by sqrt(-1) if not: */
  fe25519_square(&chk, &r->x);
  fe25519_mul(&chk, &chk, &den);
  if (!fe25519_iseq_vartime(&chk, &num))
    fe25519_mul(&r->x, &r->x, &ge25519_sqrtm1);

  /* 4. Now we have one of the two square roots, except if input was not a square */
  fe25519_square(&chk, &r->x);
  fe25519_mul(&chk, &chk, &den);
  if (!fe25519_iseq_vartime(&chk, &num))
    return -1;

  /* 5. Choose the desired square root according to parity: */
  if(fe25519_getparity(&r->x) != (1-par))
    fe25519_neg(&r->x, &r->x);

  fe25519_mul(&r->t, &r->x, &r->y);
  return 0;
}

void ge25519_pack(unsigned char r[32], const ge25519_p3 *p)
{
  fe25519 tx, ty, zi;
  fe25519_invert(&zi, &p->z); 
  fe25519_mul(&tx, &p->x, &zi);
  fe25519_mul(&ty, &p->y, &zi);
  fe25519_pack(r, &ty);
  r[31] ^= fe25519_getparity(&tx) << 7;
}

int ge25519_isneutral_vartime(const ge25519_p3 *p)
{
  int ret = 1;
  if(!fe25519_iszero(&p->x)) ret = 0;
  if(!fe25519_iseq_vartime(&p->y, &p->z)) ret = 0;
  return ret;
}

/* computes [s1]p1 + [s2]p2 */
void ge25519_double_scalarmult_vartime(ge25519_p3 *r, const ge25519_p3 *p1, const sc25519 *s1, const ge25519_p3 *p2, const sc25519 *s2)
{
  ge25519_p1p1 tp1p1;
  ge25519_p3 pre[16];
  unsigned char b[127];
  int i;

  /* precomputation                                                        s2 s1 */
  setneutral(pre);                                                      /* 00 00 */
  pre[1] = *p1;                                                         /* 00 01 */
  dbl_p1p1(&tp1p1,(ge25519_p2 *)p1);      p1p1_to_p3( &pre[2], &tp1p1); /* 00 10 */
  add_p1p1(&tp1p1,&pre[1], &pre[2]);      p1p1_to_p3( &pre[3], &tp1p1); /* 00 11 */
  pre[4] = *p2;                                                         /* 01 00 */
  add_p1p1(&tp1p1,&pre[1], &pre[4]);      p1p1_to_p3( &pre[5], &tp1p1); /* 01 01 */
  add_p1p1(&tp1p1,&pre[2], &pre[4]);      p1p1_to_p3( &pre[6], &tp1p1); /* 01 10 */
  add_p1p1(&tp1p1,&pre[3], &pre[4]);      p1p1_to_p3( &pre[7], &tp1p1); /* 01 11 */
  dbl_p1p1(&tp1p1,(ge25519_p2 *)p2);      p1p1_to_p3( &pre[8], &tp1p1); /* 10 00 */
  add_p1p1(&tp1p1,&pre[1], &pre[8]);      p1p1_to_p3( &pre[9], &tp1p1); /* 10 01 */
  dbl_p1p1(&tp1p1,(ge25519_p2 *)&pre[5]); p1p1_to_p3(&pre[10], &tp1p1); /* 10 10 */
  add_p1p1(&tp1p1,&pre[3], &pre[8]);      p1p1_to_p3(&pre[11], &tp1p1); /* 10 11 */
  add_p1p1(&tp1p1,&pre[4], &pre[8]);      p1p1_to_p3(&pre[12], &tp1p1); /* 11 00 */
  add_p1p1(&tp1p1,&pre[1],&pre[12]);      p1p1_to_p3(&pre[13], &tp1p1); /* 11 01 */
  add_p1p1(&tp1p1,&pre[2],&pre[12]);      p1p1_to_p3(&pre[14], &tp1p1); /* 11 10 */
  add_p1p1(&tp1p1,&pre[3],&pre[12]);      p1p1_to_p3(&pre[15], &tp1p1); /* 11 11 */

  sc25519_2interleave2(b,s1,s2);

  /* scalar multiplication */
  *r = pre[b[126]];
  for(i=125;i>=0;i--)
  {
    dbl_p1p1(&tp1p1, (ge25519_p2 *)r);
    p1p1_to_p2((ge25519_p2 *) r, &tp1p1);
    dbl_p1p1(&tp1p1, (ge25519_p2 *)r);
    if(b[i]!=0)
    {
      p1p1_to_p3(r, &tp1p1);
      add_p1p1(&tp1p1, r, &pre[b[i]]);
    }
    if(i != 0) p1p1_to_p2((ge25519_p2 *)r, &tp1p1);
    else p1p1_to_p3(r, &tp1p1);
  }
}

void ge25519_double_scalarmult_vartime_new(ge25519_p2 *r, const unsigned char *a,
                                  const ge25519_p3 *A, const unsigned char *b)
{
  static const ge25519_precomp Bi[8] = {
# include "base2.h"
  };
  signed char    aslide[256];
  signed char    bslide[256];
  ge25519_cached Ai[8]; /* A,3A,5A,7A,9A,11A,13A,15A */
  ge25519_p1p1   t;
  ge25519_p3     u;
  ge25519_p3     A2;
  int            i;

  slide_vartime(aslide, a);
  slide_vartime(bslide, b);

  ge25519_p3_to_cached(&Ai[0], A);

  ge25519_p3_dbl(&t, A);
  p1p1_to_p3(&A2, &t);

  ge25519_add(&t, &A2, &Ai[0]);
  p1p1_to_p3(&u, &t);
  ge25519_p3_to_cached(&Ai[1], &u);

  ge25519_add(&t, &A2, &Ai[1]);
  p1p1_to_p3(&u, &t);
  ge25519_p3_to_cached(&Ai[2], &u);

  ge25519_add(&t, &A2, &Ai[2]);
  p1p1_to_p3(&u, &t);
  ge25519_p3_to_cached(&Ai[3], &u);

  ge25519_add(&t, &A2, &Ai[3]);
  p1p1_to_p3(&u, &t);
  ge25519_p3_to_cached(&Ai[4], &u);

  ge25519_add(&t, &A2, &Ai[4]);
  p1p1_to_p3(&u, &t);
  ge25519_p3_to_cached(&Ai[5], &u);

  ge25519_add(&t, &A2, &Ai[5]);
  p1p1_to_p3(&u, &t);
  ge25519_p3_to_cached(&Ai[6], &u);

  ge25519_add(&t, &A2, &Ai[6]);
  p1p1_to_p3(&u, &t);
  ge25519_p3_to_cached(&Ai[7], &u);

  ge25519_p2_0(r);

  for (i = 255; i >= 0; --i) {
      if (aslide[i] || bslide[i]) {
          break;
      }
  }

  for (; i >= 0; --i) {
      ge25519_p2_dbl(&t, r);

      if (aslide[i] > 0) {
          p1p1_to_p3(&u, &t);
          ge25519_add(&t, &u, &Ai[aslide[i] / 2]);
      } else if (aslide[i] < 0) {
          p1p1_to_p3(&u, &t);
          ge25519_sub(&t, &u, &Ai[(-aslide[i]) / 2]);
      }

      if (bslide[i] > 0) {
          p1p1_to_p3(&u, &t);
          ge25519_madd(&t, &u, &Bi[bslide[i] / 2]);
      } else if (bslide[i] < 0) {
          p1p1_to_p3(&u, &t);
          ge25519_msub(&t, &u, &Bi[(-bslide[i]) / 2]);
      }

      p1p1_to_p2(r, &t);
  }
}

void ge25519_scalarmult_base(ge25519_p3 *r, const sc25519 *s)
{
  signed char b[85];
  int i;
  ge25519_aff t;
  sc25519_window3(b,s);

  choose_t((ge25519_aff *)r, 0, b[0]);
  fe25519_setone(&r->z);
  fe25519_mul(&r->t, &r->x, &r->y);
  for(i=1;i<85;i++)
  {
    choose_t(&t, (unsigned long long) i, b[i]);
    ge25519_mixadd2(r, &t);
  }
}

void ge25519_add(ge25519_p1p1 *r, const ge25519_p3 *p, const ge25519_cached *q)
{
  fe25519 t0;

  fe25519_add(&r->x, &p->y, &p->x);
  fe25519_sub(&r->y, &p->y, &p->x);
  fe25519_mul(&r->z, &r->x, &q->YplusX);
  fe25519_mul(&r->y, &r->y, &q->YminusX);
  fe25519_mul(&r->t, &q->T2d, &p->t);
  fe25519_mul(&r->x, &p->z, &q->Z);
  fe25519_add(&t0, &r->x, &r->x);
  fe25519_sub(&r->x, &r->z, &r->y);
  fe25519_add(&r->y, &r->z, &r->y);
  fe25519_add(&r->z, &t0, &r->t);
  fe25519_sub(&r->t, &t0, &r->t);
}

void ge25519_sub(ge25519_p1p1 *r, const ge25519_p3 *p, const ge25519_cached *q)
{
  fe25519 t0;

  fe25519_add(&r->x, &p->y, &p->x);
  fe25519_sub(&r->y, &p->y, &p->x);
  fe25519_mul(&r->z, &r->x, &q->YminusX);
  fe25519_mul(&r->y, &r->y, &q->YplusX);
  fe25519_mul(&r->t, &q->T2d, &p->t);
  fe25519_mul(&r->x, &p->z, &q->Z);
  fe25519_add(&t0, &r->x, &r->x);
  fe25519_sub(&r->x, &r->z, &r->y);
  fe25519_add(&r->y, &r->z, &r->y);
  fe25519_sub(&r->z, &t0, &r->t);
  fe25519_add(&r->t, &t0, &r->t);
}

int ge25519_has_small_order(const unsigned char s[32])
{
  CRYPTO_ALIGN(16)
  static const unsigned char blacklist[][32] = {
      /* 0 (order 4) */
      { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
      /* 1 (order 1) */
      { 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
      /* 2707385501144840649318225287225658788936804267575313519463743609750303402022
         (order 8) */
      { 0x26, 0xe8, 0x95, 0x8f, 0xc2, 0xb2, 0x27, 0xb0, 0x45, 0xc3, 0xf4,
        0x89, 0xf2, 0xef, 0x98, 0xf0, 0xd5, 0xdf, 0xac, 0x05, 0xd3, 0xc6,
        0x33, 0x39, 0xb1, 0x38, 0x02, 0x88, 0x6d, 0x53, 0xfc, 0x05 },
      /* 55188659117513257062467267217118295137698188065244968500265048394206261417927
         (order 8) */
      { 0xc7, 0x17, 0x6a, 0x70, 0x3d, 0x4d, 0xd8, 0x4f, 0xba, 0x3c, 0x0b,
        0x76, 0x0d, 0x10, 0x67, 0x0f, 0x2a, 0x20, 0x53, 0xfa, 0x2c, 0x39,
        0xcc, 0xc6, 0x4e, 0xc7, 0xfd, 0x77, 0x92, 0xac, 0x03, 0x7a },
      /* p-1 (order 2) */
      { 0xec, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f },
      /* p (=0, order 4) */
      { 0xed, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f },
      /* p+1 (=1, order 1) */
      { 0xee, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f }
  };
  unsigned char c[7] = { 0 };
  unsigned int  k;
  size_t        i, j;

  COMPILER_ASSERT(7 == sizeof blacklist / sizeof blacklist[0]);
  for (j = 0; j < 31; j++) {
      for (i = 0; i < sizeof blacklist / sizeof blacklist[0]; i++) {
          c[i] |= s[j] ^ blacklist[i][j];
      }
  }
  for (i = 0; i < sizeof blacklist / sizeof blacklist[0]; i++) {
      c[i] |= (s[j] & 0x7f) ^ blacklist[i][j];
  }
  k = 0;
  for (i = 0; i < sizeof blacklist / sizeof blacklist[0]; i++) {
      k |= (c[i] - 1);
  }
  return (int) ((k >> 8) & 1);
}

int ge25519_is_canonical(const unsigned char *s)
{
  unsigned char c;
  unsigned char d;
  unsigned int  i;

  c = (s[31] & 0x7f) ^ 0x7f;
  for (i = 30; i > 0; i--) {
      c |= s[i] ^ 0xff;
  }
  c = (((unsigned int) c) - 1U) >> 8;
  d = (0xed - 1U - (unsigned int) s[0]) >> 8;

  return 1 - (c & d & 1);
}

int ge25519_frombytes_negate_vartime(ge25519_p3 *h, const unsigned char *s)
{
  fe25519 u;
  fe25519 v;
  fe25519 v3;
  fe25519 vxx;
  fe25519 m_root_check, p_root_check;

  fe25519_frombytes(&h->y, s);
  fe25519_1(&h->z);
  fe25519_sq(&u, &h->y);
  fe25519_mul(&v, &u, &d);
  fe25519_sub(&u, &u, &h->z); /* u = y^2-1 */
  fe25519_add(&v, &v, &h->z); /* v = dy^2+1 */

  fe25519_sq(&v3, &v);
  fe25519_mul(&v3, &v3, &v); /* v3 = v^3 */
  fe25519_sq(&h->x, &v3);
  fe25519_mul(&h->x, &h->x, &v);
  fe25519_mul(&h->x, &h->x, &u); /* x = uv^7 */

  fe25519_pow22523(&h->x, &h->x); /* x = (uv^7)^((q-5)/8) */
  fe25519_mul(&h->x, &h->x, &v3);
  fe25519_mul(&h->x, &h->x, &u); /* x = uv^3(uv^7)^((q-5)/8) */

  fe25519_sq(&vxx, &h->x);
  fe25519_mul(&vxx, &vxx, &v);
  fe25519_sub(&m_root_check, &vxx, &u); /* vx^2-u */
  if (fe25519_iszero(&m_root_check) == 0) {
      fe25519_add(&p_root_check, &vxx, &u); /* vx^2+u */
      if (fe25519_iszero(&p_root_check) == 0) {
          return -1;
      }
      fe25519_mul(&h->x, &h->x, &sqrtm1);
  }

  if (fe25519_isnegative(&h->x) == (s[31] >> 7)) {
      fe25519_neg(&h->x, &h->x);
  }
  fe25519_mul(&h->t, &h->x, &h->y);

  return 0;
}

void ge25519_tobytes(unsigned char *s, const ge25519_p2 *h)
{
  fe25519 recip;
  fe25519 x;
  fe25519 y;

  fe25519_invert(&recip, &h->z);
  fe25519_mul(&x, &h->x, &recip);
  fe25519_mul(&y, &h->y, &recip);
  fe25519_tobytes(s, &y);
  s[31] ^= fe25519_isnegative(&x) << 7;
}

void ge25519_p3_tobytes(unsigned char *s, const ge25519_p3 *h)
{
    fe25519 recip;
    fe25519 x;
    fe25519 y;

    fe25519_invert(&recip, &h->z);
    fe25519_mul(&x, &h->x, &recip);
    fe25519_mul(&y, &h->y, &recip);
    fe25519_tobytes(s, &y);
    s[31] ^= fe25519_isnegative(&x) << 7;
}
