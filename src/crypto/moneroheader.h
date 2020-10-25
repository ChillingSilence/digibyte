// Copyright (c) 2020 barrystyle
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef DIGIBYTE_MONEROHEADER_H
#define DIGIBYTE_MONEROHEADER_H

#include <primitives/block.h>
#include <crypto/randomx/randomx.h>

void serialize_monero_header(const CBlockHeader& header, char& moneroHeader);
void serialize_monero_hash(const char* input, char* output);
void serialize_testcase();

#endif // DIGIBYTE_MONEROHEADER_H
