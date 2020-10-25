// Copyright (c) 2020 barrystyle
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef DIGIBYTE_SEEDMGR_H
#define DIGIBYTE_SEEDMGR_H

#include <validationinterface.h>

class SeedManager;
extern SeedManager seedMgr;

class SeedManager final : public CValidationInterface
{
public:
    explicit SeedManager() {};
    virtual ~SeedManager() = default;

    uint256 getSeedHash();
    void setSeedHash(uint256 seedHash);
    void updateSeedHashFromHeight(int height);
    void updateSeedHash(const CBlockIndex* pindexNew = nullptr);
};

#endif // DIGIBYTE_SEEDMGR_H
