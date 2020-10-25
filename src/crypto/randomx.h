// Copyright (c) 2020 barrystyle
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef DIGIBYTE_RANDOMX_H
#define DIGIBYTE_RANDOMX_H

#include <crypto/randomx/randomx.h>
#include <uint256.h>

#include <map>

class uint256;
class CBlockIndex;
class CBlockHeader;

class randomx;
extern randomx rxhash;

//! randomx hashing class
class randomx {
  private:
    randomx_flags flags{};
    randomx_vm* vm{nullptr};
    randomx_cache* cache{nullptr};

  public:

    bool is_vm_init{false};
    bool is_cache_init{false};

    uint256 initialseed{};

    uint256 currentseed{};
    bool seed_has_changed{false};
    std::map<int, uint256> seedcache;

    void cacheinit();
    void vminit();
    void cacherebuild();
    void shutoff();
    void hash(const char* input, char* output, int len);
};


#endif // DIGIBYTE_RANDOMX_H
