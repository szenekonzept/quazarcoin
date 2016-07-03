// Copyright (c) 2011-2014 The Cryptonote developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "gtest/gtest.h"

#include "cryptonote_core/cryptonote_basic_impl.h"

using namespace cryptonote;

namespace
{
  //--------------------------------------------------------------------------------------------------------------------
  class block_reward_and_already_generated_coins : public ::testing::Test
  {
  protected:
    static const size_t current_block_size = CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE / 2;

    bool m_block_not_too_big;
    uint64_t m_block_reward;
  };

  #define TEST_ALREADY_GENERATED_COINS(already_generated_coins, expected_reward)                            \
    m_block_not_too_big = get_block_reward(0, current_block_size, already_generated_coins, m_block_reward); \
    ASSERT_TRUE(m_block_not_too_big);                                                                       \
    ASSERT_EQ(m_block_reward, UINT64_C(expected_reward));

  TEST_F(block_reward_and_already_generated_coins, handles_first_values)
  {
    TEST_ALREADY_GENERATED_COINS(0, 8796093022207);
    TEST_ALREADY_GENERATED_COINS(m_block_reward, 8796088827904);
    TEST_ALREADY_GENERATED_COINS(UINT64_C(2756434948434199641), 7481722414624);
  }

  TEST_F(block_reward_and_already_generated_coins, correctly_steps_from_2_to_1)
  {
    TEST_ALREADY_GENERATED_COINS(MONEY_SUPPLY - ((2 << EMISSION_SPEED_FACTOR) + 1), 2);
    TEST_ALREADY_GENERATED_COINS(MONEY_SUPPLY -  (2 << EMISSION_SPEED_FACTOR), 2);
    TEST_ALREADY_GENERATED_COINS(MONEY_SUPPLY - ((2 << EMISSION_SPEED_FACTOR) - 1), 1);
  }

  TEST_F(block_reward_and_already_generated_coins, handles_max)
  {
    TEST_ALREADY_GENERATED_COINS(MONEY_SUPPLY - ((1 << EMISSION_SPEED_FACTOR) + 1), 1);
    TEST_ALREADY_GENERATED_COINS(MONEY_SUPPLY -  (1 << EMISSION_SPEED_FACTOR), 1);
    TEST_ALREADY_GENERATED_COINS(MONEY_SUPPLY - ((1 << EMISSION_SPEED_FACTOR) - 1), 0);
  }

  //--------------------------------------------------------------------------------------------------------------------
  class block_reward_and_current_block_size : public ::testing::Test
  {
  protected:
    virtual void SetUp()
    {
      m_block_not_too_big = get_block_reward(0, 0, already_generated_coins, m_standard_block_reward);
      ASSERT_TRUE(m_block_not_too_big);
      ASSERT_LT(CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE, m_standard_block_reward);
    }

    void do_test(size_t median_block_size, size_t current_block_size)
    {
      m_block_not_too_big = get_block_reward(median_block_size, current_block_size, already_generated_coins, m_block_reward);
    }

    static const uint64_t already_generated_coins = 0;

    bool m_block_not_too_big;
    uint64_t m_block_reward;
    uint64_t m_standard_block_reward;
  };

  TEST_F(block_reward_and_current_block_size, handles_block_size_less_relevance_level)
  {
    do_test(0, CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE - 1);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_EQ(m_block_reward, m_standard_block_reward);
  }

  TEST_F(block_reward_and_current_block_size, handles_block_size_eq_relevance_level)
  {
    do_test(0, CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_EQ(m_block_reward, m_standard_block_reward);
  }

  TEST_F(block_reward_and_current_block_size, handles_block_size_gt_relevance_level)
  {
    do_test(0, CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE + 1);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_LT(m_block_reward, m_standard_block_reward);
  }

  TEST_F(block_reward_and_current_block_size, handles_block_size_less_2_relevance_level)
  {
    do_test(0, 2 * CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE - 1);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_LT(m_block_reward, m_standard_block_reward);
    ASSERT_LT(0, m_block_reward);
  }

  TEST_F(block_reward_and_current_block_size, handles_block_size_eq_2_relevance_level)
  {
    do_test(0, 2 * CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_EQ(0, m_block_reward);
  }

  TEST_F(block_reward_and_current_block_size, handles_block_size_gt_2_relevance_level)
  {
    do_test(0, 2 * CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE + 1);
    ASSERT_FALSE(m_block_not_too_big);
  }

  TEST_F(block_reward_and_current_block_size, handles_big_median)
  {
    size_t block_size = 0;
    size_t median = std::numeric_limits<uint32_t>::max();

    do_test(median, block_size);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_EQ(m_block_reward, m_standard_block_reward);
  }

  TEST_F(block_reward_and_current_block_size, handles_big_block_size_ok)
  {
    size_t block_size = std::numeric_limits<uint32_t>::max();
    size_t median = block_size / 2 + 1;

    do_test(median, block_size);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_LT(m_block_reward, m_standard_block_reward);
  }

  TEST_F(block_reward_and_current_block_size, handles_big_block_size_fail)
  {
    size_t block_size = std::numeric_limits<uint32_t>::max();
    size_t median = block_size / 2 - 1;

    do_test(median, block_size);
    ASSERT_FALSE(m_block_not_too_big);
  }


  TEST_F(block_reward_and_current_block_size, handles_big_median_and_block_size)
  {
    size_t block_size = std::numeric_limits<uint32_t>::max();
    size_t median = std::numeric_limits<uint32_t>::max() - 1;

    do_test(median, block_size);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_LT(m_block_reward, m_standard_block_reward);
  }


  //--------------------------------------------------------------------------------------------------------------------
  class block_reward_and_last_block_sizes : public ::testing::Test
  {
  protected:
    virtual void SetUp()
    {
      m_last_block_sizes.push_back(3  * CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE);
      m_last_block_sizes.push_back(5  * CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE);
      m_last_block_sizes.push_back(7  * CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE);
      m_last_block_sizes.push_back(11 * CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE);
      m_last_block_sizes.push_back(13 * CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE);

      m_last_block_sizes_median = 7 * CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE;

      m_block_not_too_big = get_block_reward(epee::misc_utils::median(m_last_block_sizes), 0, already_generated_coins, m_standard_block_reward);
      ASSERT_TRUE(m_block_not_too_big);
      ASSERT_LT(CRYPTONOTE_BLOCK_GRANTED_FULL_REWARD_ZONE, m_standard_block_reward);
    }

    void do_test(size_t current_block_size)
    {
      m_block_not_too_big = get_block_reward(epee::misc_utils::median(m_last_block_sizes), current_block_size, already_generated_coins, m_block_reward);
    }

    static const uint64_t already_generated_coins = 0;

    std::vector<size_t> m_last_block_sizes;
    uint64_t m_last_block_sizes_median;
    bool m_block_not_too_big;
    uint64_t m_block_reward;
    uint64_t m_standard_block_reward;
  };

  TEST_F(block_reward_and_last_block_sizes, handles_block_size_less_median)
  {
    do_test(m_last_block_sizes_median - 1);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_EQ(m_block_reward, m_standard_block_reward);
  }

  TEST_F(block_reward_and_last_block_sizes, handles_block_size_eq_median)
  {
    do_test(m_last_block_sizes_median);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_EQ(m_block_reward, m_standard_block_reward);
  }

  TEST_F(block_reward_and_last_block_sizes, handles_block_size_gt_median)
  {
    do_test(m_last_block_sizes_median + 1);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_LT(m_block_reward, m_standard_block_reward);
  }

  TEST_F(block_reward_and_last_block_sizes, handles_block_size_less_2_medians)
  {
    do_test(2 * m_last_block_sizes_median - 1);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_LT(m_block_reward, m_standard_block_reward);
    ASSERT_LT(0, m_block_reward);
  }

  TEST_F(block_reward_and_last_block_sizes, handles_block_size_eq_2_medians)
  {
    do_test(2 * m_last_block_sizes_median);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_EQ(0, m_block_reward);
  }

  TEST_F(block_reward_and_last_block_sizes, handles_block_size_gt_2_medians)
  {
    do_test(2 * m_last_block_sizes_median + 1);
    ASSERT_FALSE(m_block_not_too_big);
  }

  TEST_F(block_reward_and_last_block_sizes, calculates_correctly)
  {
    ASSERT_EQ(0, m_last_block_sizes_median % 8);

    do_test(m_last_block_sizes_median * 9 / 8);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_EQ(m_block_reward, m_standard_block_reward * 63 / 64);

    // 3/2 = 12/8
    do_test(m_last_block_sizes_median * 3 / 2);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_EQ(m_block_reward, m_standard_block_reward * 3 / 4);

    do_test(m_last_block_sizes_median * 15 / 8);
    ASSERT_TRUE(m_block_not_too_big);
    ASSERT_EQ(m_block_reward, m_standard_block_reward * 15 / 64);
  }
}
