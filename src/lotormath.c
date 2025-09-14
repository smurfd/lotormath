// ------------------------------------------------------------------------------------------------------------------------------------------------ //
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "lotormath.h"

// ----- Print function -----
void bprint(char *s, bint *a) {
  printf("%s: %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x %08x: %d : neg? %d\n",
    s, a->wrd[19], a->wrd[18], a->wrd[17], a->wrd[16], a->wrd[15], a->wrd[14], a->wrd[13], a->wrd[12], a->wrd[11], a->wrd[10], a->wrd[9], a->wrd[8],
    a->wrd[7], a->wrd[6], a->wrd[5], a->wrd[4], a->wrd[3], a->wrd[2], a->wrd[1], a->wrd[0], a->siz, a->neg);
}

// ----- Helper functions -----
static inline uint32_t uint32_lo(uint32_t a) {
  return ((a) & ((uint32_t) - 1) >> 16);
}

static inline uint32_t uint32_hi(uint32_t a) {
  return ((a) >> sizeof(uint32_t) * 4);
}

static inline int16_t uint32_tru(const uint32_t *a, int16_t an) {
  while (an > 0 && a[an - 1] == 0) an--;
  return an;
}

static inline int16_t uint32_abs(const uint32_t *a, const uint32_t *b, int16_t an, int16_t bn) {
  if (an > bn) return +1;
  if (an < bn) return -1;
  for (int8_t i = an - 1; i >= 0; i--) {
    if (a[i] < b[i]) return -1;
    if (a[i] > b[i]) return +1;
  }
  return 0; // Equal
}

int16_t cmp(const bint *a, const bint *b) {
  return uint32_abs(a->wrd, b->wrd, a->siz, b->siz);
}

bint *breserve(bint *a, int cap) {
  if (a->cap >= cap) return a;
  a->cap = cap;
  return a;
}

static inline int16_t count_zeros(bint *a) {
  int8_t co = a->siz - 1;
  while(a->wrd[co] == 0 && co > 0) co--;
  return a->siz - co - 1;
}

bint bcreate(void) {
  bint x = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}; return x;
}


// ----- Helper convert functions -----
static inline uint8_t chr2uint(char *str, int8_t k) {
  return (str[k] < 'a' ? str[k] - '0' : str[k] - 'a' + 10);
}

static inline uint16_t bit(int8_t k) {
  return 7 - (k >> 3);
}

static inline uint32_t bitmod(uint16_t a, int8_t k) {
  return a << (4 * ((7 - k) & 0x7));
}

bint *str2bint(bint *x, const char *str) {
  memset(x->wrd, 0, BZ);
  char *s = (char*)str + 2; // skip 0x from hex string, ie +2
  for (int8_t i = 0; i < 64; i += 4) {
    int8_t i0 = i + 0, i1 = i + 1, i2 = i + 2, i3 = i + 3;
    uint16_t val0 = chr2uint(s, i0), val1 = chr2uint(s, i1), val2 = chr2uint(s, i2), val3 = chr2uint(s, i3);
    x->wrd[bit(i0)] |= bitmod(val0, i0);
    x->wrd[bit(i1)] |= bitmod(val1, i1);
    x->wrd[bit(i2)] |= bitmod(val2, i2);
    x->wrd[bit(i3)] |= bitmod(val3, i3);
  }
  x->siz = strlen(s) / 8;
  x->neg = 0;
  x->cap = x->siz;
  x->siz -= count_zeros(x);
  return x;
}

bint *wrd2bint(bint *x, const uint32_t w) {
  memset(x->wrd, 0, BZ);
  breserve(x, 1);
  x->wrd[0] = w;
  x->siz = 1; // always set size to 1, even if we set it to zero value
  x->neg = 0;
  x->cap = 1;
  return x;
}

// Add and get Carry & Sub and get Carry
#define AC(c, r, a) {r = (c + a); c = (r < a);}
#define SC(c, r, a) {r = (a - c); c = (r > a);}
#define ACC(c, r, a, b, r2) {AC(c, r, a); uint32_t rt = (r + b); c+= (rt < b); r2 = rt;}
#define SCC(c, r, a, b, r2) {SC(c, r, a); uint32_t rt = (r - b); c+= (rt > r); r2 = rt;}
// ----- Math Add & Sub functions -----
static inline int16_t uint32_add(uint32_t *ret, const uint32_t *a, const uint32_t *b, int16_t an, int16_t bn) {
  uint32_t carry = 0, n = an < bn ? an : bn, sum;
  for (uint8_t i = 0; i < n; i++) {
    ACC(carry, sum, a[i], b[i], ret[i]);
  }
  for (uint8_t i = n; i < an; i++) {
    AC(carry, ret[i], a[i]);
  }
  for (uint8_t i = an; i < bn; i++) {
    AC(carry, ret[i], b[i]);
  }
  uint8_t co = bn > an ? bn : an; // biggest of bn and an
  if (carry) ret[co++] = carry;
  return uint32_tru(ret, co);
}

static inline int16_t uint32_sub(uint32_t *ret, const uint32_t *a, const uint32_t *b, int16_t an, int16_t bn) {
  uint32_t carry = 0, dif = 0;
  for (uint8_t i = 0; i < bn; i++) {
    SCC(carry, dif, a[i], b[i], ret[i]);
  }
  for (uint8_t i = bn; i < an; i++) {
    SC(carry, ret[i], a[i]);
  }
  return uint32_tru(ret, an > bn ? an : bn); // biggest of an and bn
}

static inline int16_t uint32_addsigned(uint32_t *ret, int16_t *ret_neg, const uint32_t *a, const int16_t an, const int16_t a_neg,
  const uint32_t *b, const int16_t bn, const int16_t b_neg) {
  if (a_neg) {
    *ret_neg = 1;
    if (b_neg) {
      if (an >= bn) return uint32_add(ret, a, b, an, bn);
      return uint32_add(ret, b, a, bn, an);
    }
    if (uint32_abs(a, b, an, bn) >= 0) return uint32_sub(ret, a, b, an, bn);
    *ret_neg = 0; return uint32_sub(ret, b, a, bn, an);
  }
  *ret_neg = 0;
  if (b_neg) {
    if (uint32_abs(a, b, an, bn) >= 0) return uint32_sub(ret, a, b, an, bn);
    *ret_neg = 1; return uint32_sub(ret, b, a, bn, an);
  }
  if (an >= bn) return uint32_add(ret, a, b, an, bn);
  return uint32_add(ret, b, a, bn, an);
}

bint *badd(bint *ret, const bint *a, const bint *b) {
  ret->siz = uint32_addsigned(ret->wrd, &ret->neg, a->wrd, a->siz, a->neg, b->wrd, b->siz, b->neg);
  return ret;
}

bint *bsub(bint *ret, const bint *a, const bint *b) {
  ret->siz = uint32_addsigned(ret->wrd, &ret->neg, a->wrd, a->siz, a->neg, b->wrd, b->siz, !b->neg);
  return ret;
}


// ----- Math Shift functions -----
bint *blshift(bint *ret, const bint *a, const uint32_t b) {
  int8_t ws = b / 32, bs = b % 32;
  if (bs) {
    uint32_t lo, hi = 0;
    for (int8_t i = a->siz + ws; i > ws; i--) {
      lo = a->wrd[i - ws - 1];
      ret->wrd[i] = (hi << bs) | (lo >> (32 - bs));
      hi = lo;
    }
    memset(ret->wrd, 0, (ws) * sizeof(uint32_t));
    ret->wrd[ws] = hi << bs;
    ret->neg = ret->neg ^ a->neg;
    ret->siz = uint32_tru(ret->wrd, a->siz + ws + 1);
  } else {
    memcpy(ret->wrd + ws, a->wrd, a->siz * sizeof(uint32_t));
    memset(ret->wrd, 0, sizeof(uint32_t) * (ws));
    ret->neg = ret->neg ^ a->neg;
    ret->siz = uint32_tru(ret->wrd, a->siz + ws);
  }
  return ret;
}

bint *brshift(bint *ret, const bint *a, const uint32_t b) {
  int8_t ws = b / 32, bs = b % 32, i = 0;
  if (bs) {
    uint32_t hi, lo = a->wrd[ws];
    for (i = 0; i < a->siz - ws - 1; i++) {
      hi = a->wrd[i + ws + 1];
      ret->wrd[i] = (hi << (32 - bs)) | (lo >> bs);
      lo = hi;
    }
    ret->wrd[i++] = lo >> bs;
    ret->neg = ret->neg ^ a->neg;
    ret->siz = uint32_tru(ret->wrd, i);
  } else {
    memcpy(ret->wrd, a->wrd + ws, (a->siz-ws) * sizeof(uint32_t));
    ret->neg = ret->neg ^ a->neg;
    ret->siz = uint32_tru(ret->wrd, a->siz - ws);
  }
  return ret;
}


bint *brshift1(bint *ret, const bint *a) {
  int8_t i = 0;
  uint32_t hi, lo = a->wrd[0];
  for (i = 0; i < a->siz - 1; i++) {
    hi = a->wrd[i + 1];
    ret->wrd[i] = (hi << (32 - 1)) | (lo >> 1);
    lo = hi;
  }
  ret->wrd[i++] = lo >> 1;
  ret->neg = ret->neg ^ a->neg;
  ret->siz = uint32_tru(ret->wrd, i);
  return ret;
}
// ----- Bit functions -----
int16_t bbitlen(const bint *a) {
  int8_t last = a->siz - 1, ret = 0;
  if (last < 0) return 0;
  for (int8_t i = 32 - 1; i >= 0; i--) {
    if ((a->wrd[last] >> i) & 1) {
      ret = i + 1;
      break;
    }
  }
  return ret + (last * 32);
}

bint *bsetbit(bint *a, const uint32_t i) {
  int8_t wi = i / 32, n = wi + 1;
  breserve(a, n);
  if (a->siz < n) memset(a->wrd + a->siz, 0, (n - a->siz) * sizeof(uint32_t));
  a->siz = a->siz > n ? a->siz : n;
  a->wrd[wi] |= (((uint32_t)1) << (i % 32));
  return a;
}


// ----- Math Mul functions -----
static inline uint32_t uint32_mul_hi(const uint32_t a, const uint32_t b) {
  uint32_t la = uint32_lo(a), lb = uint32_lo(b), ha = uint32_hi(a), hb = uint32_hi(b);
  uint32_t r0 = la * lb, r1 = la * hb, r2 = ha * lb, r3 = ha * hb;
  uint32_t r4 = uint32_hi(r0) + uint32_lo(r1) + uint32_lo(r2);
  return uint32_hi(r4) + uint32_hi(r1) + uint32_hi(r2) + r3;
}

#define GETCAR(sw, dw, c, r, a, f) {sw = a; dw = sw * f; dw += c; c = (dw < c); c+=uint32_mul_hi(sw, f); c +=((r+=dw) < dw);}
static inline int16_t uint32_mul_add(uint32_t *ret, const uint32_t *a, const uint32_t *b, int16_t an, int16_t bn) {
  if (an == 0 || bn == 0) return 0;
  for (int8_t id = an % 16, j = 0; j < bn; j++) {
    uint32_t carry = 0, n = an, f = b[j], r[BLEN] = {0}, src_word = 0, dst_word = 0;
    memcpy(r, ret + j, (an + bn) * sizeof(uint32_t));
    for (uint32_t i = 0; i < n - id; i+=16) {
      GETCAR(src_word, dst_word, carry, r[i+0], a[i+0], f);
      GETCAR(src_word, dst_word, carry, r[i+1], a[i+1], f);
      GETCAR(src_word, dst_word, carry, r[i+2], a[i+2], f);
      GETCAR(src_word, dst_word, carry, r[i+3], a[i+3], f);
      GETCAR(src_word, dst_word, carry, r[i+4], a[i+4], f);
      GETCAR(src_word, dst_word, carry, r[i+5], a[i+5], f);
      GETCAR(src_word, dst_word, carry, r[i+6], a[i+6], f);
      GETCAR(src_word, dst_word, carry, r[i+7], a[i+7], f);
      GETCAR(src_word, dst_word, carry, r[i+8], a[i+8], f);
      GETCAR(src_word, dst_word, carry, r[i+9], a[i+9], f);
      GETCAR(src_word, dst_word, carry, r[i+10], a[i+10], f);
      GETCAR(src_word, dst_word, carry, r[i+11], a[i+11], f);
      GETCAR(src_word, dst_word, carry, r[i+12], a[i+12], f);
      GETCAR(src_word, dst_word, carry, r[i+13], a[i+13], f);
      GETCAR(src_word, dst_word, carry, r[i+14], a[i+14], f);
      GETCAR(src_word, dst_word, carry, r[i+15], a[i+15], f);
    }
    for (uint32_t i = n - id; i < n; i++) {
      GETCAR(src_word, dst_word, carry, r[i+0], a[i+0], f);
    }
    while((carry = (r[n++] += carry) < carry));
    memcpy(ret + j, r, (an + bn) * sizeof(uint32_t));
  }
  return uint32_tru(ret, an + bn);
}

// TODO: this must be optimizable
// https://en.wikipedia.org/wiki/Karatsuba_algorithm
static inline int16_t uint32_mul_karatsuba(uint32_t *ret, const uint32_t *a, const uint32_t *b, int16_t an, int16_t bn, uint32_t *tmp) {
  if (an <= 16  && bn <= 16) return uint32_mul_add(ret, a, b, an, bn);
  int16_t m = an > bn? an : bn, m2 = m / 2, m3 = m2 + 2, nlo1hi1, nlo2hi2, nz0, nz1, nz2;
  uint32_t *lo1 = (uint32_t*)a, *lo2 = (uint32_t*)b, *hi1 = (uint32_t*)a + m2, *hi2 = (uint32_t*)b + m2, *lo1hi1, *lo2hi2, *z0, *z1, *z2;
  int16_t nlo1 = uint32_tru(lo1, m2 < an? m2 : an), nlo2 = uint32_tru(lo2, m2 < bn? m2 : bn);
  int16_t nhi1 = uint32_tru(hi1, (an-m2) > 0 ? (an-m2) : 0), nhi2 = uint32_tru(hi2, (bn-m2) > 0 ? (bn-m2) : 0);
  lo1hi1 = tmp; tmp += m3; lo2hi2 = tmp; tmp += m3;
  z0 = tmp; tmp += m3 * 2; z1 = tmp; tmp += m3 * 2; z2 = tmp; tmp += m3 * 2;
  nlo1hi1 = uint32_add(lo1hi1, lo1, hi1, nlo1, nhi1); nlo2hi2 = uint32_add(lo2hi2, lo2, hi2, nlo2, nhi2);
  nz0 = uint32_mul_karatsuba(z0, lo1, lo2, nlo1, nlo2, tmp);
  nz1 = uint32_mul_karatsuba(z1, lo1hi1, lo2hi2, nlo1hi1, nlo2hi2, tmp);
  nz2 = uint32_mul_karatsuba(z2, hi1, hi2, nhi1, nhi2, tmp);
  nz1 = uint32_sub(z1, z1, z0, nz1, nz0);
  nz1 = uint32_sub(z1, z1, z2, nz1, nz2);
  memcpy(ret, z0, nz0 * sizeof(uint32_t));
  nz0 = uint32_add(ret + m2 * 1, ret + m2 * 1, z1, (nz0-m2) > 0 ? (nz0-m2) : 0, nz1);
  nz0 = uint32_add(ret + m2 * 2, ret + m2 * 2, z2, (nz0-m2) > 0 ? (nz0-m2) : 0, nz2);
  return uint32_tru(ret, nz0 + m2 * 2);

}

bint *bmul(bint *ret, const bint *a, const bint *b) {
  int8_t an = a->siz, bn = b->siz, n = an+bn;
  memset(ret->wrd, 0, BZ);
  if (ret->wrd != a->wrd && ret->wrd != b->wrd && an < BLEN && bn < BLEN) {
    breserve(ret, n);
    ret->siz = uint32_mul_add(ret->wrd, a->wrd, b->wrd, an, bn);
  } else {
    uint32_t tmp[BLEN] = {0};
    breserve(ret, n);
    ret->siz = uint32_mul_karatsuba(ret->wrd, a->wrd, b->wrd, an, bn, tmp+n);
  }
  ret->neg = a->neg ^ b->neg;
  ret->cap = b->cap;
  return ret;
}


// ----- Div functions ----
static inline bint *bdivmod(bint *ret, bint *rem, const bint *a, const bint *d) {
  if (d->siz == 0) {
    printf("Division by zero, not good\n"); return NULL; // this should never happen
  } else if (a->siz == 1 && d->siz == 1) {
    uint32_t aw = a->wrd[0], bw = d->wrd[0];
    wrd2bint(ret, aw / bw);
    wrd2bint(rem, aw % bw);
    ret->neg = a->neg;
    rem->neg = a->neg ^ d->neg;
    return ret;
  }
  BCPY(*(bint*)rem, *(bint*)a);
  rem->siz = a->siz;
  rem->neg = 0;
  if (uint32_abs(rem->wrd, d->wrd, rem->siz, d->siz) >= 0) {
    bint den = bcreate();
    BCPY(den, *(bint*)d);
    int16_t sh = bbitlen(rem) - bbitlen(&den);
    blshift(&den, &den, sh);
    den.neg = 0;
    for (int16_t s = sh; s >= 0; s--) {
      if (uint32_abs(rem->wrd, den.wrd, rem->siz, den.siz) >= 0) {
        bsub(rem, rem, &den);
        bsetbit(ret, s);
      }
      brshift1(&den, &den);
    }
  }
  rem->neg = a->neg;
  ret->neg = a->neg ^ d->neg;
  return ret;
}

bint *bdiv(bint *ret, bint *tmp, const bint *a, const bint *d) {
  memset(ret->wrd, 0, BZ);
  bdivmod(ret, tmp, a, d);
  return tmp;
}

bint *bmod(bint *ret, bint *tmp, const bint *a, const bint *m) {
  bint t1 = bcreate(), t3 = bcreate(), ret1 = bcreate();
  bdivmod(tmp, &ret1, a, m);
  badd(&t3, &ret1, m);
  bdivmod(&t1, ret, &t3, m);
  return ret;
}

//
// Borrowed / stolen / rewritten from https://github.com/983/bigint/

