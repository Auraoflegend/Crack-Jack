#pragma once
#include <string>     // for std::string
#include <ostream>    // for std::ostream
#include "optimizer.h"
#include "resume_manager.h"

bool crackBruteForceMultithreaded(const std::string& targetHash,
                                  const std::string& algorithm,
                                  const std::string& mask,
                                  int minLen, int maxLen,
                                  int numThreads,
                                  const OptimizerFlags& optim,
                                  std::ostream* logStream = nullptr,
                                  const std::string& resumePath = "");
