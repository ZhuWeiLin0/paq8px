#include "StateMap.hpp"

StateMap::StateMap(const Shared* const sh, const int s, const int n, const int lim, const StateMap::MAPTYPE mapType) :
  AdaptiveMap(sh, n * s, lim), numContextSets(s), numContextsPerSet(n), numContexts(0), cxt(s) {
#ifdef VERBOSE
  printf("Created StateMap with s = %d, n = %d, lim = %d, maptype = %d\n", s, n, lim, mapType);
#endif
  assert(numContextSets > 0 && numContextsPerSet > 0);
  assert(limit > 0 && limit < 1024);
  if( mapType == BitHistory ) { // when the context is a bit history byte, we have a-priory for p
    assert((numContextsPerSet & 255) == 0);
    for( uint32_t cx = 0; cx < numContextsPerSet; ++cx ) {
      auto state = uint8_t(cx & 255U);
      uint32_t n0 = StateTable::next(state, 2) * 3 + 1;
      uint32_t n1 = StateTable::next(state, 3) * 3 + 1;
      for( uint32_t s = 0; s < numContextSets; ++s ) {
        t[s * numContextsPerSet + cx] = ((n1 << 20U) / (n0 + n1)) << 12U; //定义在adaptivemap中, t中一个元素32bit
      }
    }
  } else if( mapType == Run ) { // when the context is a run count: we have a-priory for p
    for( uint32_t cx = 0; cx < numContextsPerSet; ++cx ) {
      const int predictedBit = (cx) & 1U;
      const int uncertainty = (cx >> 1U) & 1U;
      //const int bp = (cx>>2)&1; // unused in calculation - a-priory does not seem to depend on bitPosition in the general case
      const int runCount = (cx >> 4U); // 0..254
      uint32_t n0 = uncertainty * 16 + 16;
      uint32_t n1 = runCount * 128 + 16;
      if( predictedBit == 0 ) {
        std::swap(n0, n1);
      }
      for( uint32_t s = 0; s < numContextSets; ++s ) {
        t[s * numContextsPerSet + cx] = ((n1 << 20U) / (n0 + n1)) << 12U | min(runCount, limit);
      }
    }
  } else { // no a-priory
    for( uint32_t i = 0; i < numContextsPerSet * numContextSets; ++i ) {
      t[i] = (1U << 31U) + 0; //initial p=0.5, initial count=0
    }
  }
}

void StateMap::reset(const int rate) {
  for( uint32_t i = 0; i < numContextsPerSet * numContextSets; ++i ) {
    t[i] = (t[i] & 0xfffffc00U) | min(rate, t[i] & 0x3FFU);
  }
}

void StateMap::update() {
  assert(numContexts <= numContextSets);
  while( numContexts > 0 ) {
    numContexts--;
    const uint32_t idx = cxt[numContexts];
    if( idx + 1 == 0 ) {
      continue; // UINT32_MAX: skipped context
    }
    assert(numContexts * numContextsPerSet <= idx && idx < (numContexts + 1) * numContextsPerSet);
    AdaptiveMap::update(&t[idx]);
  }
}

auto StateMap::p1(const uint32_t cx) -> int {
  shared->GetUpdateBroadcaster()->subscribe(this);
  assert(cx >= 0 && cx < numContextsPerSet);
  cxt[0] = cx;
  numContexts++;
  return t[cx] >> 20U;
}

auto StateMap::p2(const uint32_t s, const uint32_t cx) -> int {
  assert(s >= 0 && s < numContextSets);
  assert(cx >= 0 && cx < numContextsPerSet);
  assert(s == numContexts);
  const uint32_t idx = numContexts * numContextsPerSet + cx;
  cxt[numContexts] = idx;
  numContexts++;
  return t[idx] >> 20U;
}

void StateMap::subscribe() {
  shared->GetUpdateBroadcaster()->subscribe(this);
}

void StateMap::skip(const uint32_t s) {
  assert(s >= 0 && s < numContextSets);
  assert(s == numContexts);
  cxt[numContexts] = 0 - 1; // UINT32_MAX: mark for skipping
  numContexts++;
}

void StateMap::print() const {
  for( uint32_t i = 0; i < t.size(); i++ ) {
    uint32_t p0 = t[i] >> 10;
    printf("%d\t%d\n", i, p0);
  }
}
