// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2019 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <arith_uint256.h>
#include <crypto/hashgroestl.h>
#include <crypto/hashodo.h>
#include <crypto/hashqubit.h>
#include <crypto/hashskein.h>
#include <crypto/randomx.h>
#include <crypto/moneroheader.h>
#include <consensus/consensus.h>
#include <chainparams.h>
#include <seedmgr.h>
#include <util/strencodings.h>
#include <hash.h>

const bool algodebug = true;

uint256 CBlockHeader::GetHash() const
{
    return SerializeHash(*this);
}

int CBlockHeader::GetAlgo() const
{
    switch (nVersion & BLOCK_VERSION_ALGO)
    {
        case BLOCK_VERSION_SCRYPT:
            return ALGO_SCRYPT;
        case BLOCK_VERSION_SHA256D:
            return ALGO_SHA256D;
        case BLOCK_VERSION_GROESTL:
            return ALGO_GROESTL;
        case BLOCK_VERSION_SKEIN:
            return ALGO_SKEIN;
        case BLOCK_VERSION_QUBIT:
            return ALGO_QUBIT;
        case BLOCK_VERSION_ODO:
            return ALGO_ODO;
        case BLOCK_VERSION_RANDOMX:
            return ALGO_RANDOMX;
    }
    return ALGO_UNKNOWN;
}

uint32_t OdoKey(const Consensus::Params& params, uint32_t nTime)
{
    uint32_t nShapechangeInterval = params.nOdoShapechangeInterval;
    return nTime - nTime % nShapechangeInterval;

}

uint256 CBlockHeader::GetPoWAlgoHash(int nHeight, const Consensus::Params& params) const
{
    seedMgr.updateSeedHashFromHeight(nHeight);

    switch (GetAlgo())
    {
        case ALGO_SHA256D:
        {
            auto debugHash = GetHash();
            if (algodebug)
                LogPrintf("SHA256D - %s\n", debugHash.ToString());
            return debugHash;
        }

        case ALGO_SCRYPT:
        {
            uint256 thash;
            scrypt_1024_1_1_256(BEGIN(nVersion), BEGIN(thash));
            if (algodebug)
                LogPrintf("SCRYPT - %s\n", thash.ToString());
            return thash;
        }

        case ALGO_GROESTL:
        {
            auto debugHash = HashGroestl(BEGIN(nVersion), END(nNonce));
            if (algodebug)
                LogPrintf("GROESTL - %s\n", debugHash.ToString());
            return debugHash;
        }

        case ALGO_SKEIN:
        {
            auto debugHash = HashSkein(BEGIN(nVersion), END(nNonce));
            if (algodebug)
                LogPrintf("SKEIN - %s\n", debugHash.ToString());
            return debugHash;
        }

        case ALGO_QUBIT:
        {
            auto debugHash = HashQubit(BEGIN(nVersion), END(nNonce));
            if (algodebug)
                LogPrintf("QUBIT - %s\n", debugHash.ToString());
            return debugHash;
        }

        case ALGO_ODO:
        {
            uint32_t key = OdoKey(params, nTime);
            auto debugHash = HashOdo(BEGIN(nVersion), END(nNonce), key);
            if (algodebug)
                LogPrintf("ODO - %s\n", debugHash.ToString());
            return debugHash;
        }

        case ALGO_RANDOMX:
        {
            uint256 thash;
            serialize_monero_hash(BEGIN(nVersion), BEGIN(thash));
            if (algodebug)
                LogPrintf("RANDOMX - %s\n", thash.ToString());
            return thash;
        }

        case ALGO_UNKNOWN: {
            // This block will be rejected anyway, but returning an always-invalid
            // PoW hash will allow it to be rejected sooner.
            return ArithToUint256(~arith_uint256(0));
        }
    }
    assert(false);
    return GetHash();
}

std::string CBlock::ToString(const Consensus::Params& params) const
{
    std::stringstream s;
    s << strprintf("CBlock(hash=%s, ver=0x%08x, pow_algo=%d, hashPrevBlock=%s, hashMerkleRoot=%s, nTime=%u, nBits=%08x, nNonce=%u, vtx=%u)\n",
        GetHash().ToString(),
        nVersion,
        GetAlgo(),
        hashPrevBlock.ToString(),
        hashMerkleRoot.ToString(),
        nTime, nBits, nNonce,
        vtx.size());
    for (const auto& tx : vtx) {
        s << "  " << tx->ToString() << "\n";
    }
    return s.str();
}

std::string GetAlgoName(int Algo)
{
    switch (Algo)
    {
        case ALGO_SHA256D:
            return std::string("sha256d");
        case ALGO_SCRYPT:
            return std::string("scrypt");
        case ALGO_GROESTL:
            return std::string("groestl");
        case ALGO_SKEIN:
            return std::string("skein");
        case ALGO_QUBIT:
            return std::string("qubit");
        //case ALGO_EQUIHASH:
            //return std::string("equihash");
        //case ALGO_ETHASH:
            //return std::string("ethash");
        case ALGO_ODO:
            return std::string("odo");
        case ALGO_RANDOMX:
            return std::string("randomx");
    }
    return std::string("unknown");
}

int GetAlgoByName(std::string strAlgo, int fallback)
{
    transform(strAlgo.begin(),strAlgo.end(),strAlgo.begin(),::tolower);
    if (strAlgo == "sha" || strAlgo == "sha256" || strAlgo == "sha256d")
        return ALGO_SHA256D;
    else if (strAlgo == "scrypt")
        return ALGO_SCRYPT;
    else if (strAlgo == "groestl" || strAlgo == "groestlsha2")
        return ALGO_GROESTL;
    else if (strAlgo == "skein" || strAlgo == "skeinsha2")
        return ALGO_SKEIN;
    else if (strAlgo == "q2c" || strAlgo == "qubit")
        return ALGO_QUBIT;
    //else if (strAlgo == "equihash")
        //return ALGO_EQUIHASH;
    //else if (strAlgo == "ethash")
        //return ALGO_ETHASH;
    else if (strAlgo == "odo" || strAlgo == "odosha3")
        return ALGO_ODO;
    else if (strAlgo == "randomx" || strAlgo == "rx/0")
        return ALGO_RANDOMX;
    else
        return fallback;
}

int64_t GetBlockWeight(const CBlock& block)
{
    // This implements the weight = (stripped_size * 4) + witness_size formula,
    // using only serialization with and without witness data. As witness_size
    // is equal to total_size - stripped_size, this formula is identical to:
    // weight = (stripped_size * 3) + total_size.
    return ::GetSerializeSize(block, PROTOCOL_VERSION | SERIALIZE_TRANSACTION_NO_WITNESS) * (WITNESS_SCALE_FACTOR - 1) + ::GetSerializeSize(block, PROTOCOL_VERSION);
}
