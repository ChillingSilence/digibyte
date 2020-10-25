// Copyright (c) 2020 barrystyle
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <crypto/randomx.h>
#include <crypto/randomx/randomx.h>
#include <primitives/block.h>

void serialize_monero_header(const CBlockHeader& header, char& moneroHeader) {
    memcpy(&moneroHeader, &header.nVersion, 4);
    memcpy(&moneroHeader + 4, &header.hashPrevBlock, 32);
    memcpy(&moneroHeader + 36, &header.hashMerkleRoot, 3);
    memcpy(&moneroHeader + 40, &header.hashMerkleRoot, 32);
    memcpy(&moneroHeader + 39, &header.nNonce, 4);
    memcpy(&moneroHeader + 72, &header.nTime, 4);
}

void serialize_monero_hash(const char* input, char* output) {
    unsigned char moneroHeader[76];
    memcpy(moneroHeader, input, 4);
    memcpy(moneroHeader + 4, input + 4, 32);
    memcpy(moneroHeader + 36, input + 36, 3);
    memcpy(moneroHeader + 40, input + 36, 32);
    memcpy(moneroHeader + 39, input + 76, 4);
    memcpy(moneroHeader + 72, input + 68, 4);
    rxhash.hash((const char*)moneroHeader, (char*)output, 76);
}

void serialize_testcase() {
    char* newHeader = (char*)malloc(76);
    CBlockHeader header;
    header.nVersion = 0xaaaaaaaa;
    header.hashPrevBlock = uint256S("0xbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    header.hashMerkleRoot = uint256S("0xcccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc");
    header.nTime = 0xdddddddd;
    header.nBits = 0xeeeeeeee;
    header.nNonce = 0xffffffff;
    serialize_monero_header(header, *newHeader);
}

