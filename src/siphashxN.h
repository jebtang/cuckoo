#ifndef INCLUDE_SIPHASHXN_H
#define INCLUDE_SIPHASHXN_H
#ifdef __AVX2__

#define ADD(a, b) _mm256_add_epi64(a, b)
#define XOR(a, b) _mm256_xor_si256(a, b)
#define ROTATE16 _mm256_set_epi64x(0x0D0C0B0A09080F0EULL,0x0504030201000706ULL, \
                                   0x0D0C0B0A09080F0EULL, 0x0504030201000706ULL)
#define ROT13(x) _mm256_or_si256(_mm256_slli_epi64(x,13),_mm256_srli_epi64(x,51))
#define ROT16(x) _mm256_shuffle_epi8((x), ROTATE16)
#define ROT17(x) _mm256_or_si256(_mm256_slli_epi64(x,17),_mm256_srli_epi64(x,47))
#define ROT21(x) _mm256_or_si256(_mm256_slli_epi64(x,21),_mm256_srli_epi64(x,43))
#define ROT32(x) _mm256_shuffle_epi32((x), _MM_SHUFFLE(2, 3, 0, 1))

#elif 0 && defined __SSE2__

#define ADD(a, b) _mm_add_epi64(a, b)
#define XOR(a, b) _mm_xor_si128(a, b)
#define ROT13(x) _mm_or_si128(_mm_slli_epi64(x,13),_mm_srli_epi64(x,51))
#define ROT16(x) _mm_or_si128(_mm_slli_epi64(x,16),_mm_srli_epi64(x,48))
#define ROT17(x) _mm_or_si128(_mm_slli_epi64(x,17),_mm_srli_epi64(x,47))
#define ROT21(x) _mm_or_si128(_mm_slli_epi64(x,21),_mm_srli_epi64(x,43))
#define ROT32(x) _mm_shuffle_epi32((x), _MM_SHUFFLE(2, 3, 0, 1))

#endif

#define SIPROUNDXN \
  do { \
    v0 = ADD(v0,v1); v2 = ADD(v2,v3); v1 = ROT13(v1); \
    v3 = ROT16(v3);  v1 = XOR(v1,v0); v3 = XOR(v3,v2); \
    v0 = ROT32(v0);  v2 = ADD(v2,v1); v0 = ADD(v0,v3); \
    v1 = ROT17(v1);                   v3 = ROT21(v3); \
    v1 = XOR(v1,v2); v3 = XOR(v3,v0); v2 = ROT32(v2); \
  } while(0)

#define SIPROUNDX2N \
  do { \
    v0 = ADD(v0,v1); v4 = ADD(v4,v5); \
    v2 = ADD(v2,v3); v6 = ADD(v6,v7); \
    v1 = ROT13(v1);  v5 = ROT13(v5); \
    v3 = ROT16(v3);  v7 = ROT16(v7); \
    v1 = XOR(v1,v0); v5 = XOR(v5,v4); \
    v3 = XOR(v3,v2); v7 = XOR(v7,v6); \
    v0 = ROT32(v0);  v4 = ROT32(v4); \
    v2 = ADD(v2,v1); v6 = ADD(v6,v5); \
    v0 = ADD(v0,v3); v4 = ADD(v4,v7); \
    v1 = ROT17(v1);  v5 = ROT17(v5); \
    v3 = ROT21(v3);  v7 = ROT21(v7); \
    v1 = XOR(v1,v2); v5 = XOR(v5,v6); \
    v3 = XOR(v3,v0); v7 = XOR(v7,v4); \
    v2 = ROT32(v2);  v6 = ROT32(v6); \
  } while(0)
 
#define SIPROUNDX4N \
  do { \
    v0 = ADD(v0,v1); v4 = ADD(v4,v5);  v8 = ADD(v8,v9); vC = ADD(vC,vD); \
    v2 = ADD(v2,v3); v6 = ADD(v6,v7);  vA = ADD(vA,vB); vE = ADD(vE,vF); \
    v1 = ROT13(v1);  v5 = ROT13(v5);   v9 = ROT13(v9);  vD = ROT13(vD); \
    v3 = ROT16(v3);  v7 = ROT16(v7);   vB = ROT16(vB);  vF = ROT16(vF); \
    v1 = XOR(v1,v0); v5 = XOR(v5,v4);  v9 = XOR(v9,v8); vD = XOR(vD,vC); \
    v3 = XOR(v3,v2); v7 = XOR(v7,v6);  vB = XOR(vB,vA); vF = XOR(vF,vE); \
    v0 = ROT32(v0);  v4 = ROT32(v4);   v8 = ROT32(v8);  vC = ROT32(vC); \
    v2 = ADD(v2,v1); v6 = ADD(v6,v5);  vA = ADD(vA,v9); vE = ADD(vE,vD); \
    v0 = ADD(v0,v3); v4 = ADD(v4,v7);  v8 = ADD(v8,vB); vC = ADD(vC,vF); \
    v1 = ROT17(v1);  v5 = ROT17(v5);   v9 = ROT17(v9);  vD = ROT17(vD); \
    v3 = ROT21(v3);  v7 = ROT21(v7);   vB = ROT21(vB);  vF = ROT21(vF); \
    v1 = XOR(v1,v2); v5 = XOR(v5,v6);  v9 = XOR(v9,vA); vD = XOR(vD,vE); \
    v3 = XOR(v3,v0); v7 = XOR(v7,v4);  vB = XOR(vB,v8); vF = XOR(vF,vC); \
    v2 = ROT32(v2);  v6 = ROT32(v6);   vA = ROT32(vA);  vE = ROT32(vE); \
  } while(0)

#ifdef __AVX2__

// 4-way sipHash-2-4 specialized to precomputed key and 8 byte nonces
void siphash24x4(const siphash_keys *keys, const u64 *indices, u64 *hashes) {
  const __m256i packet = _mm256_load_si256((__m256i *)indices);
  const __m256i init = _mm256_set_epi64x(keys->k3, keys->k2, keys->k1, keys->k0);
  __m256i v3 = _mm256_permute4x64_epi64(init, 0xFF);
  __m256i v0 = _mm256_permute4x64_epi64(init, 0x00);
  __m256i v1 = _mm256_permute4x64_epi64(init, 0x55);
  __m256i v2 = _mm256_permute4x64_epi64(init, 0xAA);

  v3 = XOR(v3,packet);
  SIPROUNDXN; SIPROUNDXN;
  v0 = XOR(v0,packet);
  v2 = XOR(v2,_mm256_broadcastq_epi64(_mm_cvtsi64_si128(0xff)));
  SIPROUNDXN; SIPROUNDXN; SIPROUNDXN; SIPROUNDXN;
  _mm256_store_si256((__m256i *)hashes, XOR(XOR(v0,v1),XOR(v2,v3)));
}

// 8-way sipHash-2-4 specialized to precomputed key and 8 byte nonces
void siphash24x8(const siphash_keys *keys, const u64 *indices, u64 *hashes) {
  const __m256i init = _mm256_set_epi64x(keys->k3, keys->k2, keys->k1, keys->k0);
  const __m256i packet0 = _mm256_load_si256((__m256i *)indices);
  const __m256i packet4 = _mm256_load_si256((__m256i *)(indices+4));
  __m256i v3 = _mm256_permute4x64_epi64(init, 0xFF);
  __m256i v0 = _mm256_permute4x64_epi64(init, 0x00);
  __m256i v1 = _mm256_permute4x64_epi64(init, 0x55);
  __m256i v2 = _mm256_permute4x64_epi64(init, 0xAA);
  __m256i v7 = _mm256_permute4x64_epi64(init, 0xFF);
  __m256i v4 = _mm256_permute4x64_epi64(init, 0x00);
  __m256i v5 = _mm256_permute4x64_epi64(init, 0x55);
  __m256i v6 = _mm256_permute4x64_epi64(init, 0xAA);

  v3 = XOR(v3,packet0); v7 = XOR(v7,packet4);
  SIPROUNDX2N; SIPROUNDX2N;
  v0 = XOR(v0,packet0); v4 = XOR(v4,packet4);
  v2 = XOR(v2,_mm256_broadcastq_epi64(_mm_cvtsi64_si128(0xff)));
  v6 = XOR(v6,_mm256_broadcastq_epi64(_mm_cvtsi64_si128(0xff)));
  SIPROUNDX2N; SIPROUNDX2N; SIPROUNDX2N; SIPROUNDX2N;
  _mm256_store_si256((__m256i *)hashes, XOR(XOR(v0,v1),XOR(v2,v3)));
  _mm256_store_si256((__m256i *)(hashes+4), XOR(XOR(v4,v5),XOR(v6,v7)));
}

// 16-way sipHash-2-4 specialized to precomputed key and 8 byte nonces
void siphash24x16(const siphash_keys *keys, const u64 *indices, u64 *hashes) {
  const __m256i init = _mm256_set_epi64x(keys->k3, keys->k2, keys->k1, keys->k0);
  const __m256i packet0 = _mm256_load_si256((__m256i *)indices);
  const __m256i packet4 = _mm256_load_si256((__m256i *)(indices+4));
  const __m256i packet8 = _mm256_load_si256((__m256i *)(indices+8));
  const __m256i packetC = _mm256_load_si256((__m256i *)(indices+12));
  __m256i v3 = _mm256_permute4x64_epi64(init, 0xFF);
  __m256i v0 = _mm256_permute4x64_epi64(init, 0x00);
  __m256i v1 = _mm256_permute4x64_epi64(init, 0x55);
  __m256i v2 = _mm256_permute4x64_epi64(init, 0xAA);
  __m256i v7 = _mm256_permute4x64_epi64(init, 0xFF);
  __m256i v4 = _mm256_permute4x64_epi64(init, 0x00);
  __m256i v5 = _mm256_permute4x64_epi64(init, 0x55);
  __m256i v6 = _mm256_permute4x64_epi64(init, 0xAA);
  __m256i vB = _mm256_permute4x64_epi64(init, 0xFF);
  __m256i v8 = _mm256_permute4x64_epi64(init, 0x00);
  __m256i v9 = _mm256_permute4x64_epi64(init, 0x55);
  __m256i vA = _mm256_permute4x64_epi64(init, 0xAA);
  __m256i vF = _mm256_permute4x64_epi64(init, 0xFF);
  __m256i vC = _mm256_permute4x64_epi64(init, 0x00);
  __m256i vD = _mm256_permute4x64_epi64(init, 0x55);
  __m256i vE = _mm256_permute4x64_epi64(init, 0xAA);

  v3 = XOR(v3,packet0); v7 = XOR(v7,packet4); vB = XOR(vB,packet8); vF = XOR(vF,packetC);
  SIPROUNDX4N; SIPROUNDX4N;
  v0 = XOR(v0,packet0); v4 = XOR(v4,packet4); v8 = XOR(v8,packet8); vC = XOR(vC,packetC);
  v2 = XOR(v2,_mm256_broadcastq_epi64(_mm_cvtsi64_si128(0xff)));
  v6 = XOR(v6,_mm256_broadcastq_epi64(_mm_cvtsi64_si128(0xff)));
  vA = XOR(vA,_mm256_broadcastq_epi64(_mm_cvtsi64_si128(0xff)));
  vE = XOR(vE,_mm256_broadcastq_epi64(_mm_cvtsi64_si128(0xff)));
  SIPROUNDX4N; SIPROUNDX4N; SIPROUNDX4N; SIPROUNDX4N;
  _mm256_store_si256((__m256i *) hashes    , XOR(XOR(v0,v1),XOR(v2,v3)));
  _mm256_store_si256((__m256i *)(hashes+ 4), XOR(XOR(v4,v5),XOR(v6,v7)));
  _mm256_store_si256((__m256i *)(hashes+ 8), XOR(XOR(v8,v9),XOR(vA,vB)));
  _mm256_store_si256((__m256i *)(hashes+12), XOR(XOR(vC,vD),XOR(vE,vF)));
}

#elif 0 && defined __SSE2__

// 4-way sipHash-2-4 specialized to precomputed key and 8 byte nonces
void siphash24x2(const siphash_keys *keys, const u64 *indices, u64 *hashes) {
  const __m128i packet = _mm_load_si128((__m128i *)indices);
  const __m128i init0  = _mm_load_si128((__m128i *)&keys);
  const __m128i init1  = _mm_load_si128((__m128i *)(&keys + 2));

  __m128i v3 = _mm_unpackhi_epi64(init1, init1); // _mm_shuffle_epi32(init1, 0xee);
  __m128i v0 = _mm_unpacklo_epi64(init0, init0); // _mm_shuffle_epi32(init0, 0x44);
  __m128i v1 = _mm_unpackhi_epi64(init0, init0); // _mm_shuffle_epi32(init0, 0xee);
  __m128i v2 = _mm_unpacklo_epi64(init1, init1); // _mm_shuffle_epi32(init1, 0x44);

  v3 = XOR(v3,packet);
  SIPROUNDXN; SIPROUNDXN;
  v0 = XOR(v0,packet);
  v2 = XOR(v2,_mm128_broadcastq_epi64(_mm_cvtsi64_si128(0xff)));
  SIPROUNDXN; SIPROUNDXN; SIPROUNDXN; SIPROUNDXN;
  _mm128_store_si256((__m256i *)hashes, XOR(XOR(v0,v1),XOR(v2,v3)));
}

// 8-way sipHash-2-4 specialized to precomputed key and 8 byte nonces
void siphash24x4(const siphash_keys *keys, const u64 *indices, u64 *hashes) {
  const __m256i init = _mm128_set_epi64x(keys->k3, keys->k2, keys->k1, keys->k0);
  const __m256i packet0 = _mm128_load_si256((__m256i *)indices);
  const __m256i packet4 = _mm128_load_si256((__m256i *)(indices+4));
  __m256i v3 = _mm128_permute4x64_epi64(init, 0xFF);
  __m256i v0 = _mm128_permute4x64_epi64(init, 0x00);
  __m256i v1 = _mm128_permute4x64_epi64(init, 0x55);
  __m256i v2 = _mm128_permute4x64_epi64(init, 0xAA);
  __m256i v7 = _mm128_permute4x64_epi64(init, 0xFF);
  __m256i v4 = _mm128_permute4x64_epi64(init, 0x00);
  __m256i v5 = _mm128_permute4x64_epi64(init, 0x55);
  __m256i v6 = _mm128_permute4x64_epi64(init, 0xAA);

  v3 = XOR(v3,packet0); v7 = XOR(v7,packet4);
  SIPROUNDX2N; SIPROUNDX2N;
  v0 = XOR(v0,packet0); v4 = XOR(v4,packet4);
  v2 = XOR(v2,_mm128_broadcastq_epi64(_mm_cvtsi64_si128(0xff)));
  v6 = XOR(v6,_mm128_broadcastq_epi64(_mm_cvtsi64_si128(0xff)));
  SIPROUNDX2N; SIPROUNDX2N; SIPROUNDX2N; SIPROUNDX2N;
  _mm128_store_si256((__m256i *)hashes, XOR(XOR(v0,v1),XOR(v2,v3)));
  _mm128_store_si256((__m256i *)(hashes+4), XOR(XOR(v4,v5),XOR(v6,v7)));
}
#endif

#ifndef NSIPHASH
// how many siphash24 to compute in parallel
// currently 1, 4, 8 are supported, but
// more than 1 requires the use of avx2
#define NSIPHASH 1
#endif

void siphash24xN(const siphash_keys *keys, const u64 *indices, u64 * hashes) {
#if NSIPHASH == 1
  *hashes = siphash24(keys, *indices);
#elif NSIPHASH == 4
  siphash24x4(keys, indices, hashes);
#elif NSIPHASH == 8
  siphash24x8(keys, indices, hashes);
#elif NSIPHASH == 16
  siphash24x16(keys, indices, hashes);
#else
#error not implemented
#endif
}

#endif // ifdef INCLUDE_SIPHASHXN_H
