#pragma once
#include <random>

class RNG {
 private:
  RNG() {
    std::random_device rd;
    gen = std::mt19937(rd());
  }

 public:
  std::mt19937 gen;
  static RNG& getInstance() {
    static RNG rng = RNG();
    return rng;
  }
};
