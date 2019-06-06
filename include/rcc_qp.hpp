/*
 * Copyright 2018, 2019 DaeHun Nyang, DaeHong Min
 * Free for non-commercial and education use.
 * For commercial use, contact nyang@inha.ac.kr
 */

#ifndef TETHYS_SCE_RCC_QP_HPP
#define TETHYS_SCE_RCC_QP_HPP

#define USE_BOTAN_HASH 1

#include <memory>
#include <random>
#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include <list>
#include <deque>

#ifdef USE_BOTAN_HASH
#include <botan-2/botan/hash.h>
#include <cstring>
#else
#endif

namespace tethys {

using namespace std;

constexpr int HASH_TABLE_SIZE = 1048576; // 0x100000;
constexpr int CACHE_SIZE = 131072; // ‬(1 << 17);
constexpr int JUMP_THRESHOLD = 100;	// hashtable jump JUMP_THRESHOLD
constexpr int VIRTUAL_VECTOR_SIZE = 8;
constexpr int LC_COUNTER_SINGLEFR_IDXSIZE = 786432; // 3*1024*1024/4;			// single layer Flow Regulator를 사용할 때 변수
constexpr int LC_COUNTER_DOUBLEFRE_IDXSIZE = 4718592; // 3*1024*1024/4*6;		// double layer Flow Regulator를 사용할 때 변수


struct Entry_t {
  uint64_t hv;
  uint64_t timestamp;
  float total_counter;
  std::string data;
  Entry_t() : hv(0), timestamp(0), total_counter(0.0){}
};

struct HashTable_t {
  uint32_t usage;
  uint32_t size;
  uint64_t insert;
  uint64_t total_jump;
  uint64_t eviction;
  std::vector<Entry_t> table;
  HashTable_t() : usage(0), size(0), insert(0), total_jump(0), eviction(0){}
};

struct RCC32_t {
  uint32_t memory_fr_limit;
  std::vector<uint32_t> rcounter;
  HashTable_t htable;
  RCC32_t() : memory_fr_limit(0){}
};

template<typename T>
struct DataEntry {
  uint64_t hv;
  std::vector<uint8_t> rhv;
  T data;
};

template<typename T>
struct ChainHashTable {
  uint64_t usage;
  uint64_t insert;
  uint64_t evict;
  std::vector<std::list<DataEntry<T>>> htable;
};

template<typename DATA_T>
class RCCQP {
private:
  ChainHashTable<DATA_T> m_ch_table;
  RCC32_t m_rcc;
  bool m_rcc_qp_flag;
  std::list<uint64_t> m_lru_list;
  std::mt19937 prng;

public:
  RCCQP() : m_rcc_qp_flag(false){
    std::random_device rand_device;
    prng.seed(rand_device());
    clear();
  }

  ~RCCQP(){
    chainClear();
  }

  void clear(){
    chainInit();
    m_rcc_qp_flag = false;
    lruInit();
    rccCreate(1);
  }

  bool push(const std::string &key, const DATA_T& data) {
    std::vector<uint8_t> rhv;
    auto hv = hash(key,rhv);

    if (chainSearch(hv,rhv)){
      if (m_rcc_qp_flag)
        m_rcc_qp_flag = false;

      return chainUpdate(hv,rhv,data);
    }

    if (m_rcc_qp_flag){
      chainInsert(hv, rhv, data);
      m_rcc_qp_flag = false;
      return true;
    }

    return false;
  }

  std::optional<DATA_T> get(const std::string &key){
    std::vector<uint8_t> rhv;
    auto hv = hash(key,rhv);

    return chainGet(hv,rhv);
  }

  void erase(const std::string &key) {
    std::vector<uint8_t> rhv;
    auto hv = hash(key,rhv);
    chainDelete(hv);
  }


private:

  void chainClear(){
    for(auto &each : m_ch_table.htable)
      each.clear();
    m_ch_table.htable.clear();
  }

  void chainInit() {
    chainClear();
    m_ch_table.htable.resize(HASH_TABLE_SIZE);
    m_ch_table.evict = 0;
    m_ch_table.insert = 0;
    m_ch_table.usage = 0;
  }

  bool chainSearch(uint64_t hv, std::vector<uint8_t> &rhv){
    lruUpdate(hv);
    singleFREncode(hv);
    uint32_t idx = hv % HASH_TABLE_SIZE;

    m_ch_table.htable[idx];

    if (m_ch_table.htable[idx].empty())
      return false;

    for (auto itr = m_ch_table.htable[idx].begin(); itr != m_ch_table.htable[idx].end(); ++itr) {
      if((*itr).rhv == rhv)
        return true;
    }

    return false;
  }

  bool chainUpdate(uint64_t hv, std::vector<uint8_t> &rhv, const DATA_T&data){
    uint32_t idx = hv % HASH_TABLE_SIZE;

    m_ch_table.htable[idx];

    if (m_ch_table.htable[idx].empty())
      return false;

    for (auto itr = m_ch_table.htable[idx].begin(); itr != m_ch_table.htable[idx].end(); ++itr) {
      if((*itr).rhv == rhv) {
        (*itr).data = data;
        return true;
      }
    }

    return false;
  }

  std::optional<DATA_T> chainGet(uint64_t hv, std::vector<uint8_t> &rhv){

    uint32_t idx = hv % HASH_TABLE_SIZE;

    m_ch_table.htable[idx];

    if (m_ch_table.htable[idx].empty()) {
      return std::nullopt;
    }

    DATA_T ret_data;

    bool found = false;

    for (auto itr = m_ch_table.htable[idx].begin(); itr != m_ch_table.htable[idx].end(); ++itr) {
      if ((*itr).rhv == rhv) {
        ret_data = (*itr).data;
        found = true;
        break;
      }
    }

    if(found)
      return ret_data;

    return std::nullopt;

  }

  void chainInsert(uint64_t hv, std::vector<uint8_t> &rhv, const DATA_T& data){
    if (m_ch_table.usage < CACHE_SIZE){
      ++m_ch_table.usage;
    }
    else{
      uint64_t evict_hv = lruEvict();
      chainDelete(evict_hv);
    }

    uint32_t idx = hv % HASH_TABLE_SIZE;
    DataEntry<DATA_T> tmp_data_entry;
    tmp_data_entry.hv = hv;
    tmp_data_entry.rhv = rhv;
    tmp_data_entry.data = data;
    m_ch_table.htable[idx].emplace_back(tmp_data_entry);
    ++m_ch_table.insert;

    lruInsert(hv);
  }

  void chainDelete(uint64_t hv){
    uint32_t idx = hv % HASH_TABLE_SIZE;

    if (m_ch_table.htable[idx].empty())
      return;

    for (auto itr = m_ch_table.htable[idx].begin(); itr != m_ch_table.htable[idx].end(); ++itr) {
      if((*itr).hv == hv) {
        m_ch_table.htable[idx].erase(itr);
      }
    }
  }


  void lruInit() {
    m_lru_list.clear();
  }

  void lruInsert(uint64_t hv) {
    m_lru_list.push_front(hv);
  }

  uint64_t lruEvict() {
    uint64_t ret_val = 0;
    if(!m_lru_list.empty()) {
      ret_val = m_lru_list.back();
      m_lru_list.pop_back();
    }
    return ret_val;
  }

  void lruUpdate(uint64_t hv) {

    for (auto itr = m_lru_list.begin(); itr != m_lru_list.end(); ++itr) {
      if((*itr) == hv) {
        m_lru_list.erase(itr);
        break;
      }
    }

    lruInsert(hv);
  }


  void rccCreate(int layer) {

    if (layer == 1)
      m_rcc.rcounter.resize(LC_COUNTER_SINGLEFR_IDXSIZE);
    else if (layer == 2)
      m_rcc.rcounter.resize(LC_COUNTER_DOUBLEFRE_IDXSIZE);

    std::fill(m_rcc.rcounter.begin(),m_rcc.rcounter.end(), 0);

    m_rcc.htable.table.reserve(HASH_TABLE_SIZE);
    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
      m_rcc.htable.table[i].hv = 0;
      m_rcc.htable.table[i].total_counter = 0;
    }
    m_rcc.htable.size = HASH_TABLE_SIZE;
    m_rcc.htable.usage = 0;
    m_rcc.htable.insert = 0;
    m_rcc.htable.eviction = 0;
    m_rcc.htable.total_jump = 0;

    if (layer == 1)
      m_rcc.memory_fr_limit = LC_COUNTER_SINGLEFR_IDXSIZE;
    else if (layer == 2)
      m_rcc.memory_fr_limit = LC_COUNTER_DOUBLEFRE_IDXSIZE / 6;
  }

  inline uint32_t getMz(uint32_t word, uint32_t vector) {
    float i = (32 - VIRTUAL_VECTOR_SIZE) - (getNumbSetBits(word) - getNumbSetBits(vector & word));
    return (uint32_t)(i / ((32 / VIRTUAL_VECTOR_SIZE) - 1));
  }

  float getKhat(float zeros, float mzeros) {
    float Vs = (zeros / (VIRTUAL_VECTOR_SIZE - 1));
    float Vm = ((mzeros) / (VIRTUAL_VECTOR_SIZE));
    float cs = log(1.0 - 1.0 / (VIRTUAL_VECTOR_SIZE)); // constant for tt
    float k2 = -(VIRTUAL_VECTOR_SIZE - 1)*log(Vs);
    float k1 = log(Vm) / cs;
    float khat = k2 - k1;
    khat = (khat < 0) ? 0 : (khat);
    return khat;
  }

  inline int getBitmaskDIndex(int vector, int D){
    if (D < 0)
      return 0;

    while (D-- > 0)
      vector &= vector - 1;

    return vector & -vector;    // a word that contains the D's bit in the virtual vector
  }

  inline int getNumbSetBits(uint32_t i){
    i = i - ((i >> 1) & 0x55555555);
    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
  }

/* Insert a hv-value pair into a hash table. */

  void htSet4(uint64_t hash_value, float est){
    ++(m_rcc.htable.insert);
    uint32_t loc = 0;
    uint32_t min = -1;
    int expired_loc = -1;
    int hsize = m_rcc.htable.size;

    for (int64_t qp = 0; qp < JUMP_THRESHOLD; ++qp) {
      // prepare set to another place
      loc = (hash_value + (qp + qp*qp) / 2) % hsize; // hv + 0.5i+ 0.5i^2
      Entry_t &me = m_rcc.htable.table[loc];
      if (me.hv == 0){
        me.hv = hash_value;
        me.total_counter = est;

        m_rcc.htable.total_jump += (qp + 1);
        ++(m_rcc.htable.usage);
        return;
      }
      else if (me.hv == hash_value){
        me.total_counter += est;

        m_rcc.htable.total_jump += (qp + 1);
        return;
      }
        // eviction target search
      else if ((me.total_counter) < min) {
        min = me.total_counter;
        expired_loc = loc;
      }
    }

    m_rcc.htable.total_jump += JUMP_THRESHOLD;
    m_rcc.htable.eviction++;
    m_rcc.htable.table[expired_loc].hv = hash_value;  // expired
    m_rcc.htable.table[expired_loc].total_counter = est;  // expired
  }

/* single layer Flow Regulator에 입력 */
  void singleFREncode(uint64_t hv){
    int left = -1;
    int right = 59;
    uint32_t last_vector = 0;
    uint32_t vector = 0;
    uint64_t rehash = hv;
    int i = 0;
    std::vector<uint8_t> dummy_rhv;

    uint64_t temp;
    while (i < VIRTUAL_VECTOR_SIZE){
      ++left;
      temp = rehash;
      temp = (temp << left);
      temp = (temp >> right);
      vector |= (0x1 << temp);
      if (last_vector != vector){
        ++i;
        last_vector = vector;
      }
      if (left == 59){
        rehash = hash(rehash, dummy_rhv);
        left = -1;
      }
    }

    uint32_t a_idx = (hv % (m_rcc.memory_fr_limit));

    std::uniform_int_distribution<> rand_dist(0,VIRTUAL_VECTOR_SIZE-1);

    //set random bit
    i = rand_dist(prng);
    uint32_t composed_word = m_rcc.rcounter[a_idx] | getBitmaskDIndex(vector, i);
    uint32_t zeros = VIRTUAL_VECTOR_SIZE - getNumbSetBits(vector & composed_word);

    if (zeros < ((double)VIRTUAL_VECTOR_SIZE * 0.3)) {
      uint32_t mzeros = getMz(composed_word, vector);
      zeros = 2;
      if (mzeros <= 3)
        mzeros = VIRTUAL_VECTOR_SIZE;

      uint32_t bit_mask, test;

      while (zeros < mzeros){
        i = rand_dist(prng);
        bit_mask = getBitmaskDIndex(vector, i);
        test = bit_mask & composed_word;
        if (test == bit_mask){
          composed_word ^= bit_mask;
          ++zeros;
        }
      }

      m_rcc.rcounter[a_idx] = composed_word;
      float est = getKhat(2, mzeros) + 1;

      m_rcc_qp_flag = true;
      htSet4(hv, est);
    }
    m_rcc.rcounter[a_idx] = composed_word;
  }

  uint64_t hash(const std::string &key, std::vector<uint8_t> &rhv) {
    std::vector<uint8_t> byte_data(key.begin(), key.end());

    uint64_t ret_val;

#ifdef USE_BOTAN_HASH
    std::unique_ptr<Botan::HashFunction> hash_function(Botan::HashFunction::create("SHA-256"));
    hash_function->update(byte_data);

    rhv = hash_function->final_stdvec();
    std::memcpy(&ret_val, rhv.data(), sizeof(uint64_t));
#else

#endif

    return ret_val;

  }

  uint64_t hash(uint64_t msg_int, std::vector<uint8_t> &rhv) {

    std::vector<uint8_t> msg_bytes;
    auto input_size = sizeof(msg_int);

    msg_bytes.reserve(input_size);

    for (auto i = 0; i < input_size; ++i) {
      msg_bytes.push_back(static_cast<uint8_t>(msg_int & 0xFF));
      msg_int >>= 8;
    }

    uint64_t ret_val;

#ifdef USE_BOTAN_HASH
    std::unique_ptr<Botan::HashFunction> hash_function(Botan::HashFunction::create("SHA-256"));
    hash_function->update(msg_bytes);

    rhv = hash_function->final_stdvec();
    std::memcpy(&ret_val, rhv.data(), sizeof(uint64_t));
#else

#endif

    return ret_val;
  }

};

}

#endif //GRUUTSCE_RCC_QP_HPP
