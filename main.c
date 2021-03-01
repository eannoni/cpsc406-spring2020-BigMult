#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h> /* declarations of uintX_t, etc */
#include <limits.h>

// computes: as += bs
uint32_t addto32(uint32_t as[], int sz_a, uint32_t bs[], int sz_b) { // Assume that sz_b <= sz_a
  // Compute as += bs, where sz_b < sz_a because this is how we're going to add things in different places and the thing we're adding is typically 2 digit numbers (we need this for adding columns)

  int i;
  uint32_t c = 0;
  uint64_t s;
  
  // initialization happens once, at the beginning of the loop, i=0
  // after, check to see if the condition (i < sz_b) is met
  // then, it executes the body of the loop
  // when the body of the loop is finished executing (where the end of the '}') it will then increment i and goes back to see if the condition was met
  for (i = 0; i < sz_b; i++) {
    // s is a 33 bit value
    s = (uint64_t) as[i] + (uint64_t) bs[i] + (uint64_t) c;
    // >>: right shift operator, this will extract the lower part of the value
    c = s >> 32;

    // as = as + bs, add bs to as
    as[i] = (uint32_t) s;
  }

  // after the first loop finished it will continue to the second loop if this is true: (sz _ b < sz_a)
  // c == 1 is to slightly optimize the loop so that it terminates once there is no carry bit
  for (; i < sz_a && c == 1; i++) {
    // s is a 33 bit value
    s = (uint64_t) as[i] + (uint64_t) c;
    c = s >> 32;
    as[i] = (uint32_t) s;
  }
  return c;
}

// computes: as += bs * d
void partialprod32(uint32_t as[], int sz_a, uint32_t bs[], int sz_b, uint32_t d) { // Assume that sz_b <= sz_a
  uint32_t c = 0; // stores carry bits
  uint64_t sum; // the product plus the previous carry (bs[i] * d + c) this will never exceed 64 bits
  int i;

  // iterate through bs, calculate (bs[i]*d + c + as[i]), then store the new carry
  for(i = 0; i < sz_b; i++) {
    sum = (uint64_t) bs[i] * (uint64_t) d + (uint64_t) c + as[i]; // sum <= 64 bits
    c = sum >> 32;
    as[i] = (uint32_t) sum;
  }

  // after the first loop finished it will continue to the second loop if this is true: (sz _ b < sz_a)
  // c != 0 is to slightly optimize the loop so that it terminates once there is no more carry
  for (; i < sz_a && c != 0; i++) {
    sum = (uint64_t) as[i] + (uint64_t) c;
    c = sum >> 32;
    as[i] = (uint32_t) sum;
  }
}

// computes: a = b * c
void bigmul64(uint64_t a[], int sz_a, uint64_t b[], int sz_b, uint64_t c[], int sz_c) {
  // sz_a >= sz_b + sz_c
  uint32_t *a_32 = (uint32_t *) a; // treat address as an array of 32-bit values
  uint32_t *b_32 = (uint32_t *) b; // treat address as an array of 32-bit values
  uint32_t *c_32 = (uint32_t *) c; // treat address as an array of 32-bit values
  
  // choose array c as partial product number, iterate through c using c[i] as d value for partialprod32
  for(int i = 0; i < sz_c*2; i++) {
    // a_32 + i increments array pointer to adjust for adding into the correct column
    partialprod32(a_32 + i, sz_a*2, b_32, sz_b*2, c_32[i]);
  }
}

int main(void) {
  // a is product output array, b and c are input factors.
  // sz_a must be >= sz_b + sz_c, and a must be an empty array

  const int sz_a = 5;
  const int sz_b = 3;
  const int sz_c = 2;

  uint64_t a[5] = {0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000};
  uint64_t b[3] = {0xffffffffffffffff, 0xffffffffffffffff, 0xffffffffffffffff};
  uint64_t c[2] = {0xffffffffffffffff, 0xffffffffffffffff};

  bigmul64(a, sz_a, b, sz_b, c, sz_c);

  printf("B: ");
  for(int i = 0; i < sz_b; ++i) {
    printf("%016lx ", b[i]);
  }
  printf("\n");

  printf("C: ");
  for(int i = 0; i < sz_c; ++i) {
    printf("%016lx ", c[i]);
  }
  printf("\n");

  printf("A: ");
  for(int i = 0; i < sz_a; ++i) {
    printf("%016lx ", a[i]);
  }
  printf("\n");
 
  return 0;
}
