#pragma once
#include <string>
#include "optimizer.h"  // ✅ Required for OptimizerFlags



bool crackBruteForceMultithreaded(const std::string& hash,
                                  const std::string& algorithm,
                                  const std::string& charset,
                                  int minLen, int maxLen,
                                  int threads,
                                  const OptimizerFlags& optim);
