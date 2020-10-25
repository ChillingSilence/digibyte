// Copyright (c) 2020 barrystyle
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <crypto/randomx.h>

#include <chainparams.h>
#include <primitives/block.h>
#include <crypto/moneroheader.h>
#include <crypto/randomx/randomx.h>

randomx rxhash;

void randomx::cacheinit() {
    flags = randomx_get_flags();
    cache = randomx_alloc_cache(flags);
    randomx_init_cache(cache, &initialseed, 32);
    is_cache_init = true;
    LogPrintf("%s complete\n", __func__);
}

void randomx::cacherebuild() {
    cache = randomx_alloc_cache(flags);
    randomx_init_cache(cache, &currentseed, 32);
    LogPrintf("%s complete\n", __func__);
}

void randomx::vminit() {
    if (vm) randomx_destroy_vm(vm);
    vm = randomx_create_vm(flags, cache, nullptr);
    is_vm_init = true;
    LogPrintf("%s complete\n", __func__);
}

void randomx::shutoff() {
    if (vm) randomx_destroy_vm(vm);
    vm = nullptr;
    randomx_release_cache(cache);
    LogPrintf("%s complete\n", __func__);
}

void randomx::hash(const char* input, char* output, int len) {
    if (!is_cache_init) cacheinit();
    if (!is_vm_init) vminit();
    if (seed_has_changed) {
        this->shutoff();
        this->cacherebuild();
        this->vminit();
        seed_has_changed = false;
    }
    randomx_calculate_hash(vm, input, len, output);
}

