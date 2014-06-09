// Copyright (c) 2012-2013 The Cryptonote developers
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

    return true;
  }
}
