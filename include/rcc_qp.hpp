/*
 * Copyright 2018, 2019 DaeHun Nyang, DaeHong Min
 * Free for non-commercial and education use.
 * For commercial use, contact nyang@inha.ac.kr
 */

#ifndef GRUUTSCE_RCC_QP_HPP
#define GRUUTSCE_RCC_QP_HPP

#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <memory>
#include <vector>
#include <algorithm>
#include <random>
#include <cstring>

#include <botan-2/botan/hash.h>

namespace gruut {

constexpr int HASH_TABLE_SIZE = (1 << 20);
constexpr int CACHE_SIZE = (1 << 17);
constexpr int JUMP_THRESHOLD = 100;	// hashtable jump JUMP_THRESHOLD
constexpr int VIRTUAL_VECTOER_SIZE = 8;
constexpr int LC_COUNTER_SINGLEFR_IDXSIZE = 3*1024*1024/4;			// single layer Flow Regulator를 사용할 때 변수
constexpr int LC_COUNTER_DOUBLEFRE_IDXSIZE = 3*1024*1024/4*6;		// double layer Flow Regulator를 사용할 때 변수

typedef struct input_array_entry {
  uint64_t hv;
  std::shared_ptr<input_array_entry> next;
} input_array_entry;

typedef  struct lru_entry {
  uint64_t hash_value;
  std::shared_ptr<lru_entry> prev;
  std::shared_ptr<lru_entry> next;
} lru_entry;

typedef struct entry_t {
  uint64_t hash_value;
  float total_counter;
  uint64_t timestamp;
} entry_t;

typedef struct {
  uint32_t usage;
  uint32_t size;
  uint64_t insert;
  uint64_t total_jump;
  uint64_t eviction;
  std::vector<entry_t> table;
} hashtable_t;

typedef struct {
  uint32_t vector_size;
  uint32_t memory_FRlimit;
  uint32_t memory_singleRCClimit;
  uint32_t memory_usage;
  std::vector<uint32_t> rcounter;
  hashtable_t htable;
} rcc32_t;


typedef struct chain_entry {
  uint64_t hv;
  std::shared_ptr<chain_entry> next;
} chain_entry;

typedef struct chain_hash_table
{
  uint64_t usage;
  uint64_t insert;
  uint64_t evict;
  std::vector<std::shared_ptr<chain_entry>> htable;
} chain_hash_table;

class RCCQP {
private:
  std::shared_ptr<chain_hash_table> hash_table;
  std::shared_ptr<rcc32_t> rcc;
  int rcc_qp_flag;
  std::shared_ptr<lru_entry> lru_head;
  std::shared_ptr<lru_entry> lru_tail;
  std::shared_ptr<input_array_entry> input_array_head;
  std::shared_ptr<input_array_entry> input_array_tail;
  std::mt19937 prng;
public:
  RCCQP(){
    std::random_device rand_device;
    prng.seed(rand_device());
  }

  int test() {
    uint64_t hv;

    input_array_init();

    chain_init();

    rcc_qp_flag = 0;
    lru_init();
    rcc_create(1);

    scanf("%" PRIu64, &hv);
    while (hv != 0) {
      input_array_insert(hv);
      hv = 0;
      scanf("%" PRIu64, &hv);
    }

    hv = input_array_delete();
    while (hv != 0)
    {
      if (chain_search(hv)){
        hv = 0;
        hv = input_array_delete();

        if (rcc_qp_flag == 1)
          rcc_qp_flag = 0;
        continue;
      }

      if (rcc_qp_flag == 1){
        chain_insert(hv);
        rcc_qp_flag = 0;
      }
      hv = 0;
      hv = input_array_delete();
    }

    return 0;
  }

private:

  void input_array_init()
  {
    input_array_head = nullptr;
    input_array_tail = nullptr;
  }

  void input_array_insert(uint64_t hv)
  {
    std::shared_ptr<input_array_entry> newEntry(new input_array_entry);
    newEntry->hv = hv;
    newEntry->next = nullptr;

    if (input_array_head == nullptr) {
      input_array_head = newEntry;
      input_array_tail = newEntry;
    }
    else {
      input_array_tail->next = newEntry;
      input_array_tail = newEntry;
    }
  }

  uint64_t input_array_delete()
  {
    if (input_array_head == nullptr)
    {
      return 0;
    }

    std::shared_ptr<input_array_entry> tmp = input_array_head;

    if (input_array_head->next == nullptr) {
      input_array_head = nullptr;
    }
    else {
      input_array_head = input_array_head->next;
    }

    return tmp->hv;
  }


  void chain_init() {

    hash_table.reset(new chain_hash_table);
    hash_table->htable.resize(HASH_TABLE_SIZE);

    for (uint32_t i = 0; i < HASH_TABLE_SIZE; ++i) {
      hash_table->htable[i] = nullptr;
    }
    hash_table->evict = 0;
    hash_table->insert = 0;
    hash_table->usage = 0;
  }

  bool chain_search(uint64_t hv)
  {
    lru_update(hv);
    singleFR_encode(hv);
    uint32_t idx = hv % HASH_TABLE_SIZE;

    std::shared_ptr<chain_entry> ptr = hash_table->htable[idx];

    if (ptr == nullptr)
      return false;

    if (ptr->hv == hv)
      return true;


    while (ptr->next != nullptr){
      ptr = ptr->next;

      if (ptr->hv == hv)
        return true;
    }
    return false;
  }

  void chain_insert(uint64_t hv)
  {
    if (hash_table->usage < CACHE_SIZE){
      ++hash_table->usage;
    }
    else{
      uint64_t evict_hv = 0;
      evict_hv = lru_evict();
      chain_delete(evict_hv);
    }


    uint32_t idx = hv % HASH_TABLE_SIZE;
    std::shared_ptr<chain_entry> temp(new chain_entry);
    temp->hv = hv;
    temp->next = hash_table->htable[idx];
    hash_table->htable[idx] = temp;

    ++hash_table->insert;

    lru_insert(hv);
  }

  void chain_delete(uint64_t hv)
  {
    uint32_t idx = hv % HASH_TABLE_SIZE;
    std::shared_ptr<chain_entry> temp, ptr;

    ptr = hash_table->htable[idx];
    if (ptr == nullptr)
    {
      return;
    }

    if (ptr->hv == hv)
    {
      if (ptr->next != nullptr)
      {
        hash_table->htable[idx] = ptr->next;
      }
      else
      {
        hash_table->htable[idx] = nullptr;
      }
      ++(hash_table->evict);
      return;
    }

    while (ptr->next != nullptr)
    {
      if (ptr->next->hv == hv)
      {
        temp = ptr->next;
        ptr->next = temp->next;
        ++(hash_table->evict);
        return;
      }

      ptr = ptr->next;
    }
  }


  void lru_init() {
    lru_head = nullptr;
    lru_tail = nullptr;
  }

  void lru_insert(uint64_t hv) {
    std::shared_ptr<lru_entry> newEntry(new lru_entry);
    newEntry->hash_value = hv;

    if (lru_head == nullptr) {
      lru_tail = newEntry;
    }
    else {
      lru_head->prev = newEntry;
    }

    newEntry->next = lru_head;
    lru_head = newEntry;
  }

  uint64_t lru_evict() {
    std::shared_ptr<lru_entry> tmp = lru_tail;

    if (lru_head == nullptr)
      return 0;

    if (lru_head->next == nullptr)
      lru_head = nullptr;
    else
      lru_tail->prev->next = nullptr;

    lru_tail = lru_tail->prev;

    return tmp->hash_value;
  }

  void lru_update(uint64_t hv) {

    if (lru_head == nullptr) {
      return;
    }

    std::shared_ptr<lru_entry> current = lru_head;
    while (current->hash_value != hv) {
      if (current->next == nullptr)
        return;
      else
        current = current->next;
    }

    if (current == lru_head)
      lru_head = lru_head->next;
    else
      current->prev->next = current->next;

    if (current == lru_tail)
      lru_tail = current->prev;
    else
      current->next->prev = current->prev;

    lru_insert(hv);
  }


  void rcc_create(int layer) {
    rcc.reset(new rcc32_t);

    if (layer == 1)
      rcc->rcounter.resize(LC_COUNTER_SINGLEFR_IDXSIZE);
    else if (layer == 2)
      rcc->rcounter.resize(LC_COUNTER_DOUBLEFRE_IDXSIZE);

    std::fill(rcc->rcounter.begin(),rcc->rcounter.end(), 0);

    rcc->htable.table.reserve(HASH_TABLE_SIZE);
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
      rcc->htable.table[i].hash_value = 0;
      rcc->htable.table[i].total_counter = 0;
    }
    rcc->htable.size = HASH_TABLE_SIZE;
    rcc->htable.usage = 0;
    rcc->htable.insert = 0;
    rcc->htable.eviction = 0;
    rcc->htable.total_jump = 0;

    if (layer == 1)
      rcc->memory_FRlimit = LC_COUNTER_SINGLEFR_IDXSIZE;
    else if (layer == 2)
      rcc->memory_FRlimit = LC_COUNTER_DOUBLEFRE_IDXSIZE / 6;

    rcc->memory_usage = 0;
    rcc->vector_size = VIRTUAL_VECTOER_SIZE;
  }

  uint32_t get_mz(uint32_t word, uint32_t vector) {
    float i = (32 - VIRTUAL_VECTOER_SIZE) - (number_of_set_bits(word) - number_of_set_bits(vector & word));
    return (uint32_t)(i / ((32 / VIRTUAL_VECTOER_SIZE) - 1));
  }

  float get_khat(float zeros, float mzeros) {
        float Vs = (zeros / (VIRTUAL_VECTOER_SIZE - 1));
    float Vm = ((mzeros) / (VIRTUAL_VECTOER_SIZE));
    float cs = log(1.0 - 1.0 / (VIRTUAL_VECTOER_SIZE));			  // constant for vs
    float k2 = -(VIRTUAL_VECTOER_SIZE - 1)*log(Vs);
    float k1 = log(Vm) / cs;
    float khat = k2 - k1;
    khat = (khat < 0) ? 0 : (khat);
    return khat;
  }

  int get_bitmask_of_d_index(int vector, int D)
  {
    if (D < 0)
      return 0;

    while (D-- > 0)
      vector &= vector - 1;

    return vector & -vector;    // a word that contains the D's bit in the virtual vector
  }

  int number_of_set_bits(uint32_t i)
  {
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
  }

/* Insert a hash_value-value pair into a hash table. */

  void ht_set_4(hashtable_t &hashtable, uint64_t hash_value, float est)
  {
    ++(hashtable.insert);
    uint32_t loc = 0;
    int64_t qp = -1;
    int expired_loc = -1;
    uint32_t min = -1;
    int hsize = hashtable.size;

    for (qp = 0; qp < JUMP_THRESHOLD; ++qp)
    {
      // prepare set to another place
      loc = (hash_value + (qp + qp*qp) / 2) % hsize; // hash_value + 0.5i+ 0.5i^2
      entry_t &me = hashtable.table[loc];
      if (me.hash_value == 0)
      {
        me.hash_value = hash_value;
        me.total_counter = est;

        hashtable.total_jump += (qp + 1);
        ++(hashtable.usage);
        return;
      }
      else if (me.hash_value == hash_value)
      {
        me.total_counter += est;

        hashtable.total_jump += (qp + 1);
        return;
      }
        // eviction target search
      else if ((me.total_counter) < min) {
        min = me.total_counter;
        expired_loc = loc;
      }
    }

    hashtable.total_jump += JUMP_THRESHOLD;
    hashtable.eviction++;
    hashtable.table[expired_loc].hash_value = hash_value;  // expired
    hashtable.table[expired_loc].total_counter = est;  // expired
  }

/* single layer Flow Regulator에 입력 */
  void singleFR_encode(uint64_t hv)
  {
    float est;
    uint64_t temp, rehash;
    uint32_t composed_word, zeros, mzeros, bit_mask, test, vector, last_vector, A_index;
    int i, left, right;

    left = -1, right = 59;
    last_vector = 0;
    vector = 0;
    rehash = hv;
    i = 0;
    while (i < VIRTUAL_VECTOER_SIZE){
      ++left;
      temp = rehash;
      temp = (temp << left);
      temp = (temp >> right);
      vector |= (0x1 << temp);
      if (last_vector != vector){
        i++;
        last_vector = vector;
      }
      if (left == 59){
        rehash = hash(rehash);
        left = -1;
      }
    }

    A_index = (hv % (rcc->memory_FRlimit));

    std::uniform_int_distribution<> rand_dist(0,VIRTUAL_VECTOER_SIZE-1);

    //set random bit
    i = rand_dist(prng);
    composed_word = rcc->rcounter[A_index] | get_bitmask_of_d_index(vector, i);
    zeros = VIRTUAL_VECTOER_SIZE - number_of_set_bits(vector & composed_word);

    if (zeros < ((double)VIRTUAL_VECTOER_SIZE * 0.3)) {
      mzeros = get_mz(composed_word, vector);
      zeros = 2;
      if (mzeros <= 3)
      {
        mzeros = VIRTUAL_VECTOER_SIZE;
      }

      while (zeros < mzeros)
      {
        i = rand_dist(prng);
        bit_mask = get_bitmask_of_d_index(vector, i);
        test = bit_mask & composed_word;
        if (test == bit_mask){
          composed_word ^= bit_mask;
          ++zeros;
        }
      }
      rcc->rcounter[A_index] = composed_word;
      est = get_khat(2, mzeros) + 1;

      rcc_qp_flag = 1;
      ht_set_4(rcc->htable, hv, est);
    }
    rcc->rcounter[A_index] = composed_word;
  }

  uint64_t hash(uint64_t msg_int) {

    std::vector<uint8_t> msg_bytes;
    auto input_size = sizeof(msg_int);

    msg_bytes.reserve(input_size);

    for (auto i = 0; i < input_size; ++i) {
      msg_bytes.push_back(static_cast<uint8_t>(msg_int & 0xFF));
      msg_int >>= 8;
    }

    unique_ptr<Botan::HashFunction> hash_function(Botan::HashFunction::create("SHA-256"));
    hash_function->update(msg_bytes);

    std::vector<uint8_t> hash_result = hash_function->final_stdvec();

    uint64_t ret_val;

    std::memcpy(&ret_val, hash_result.data(), sizeof(uint64_t));

    return ret_val;
  }

};

}

#endif //GRUUTSCE_RCC_QP_HPP
