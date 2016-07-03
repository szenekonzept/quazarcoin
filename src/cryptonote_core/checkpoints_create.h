// Copyright (c) 2011-2014 The Cryptonote developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "checkpoints.h"
#include "misc_log_ex.h"

#define ADD_CHECKPOINT(h, hash)  CHECK_AND_ASSERT(checkpoints.add_checkpoint(h,  hash), false);

namespace cryptonote {
  inline bool create_checkpoints(cryptonote::checkpoints& checkpoints)
  {
    ADD_CHECKPOINT(22400, "860e9ce8e668abcc44ffeff673d0e0db85fa1b2f335b6ad7833c86044f0d3fb3");
    ADD_CHECKPOINT(50000, "f17cd6cbb2a71aee3c1c1825ae1b32a8dda2e45688c3ae0ad24967e261fef58b");
    ADD_CHECKPOINT(75000, "d9cc03f53c81bdec812726997e49e96652250a3d87e7ebe96232f2b3daf41044");
    ADD_CHECKPOINT(100000, "7d419a69cb27aaeafa34e7374bd151b50ee539209ec4c46ce9e8844ab97d0db7");
    ADD_CHECKPOINT(110000, "f187eca03b650d6c6b27210a84a60265faaf520fb9af27bddfea42104b8a5190");   
    return true;
  }
}
