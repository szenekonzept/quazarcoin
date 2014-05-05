#include <future>
#include <boost/program_options.hpp>
#include "include_base_utils.h"
#include "net/http_client.h"
#include "storages/http_abstract_invoke.h"
#include "cryptonote_core/cryptonote_format_utils.h"
#include "rpc/core_rpc_server_commands_defs.h"

struct BlockTemplate {
  cryptonote::block block;
  uint64_t height;
  cryptonote::difficulty_type difficulty;
};

const size_t TX_EXTRA_FIELD_TAG_BYTES = 1;
const size_t TX_MM_FIELD_SIZE_BYTES = 1;
const size_t MAX_VARINT_SIZE = 9;
const size_t TX_MM_TAG_MAX_BYTES = MAX_VARINT_SIZE + sizeof(crypto::hash);
const size_t MERGE_MINING_TAG_RESERVED_SIZE = TX_EXTRA_FIELD_TAG_BYTES  + TX_MM_FIELD_SIZE_BYTES + TX_MM_TAG_MAX_BYTES;

class Miner {
public:
  Miner() : m_stopped(false) {
  }

  void start() {
    m_stopped = false;
  }

  void stop() {
    m_stopped = true;
  }

  bool findNonce(cryptonote::block& block, uint64_t height, cryptonote::difficulty_type difficulty, size_t threads, crypto::hash& hash) {
    block.nonce = 0;
    m_block = &block;
    m_difficulty = difficulty;
    m_hashes = 0;
    m_height = height;
    m_nonce = 0;
    m_nonceFound = false;
    m_startedThreads = 1;
    m_threads = threads;
    std::vector<std::future<crypto::hash>> futures;
    for (size_t i = 1; i < threads; ++i) {
      futures.emplace_back(std::async(std::launch::async, &Miner::threadProcedure, this));
    }

    std::chrono::steady_clock::time_point time1 = std::chrono::steady_clock::now();
    hash = findNonce(block);
    for (std::future<crypto::hash>& future : futures) {
      crypto::hash threadHash = future.get();
      if (cryptonote::null_hash != threadHash) {
        hash = threadHash;
      }
    }

    std::chrono::steady_clock::time_point time2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(time2 - time1);
    std::cout << "Hash rate: " << m_hashes / duration.count() << std::endl;
    block.nonce = m_nonce;
    return m_nonceFound;
  }

private:
  cryptonote::block* m_block;
  cryptonote::difficulty_type m_difficulty;
  std::atomic<uint64_t> m_hashes;
  uint64_t m_height;
  std::atomic<uint32_t> m_nonce;
  std::atomic<bool> m_nonceFound;
  std::atomic<uint32_t> m_startedThreads;
  std::atomic<bool> m_stopped;
  size_t m_threads;

  crypto::hash threadProcedure() {
    cryptonote::block block = *m_block;
    block.nonce = m_startedThreads++;
    return findNonce(block);
  }

  crypto::hash findNonce(cryptonote::block& block) {
    std::chrono::steady_clock::time_point time1 = std::chrono::steady_clock::now();
    crypto::hash blockHash = cryptonote::null_hash;
    while (!m_nonceFound && !m_stopped) {
      crypto::hash hash;
      bool result = get_block_longhash(block, hash, m_height);
      ++m_hashes;
      if (!result) {
        std::cout << "get_block_longhash failed." << std::endl;
        break;
      }

      if (cryptonote::check_hash(hash, m_difficulty)) {
        std::cout << "Nonce found." << std::endl;
        m_nonce = block.nonce;
        m_nonceFound = true;
        blockHash = hash;
        break;
      }

      if (std::numeric_limits<uint32_t>::max() - block.nonce < m_threads) {
        break;
      }

      std::chrono::steady_clock::time_point time2 = std::chrono::steady_clock::now();
      std::chrono::duration<double> duration = std::chrono::duration_cast<std::chrono::duration<double>>(time2 - time1);
      if (duration.count() > 15.0) {
        break;
      }

      block.nonce += static_cast<uint32_t>(m_threads);
    }

    return blockHash;
  }
};

class Minerc {
public:
  bool mine(const std::string& address1, const std::string& address2, size_t threads, const std::string& wallet1, const std::string& wallet2) {
    uint64_t prefix1;
    cryptonote::account_public_address walletAddress1;
    if (!get_account_address_from_str(prefix1, walletAddress1, wallet1)) {
      return false;
    }

    uint64_t prefix2;
    cryptonote::account_public_address walletAddress2;
    if (!get_account_address_from_str(prefix2, walletAddress2, wallet2)) {
      return false;
    }

    m_legacy2 = false;
    for (;;) {
      std::cout << "Requesting block1..." << std::endl;
      while (!getBlockTemplate(m_block1, address1, wallet1, MERGE_MINING_TAG_RESERVED_SIZE, m_httpClient1)) {
        std::cout << "Failed to get block1" << std::endl;
      }

      if (m_legacy2) {
        if (getBlockTemplate(m_block2, address2, wallet2, 0, m_httpClient2)) {
          if (m_block2.block.major_version == BLOCK_MAJOR_VERSION_2) {
            std::cout << "Block version 2 received from node2, enabling merged mining." << std::endl;
            m_legacy2 = false;
          }
        }
      } else {
        std::cout << "Requesting block2..." << std::endl;
        for (;;) {
          if (getBlockTemplate(m_block2, address2, wallet2, 0, m_httpClient2)) {
            if (m_block2.block.major_version != BLOCK_MAJOR_VERSION_2) {
              std::cout << "Unsupported block version received from node2, disabling merged mining." << std::endl;
              m_legacy2 = true;
            }

            break;
          }

          std::cout << "Failed to get block2" << std::endl;
        }
      }

      if (!m_legacy2) {
        if (!fillExtra()) {
          std::cout << "Internal error" << std::endl;
          return false;
        }
      }

      std::cout << "Mining..." << std::endl;
      crypto::hash hash;
      if (m_miner.findNonce(m_block1.block, m_block1.height, std::min(m_block1.difficulty, m_block2.difficulty), threads, hash)) {
        if (cryptonote::check_hash(hash, m_block1.difficulty)) {
          if (submitBlock(m_block1.block, address1, m_httpClient1)) {
            std::cout << "Submitted block1" << std::endl;
          } else {
            std::cout << "Failed to submit block1" << std::endl;
          }
        }

        if (!m_legacy2) {
          if (cryptonote::check_hash(hash, m_block2.difficulty)) {
            if (!mergeBlocks()) {
              std::cout << "Internal error" << std::endl;
              return false;
            }

            if (submitBlock(m_block2.block, address2, m_httpClient2)) {
              std::cout << "Submitted block2" << std::endl;
            } else {
              std::cout << "Failed to submit block2" << std::endl;
            }
          }
        }
      }
    }
  }

private:
  epee::net_utils::http::http_simple_client m_httpClient1;
  epee::net_utils::http::http_simple_client m_httpClient2;
  BlockTemplate m_block1;
  BlockTemplate m_block2;
  Miner m_miner;
  bool m_legacy2;

  bool getBlockTemplate(BlockTemplate& blockTemplate, const std::string& address, const std::string& walletAddress, size_t extraNonceSize, epee::net_utils::http::http_simple_client& client) {
    cryptonote::COMMAND_RPC_GETBLOCKTEMPLATE::request request;
    request.reserve_size = extraNonceSize;
    request.wallet_address = walletAddress;
    cryptonote::COMMAND_RPC_GETBLOCKTEMPLATE::response response;
    bool result = epee::net_utils::invoke_http_json_rpc(address + "/json_rpc", "getblocktemplate", request, response, client);
    if (!result || (!response.status.empty() && response.status != CORE_RPC_STATUS_OK)) {
      return false;
    }

    std::string blockString;
    epee::string_tools::parse_hexstr_to_binbuff(response.blocktemplate_blob, blockString);
    std::istringstream stringStream(blockString);
    binary_archive<false> archive(stringStream);
    if (!serialization::serialize(archive, blockTemplate.block)) {
      return false;
    }

    blockTemplate.difficulty = response.difficulty;
    blockTemplate.height = response.height;
    return true;
  }

  bool fillExtra() {
    std::vector<uint8_t>& extra = m_block1.block.miner_tx.extra;
    std::string extraAsString(reinterpret_cast<const char*>(extra.data()), extra.size());

    std::string extraNonceTemplate;
    extraNonceTemplate.push_back(TX_EXTRA_NONCE);
    extraNonceTemplate.push_back(MERGE_MINING_TAG_RESERVED_SIZE);
    extraNonceTemplate.append(MERGE_MINING_TAG_RESERVED_SIZE, '\0');

    size_t extraNoncePos = extraAsString.find(extraNonceTemplate);
    if (std::string::npos == extraNoncePos) {
      return false;
    }

    cryptonote::tx_extra_merge_mining_tag tag;
    tag.depth = 0;
    if (!cryptonote::get_block_header_hash(m_block2.block, tag.merkle_root)) {
      return false;
    }

    std::vector<uint8_t> extraNonceReplacement;
    if (!cryptonote::append_mm_tag_to_extra(extraNonceReplacement, tag)) {
      return false;
    }

    if (MERGE_MINING_TAG_RESERVED_SIZE < extraNonceReplacement.size()) {
      return false;
    }

    size_t diff = extraNonceTemplate.size() - extraNonceReplacement.size();
    if (0 < diff) {
      extraNonceReplacement.push_back(TX_EXTRA_NONCE);
      extraNonceReplacement.push_back(diff - 2);
    }

    std::copy(extraNonceReplacement.begin(), extraNonceReplacement.end(), extra.begin() + extraNoncePos);

    return true;
  }

  bool mergeBlocks() {
    m_block2.block.timestamp = m_block1.block.timestamp;
    m_block2.block.parent_block.major_version = m_block1.block.major_version;
    m_block2.block.parent_block.minor_version = m_block1.block.minor_version;
    m_block2.block.parent_block.prev_id = m_block1.block.prev_id;
    m_block2.block.parent_block.nonce = m_block1.block.nonce;
    m_block2.block.parent_block.miner_tx = m_block1.block.miner_tx;
    m_block2.block.parent_block.number_of_transactions = m_block1.block.tx_hashes.size() + 1;
    m_block2.block.parent_block.miner_tx_branch.resize(crypto::tree_depth(m_block1.block.tx_hashes.size() + 1));
    std::vector<crypto::hash> transactionHashes;
    transactionHashes.push_back(cryptonote::get_transaction_hash(m_block1.block.miner_tx));
    std::copy(m_block1.block.tx_hashes.begin(), m_block1.block.tx_hashes.end(), std::back_inserter(transactionHashes));
    tree_branch(transactionHashes.data(), transactionHashes.size(), m_block2.block.parent_block.miner_tx_branch.data());
    m_block2.block.parent_block.blockchain_branch.clear();
    return true;
  }

  bool submitBlock(const cryptonote::block& block, const std::string& address, epee::net_utils::http::http_simple_client& client) {
    cryptonote::COMMAND_RPC_SUBMITBLOCK::request request;
    request.push_back(epee::string_tools::buff_to_hex_nodelimer(t_serializable_object_to_blob(block)));
    cryptonote::COMMAND_RPC_SUBMITBLOCK::response response;
    bool result = epee::net_utils::invoke_http_json_rpc(address + "/json_rpc", "submitblock", request, response, client);
    if (!result || (!response.status.empty() && response.status != CORE_RPC_STATUS_OK)) {
      return false;
    }

    return true;
  }
};

int main(int argc, char* argv[], char* envp[]) {
  std::string node1;
  std::string node2;
  size_t threads;
  std::string wallet1;
  std::string wallet2;
  boost::program_options::options_description optionsDescription("Allowed options");
  optionsDescription.add_options()
    ("help", "show usage help")
    ("node1", boost::program_options::value(&node1)->required(), "set first node address, for example 127:0.0.1:80")
    ("node2", boost::program_options::value(&node2)->required(), "set second node address")
    ("threads", boost::program_options::value(&threads)->default_value(1), "set number of mining threads")
    ("wallet1", boost::program_options::value(&wallet1)->required(), "set target wallet 1")
    ("wallet2", boost::program_options::value(&wallet2)->required(), "set target wallet 2");

  boost::program_options::variables_map options;
  try {
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, optionsDescription), options);
    if (options.count("help") != 0) {
      std::cout << optionsDescription << std::endl;
      return 0;
    }

    boost::program_options::notify(options);
  } catch (std::exception& e) {
    std::cout << e.what() << std::endl;
    std::cout << "type --help to see valid options" << std::endl;
    return 1;
  }

  Minerc miner;
  if (!miner.mine(node1, node2, threads, wallet1, wallet2)) {
    std::cout << "Unable to start miner." << std::endl;
    return 1;
  }

  return 0;
}
