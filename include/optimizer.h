#pragma once

struct OptimizerFlags {
    bool zero_byte     = false;
    bool early_skip    = false;
    bool not_salted    = false;
    bool not_iterated  = false;
    bool single_hash   = false;
    bool single_salt   = false;
    bool brute_force   = false;
    bool raw_hash      = false;
};
