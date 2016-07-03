// Copyright (c) 2011-2014 The Cryptonote developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#define CRYPTONOTE_MAX_BLOCK_NUMBER                     500000000
#define CRYPTONOTE_MAX_BLOCK_SIZE                       500000000  // block header blob limit, never used!
#define CRYPTONOTE_MAX_TX_SIZE                          1000000000
#define CRYPTONOTE_PUBLIC_ADDRESS_TEXTBLOB_VER          0
#define CRYPTONOTE_PUBLIC_ADDRESS_BASE58_PREFIX         2 // addresses start with "2"
#define CRYPTONOTE_MINED_MONEY_UNLOCK_WINDOW            60
       
#define CURRENT_TRANSACTION_VERSION                     1
#define CURRENT_BLOCK_MAJOR_VERSION                     1
#define CURRENT_BLOCK_MINOR_VERSION                     0
#define CRYPTONOTE_BLOCK_FUTURE_TIME_LIMIT              60*60*2

#define BLOCKCHAIN_TIMESTAMP_CHECK_WINDOW               60

// MONEY_SUPPLY - total number coins to be generated
#define MONEY_SUPPLY                                    ((uint64_t)(-1))
#define EMISSION_SPEED_FACTOR                           (21)

#define CRYPTONOTE_REWARD_BLOCKS_WINDOW                 100
#define CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE       20000 //size of block (bytes) after which reward for block calculated using block size
#define CRYPTONOTE_COINBASE_BLOB_RESERVED_SIZE          600
#define CRYPTONOTE_DISPLAY_DECIMAL_POINT                12
// COIN - number of smallest units in one coin
#define COIN                                            ((uint64_t)1000000000000) // pow(10, 12)
#define DEFAULT_FEE                                     ((uint64_t)   5000000000) // 5 * pow(10, 9)


#define ORPHANED_BLOCKS_MAX_COUNT                       100
#define DIFFICULTY_TARGET                               120 // seconds
#define DIFFICULTY_WINDOW                               720 // blocks
#define DIFFICULTY_LAG                                  15
#define DIFFICULTY_CUT                                  60  // timestamps to cut after sorting
#define DIFFICULTY_BLOCKS_COUNT                         DIFFICULTY_WINDOW + DIFFICULTY_LAG


#define CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_SECONDS      DIFFICULTY_TARGET * CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS
#define CRYPTONOTE_LOCKED_TX_ALLOWED_DELTA_BLOCKS       1


#define DIFFICULTY_BLOCKS_ESTIMATE_TIMESPAN             DIFFICULTY_TARGET


#define BLOCKS_IDS_SYNCHRONIZING_DEFAULT_COUNT          10000  //by default, blocks ids count in synchronizing
#define BLOCKS_SYNCHRONIZING_DEFAULT_COUNT              200    //by default, blocks count in blocks downloading
#define CRYPTONOTE_PROTOCOL_HOP_RELAX_COUNT             3      //value of hop, after which we use only announce of new block

#define CRYPTONOTE_MEMPOOL_TX_LIVETIME                    86400 //seconds, one day
#define CRYPTONOTE_MEMPOOL_TX_FROM_ALT_BLOCK_LIVETIME     604800 //seconds, one week

#define P2P_DEFAULT_PORT                                23080
#define RPC_DEFAULT_PORT                                23081
#define COMMAND_RPC_GET_BLOCKS_FAST_MAX_COUNT           1000

#define P2P_LOCAL_WHITE_PEERLIST_LIMIT                  1000
#define P2P_LOCAL_GRAY_PEERLIST_LIMIT                   5000

#define P2P_DEFAULT_CONNECTIONS_COUNT                   8
#define P2P_DEFAULT_HANDSHAKE_INTERVAL                  60           //secondes
#define P2P_DEFAULT_PACKET_MAX_SIZE                     50000000     //50000000 bytes maximum packet size
#define P2P_DEFAULT_PEERS_IN_HANDSHAKE                  250
#define P2P_DEFAULT_CONNECTION_TIMEOUT                  5000       //5 seconds
#define P2P_DEFAULT_PING_CONNECTION_TIMEOUT             2000       //2 seconds
#define P2P_DEFAULT_INVOKE_TIMEOUT                      60*2*1000  //2 minutes
#define P2P_DEFAULT_HANDSHAKE_INVOKE_TIMEOUT            5000       //5 seconds
#define P2P_STAT_TRUSTED_PUB_KEY                        "fede02571de64623093f19e08a3b791e0ffe4d39d968c7cdc51cb8d37e6a2a0d"
#define P2P_DEFAULT_WHITELIST_CONNECTIONS_PERCENT       70

#define ALLOW_DEBUG_COMMANDS

#define CRYPTONOTE_NAME                         "quazarcoin"
#define CRYPTONOTE_BLOCKS_FILENAME              "blocks.dat"
#define CRYPTONOTE_BLOCKINDEXES_FILENAME        "blockindexes.dat"
#define CRYPTONOTE_BLOCKSCACHE_FILENAME         "blockscache.dat"
#define CRYPTONOTE_POOLDATA_FILENAME            "poolstate.bin"
#define P2P_NET_DATA_FILENAME                   "p2pstate.bin"
#define MINER_CONFIG_FILE_NAME                  "miner_conf.json"

#define THREAD_STACK_SIZE                       5 * 1024 * 1024

#define GENESIS_COINBASE_TX_HEX                 "013c01ff0001ffffffffffff01029b2e4c0281c0b02e7c53291a94d1d0cbff8883f8024f5142ee494ffbbd08807121014c52c089aceeab13a8399d61408b337bace367fe5de10318bb4ca45259b76314"

