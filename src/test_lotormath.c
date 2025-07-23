// Auth: smurfd, 2025 More reading at the bottom of the file; 2 spacs indent; 150 width                                                             //
// ------------------------------------------------------------------------------------------------------------------------------------------------ //
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include "lotormath.h"

void tester_bint_sanity(void) {
  bint a = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, b = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, c = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint d = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  str2bint(&a, "0x3b6859c358bb6fa30b3f11ff6c29164dc21b2abaf4c2027ea8e6701e99dd5b7c");
  str2bint(&b, "0xd7dab791a8647ac88695e20e29960a6fd41707258cc88cc0480ea7e5bb3f132f");
  bmul(&c, &a, &b);
  // 0x321764d8e538429bc4539ec57462be61fa493fd0c700db4f19c51549d9c6aad6904bf1b900775bdf11da25e74e4d698c386446a85b7f1afaf9062442c4f1ffc4
  uint32_t xxx[BLEN] = {0x321764d8, 0xe538429b, 0xc4539ec5, 0x7462be61, 0xfa493fd0, 0xc700db4f, 0x19c51549, 0xd9c6aad6, 0x904bf1b9, 0x00775bdf,
    0x11da25e7, 0x4e4d698c, 0x386446a8, 0x5b7f1afa, 0xf9062442, 0xc4f1ffc4};
  for (int i = 0; i < 15; i++) {assert(c.wrd[i] == xxx[15-i]);}
  bmul(&d, &c, &a);
  // 0xb9fcf4f502f06b563974c8fd64b52750b925ec4138d28632fc84b3f8eedf7c7d471a809102365772e1eefc491b090a624584b32a122142b07a6c3aefc791d81cf527e4053ea119ffa86f73189e82d3ae2650192831a8fc8618fac175b568ef0
  uint32_t yyy[BLEN] = {0x0b9fcf4f, 0x502f06b5, 0x63974c8f, 0xd64b5275, 0x0b925ec4, 0x138d2863, 0x2fc84b3f, 0x8eedf7c7, 0xd471a809, 0x10236577,
    0x2e1eefc4, 0x91b090a6, 0x24584b32, 0xa122142b, 0x07a6c3ae, 0xfc791d81, 0xcf527e40, 0x53ea119f, 0xfa86f731, 0x89e82d3a, 0xe2650192, 0x831a8fc8,
    0x618fac17, 0x5b568ef0};
  for (int i = 0; i < 24; i++) assert(yyy[23 - i] == d.wrd[i]);
}

void tester_bint_div_sanity(void) {
  bint a = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, b = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, c = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint tmp = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, one = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  str2bint(&a, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  str2bint(&b, "0x9075b4ee4d4788cabb49f7f81c221151fa2f68914d0aa833388fa11ff621a970");
  bdiv(&c, &tmp, &a, &b);
  wrd2bint(&one, 1);
  assert(cmp(&c, &one) == 0);
}

void tester_bint_mod_sanity(void) {
  bint a = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, b = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, c = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint tmp = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, ret = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, r1 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint tmp2 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, r2 = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  str2bint(&a, "0x34ca4d7bd7efe5cfc1462edca66d539c0a77a83d09ce1196c92c514312232a7e");
  str2bint(&c, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  str2bint(&b, "0x9ce3bc8a331e7860fe957feaab820c680af219522713db054053fdc5b41f424d");
  str2bint(&r1, "0x32339163b1c163c5132aeb93cffbd83fc98b2045f072b3772675ca04684cbf67");
  str2bint(&r2, "0xcdcc6e9c4e3e9c3aecd5146c300427c03674dfba0f8d4c88d98a35fa97b33cc8");
  a.neg = 0;
  bmul(&tmp, &a, &b);
  bmod(&ret, &tmp2, &tmp, &c);
  assert(cmp(&ret, &r1) == 0);
  a.neg = 1;
  bmul(&tmp, &a, &b);
  bmod(&ret, &tmp2, &tmp, &c);
  assert(cmp(&ret, &r2) == 0);
}

void tester_bint_2ways_sanity(void) {
  bint a = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, b = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, c = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  bint d = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, e = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, t = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
  str2bint(&a, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  wrd2bint(&b, 0x33333333);
  wrd2bint(&c, 0x3);
  wrd2bint(&d, 0x33);
  for (int i = 0; i < 20000; i++) bsub(&a, &a, &b);
  for (int i = 0; i < 20000; i++) badd(&a, &a, &b);
  assert(cmp(str2bint(&b, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f"), &a) == 0);
  for (int i = 0; i < 200; i++) {
    bmul(&e, &d, &c);
    memcpy(&d.wrd, e.wrd, e.siz * sizeof(uint32_t));
    d.siz = e.siz;
  }
  for (int i = 0; i < 200; i++) {
    bint ee = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1}, tt = {.wrd = {0}, .siz = 1, .neg = 0, .cap = 1};
    bdiv(&ee, &tt, &d, &c);
    memcpy(&d, &ee, sizeof(bint));
  }
  assert(cmp(wrd2bint(&e, 0x33), &d) == 0);
  memset(a.wrd, 0, BLEN * sizeof(uint32_t));
  memset(d.wrd, 0, BLEN * sizeof(uint32_t));
  memset(e.wrd, 0, BLEN * sizeof(uint32_t));
  memset(t.wrd, 0, BLEN * sizeof(uint32_t));
  a.neg = d.neg = e.neg = t.neg = a.cap = d.cap = e.cap = t.cap = 0;
  a.siz = d.siz = e.siz = t.siz = 1;
  str2bint(&a, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  str2bint(&d, "0x0000000000000000000000000000000000000000000000000000003333333333");
  bdiv(&e, &t, &a, &d);
  str2bint(&d, "0x000000000500000000050000000005000000000500000000050000000004ffff");
  assert(cmp(&d, &e) == 0);
}

void tester_speed(void) {
  clock_t start = clock();
  for (int i = 0; i < 1000000; i++) {
  bint a = bcreate(), b = bcreate(), c = bcreate(), t = bcreate();
  str2bint(&a, "0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffefffffc2f");
  str2bint(&b, "0x0000000000000000000000000000000000000000000000000000003333333333");
  //for (int i = 0; i < 1000000; i++) {
    //badd(&c, &a, &b);
    bdiv(&c, &t, &a, &b);
  }
  printf("bint speed: Time %us %ums\n", (uint32_t)((clock() - start) * 1000 / CLOCKS_PER_SEC) / 1000, (uint32_t)((clock() - start) * 1000 / CLOCKS_PER_SEC) % 1000);
}

void tester_speed_sanity(void) {
  clock_t start = clock();
  for (int i = 0; i < 1000000; i++) {
  }
  printf("bint speed sanity: Time %us %ums\n", (uint32_t)((clock() - start) * 1000 / CLOCKS_PER_SEC) / 1000, (uint32_t)((clock() - start) * 1000 / CLOCKS_PER_SEC) % 1000);
}

int main(void) {
  tester_bint_sanity();
  tester_bint_2ways_sanity();
  tester_bint_div_sanity();
  tester_bint_mod_sanity();
  tester_speed_sanity();
  tester_speed();
  printf("ok\n");
}

