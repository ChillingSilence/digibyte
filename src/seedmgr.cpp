// Copyright (c) 2020 barrystyle
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <chainparams.h>
#include <crypto/randomx.h>
#include <crypto/moneroheader.h>
#include <crypto/randomx/randomx.h>
#include <seedmgr.h>
#include <util/system.h>
#include <validation.h>

SeedManager seedMgr;

uint256 SeedManager::getSeedHash() {
    return rxhash.currentseed;
}

void SeedManager::setSeedHash(uint256 seedHash) {
    rxhash.currentseed = seedHash;
}

void SeedManager::updateSeedHashFromHeight(int height) {
    const CBlockIndex* pindex = ::ChainActive()[height];
    if(!pindex)
       updateSeedHash(nullptr);
    else
       updateSeedHash(pindex);
}

void SeedManager::updateSeedHash(const CBlockIndex *pindexNew)
{
    LOCK(cs_main);

    //! if fresh chain..
    if (pindexNew == nullptr) {
        pindexNew = ::ChainActive().Tip();
        if (pindexNew == nullptr) {
            rxhash.seed_has_changed = true;
            this->setSeedHash(uint256());
            return;
        }
    }

    //! potential seed candidate
    uint256 seed_prestage{};

    //! used to track the current state
    const int height = pindexNew->nHeight;
    const int epochlen = Params().GetConsensus().nEpochLength;
    const int epochprev = (((height / epochlen) * epochlen) - epochlen) > 0 ? (((height / epochlen) * epochlen) - epochlen) : 0;
    const int epochcurrent = ((height / epochlen) * epochlen);

    LogPrint(BCLog::BENCH, "height %d epochlen %d epochprev %d epochcurrent %d\n", height, epochlen, epochprev, epochcurrent);

    //! don't try using a negative epoch
    int offset = epochprev - epochlen;
    if (offset < 0)
        offset = 0;

    //! for the remote node, when syncing via headers - the blockheader will not make it into ::ChainActive()[] until all headers in a bunch
    //! are processed.. however, the seed relies on this data.. so we must preemptively cache the merkleroot of every 'future epoch' header
    if (height && (height % epochlen == 0))
        rxhash.seedcache.insert(std::pair<int, uint256>(height, pindexNew->GetBlockHeader().hashMerkleRoot));

    //! deal with chain scenarios
    if (height < epochlen) {
        seed_prestage = rxhash.initialseed;
    } else {
        auto it = rxhash.seedcache.find(offset);
        if (it != rxhash.seedcache.end()) {
            seed_prestage = it->second;
        } else {
            seed_prestage = ::ChainActive()[offset]->GetBlockHeader().hashMerkleRoot;
            rxhash.seedcache.insert(std::pair<int, uint256>(offset, seed_prestage));
        }
    }

    //! use this to signal vm/cache rebuild
    if (rxhash.currentseed != seed_prestage) {
        LogPrintf("seed has changed (from %s to %s)\n", rxhash.currentseed.ToString(), seed_prestage.ToString());
        rxhash.seed_has_changed = true;
        this->setSeedHash(seed_prestage);
    }
}

