//
// Created by zhenya on 20.03.2021.
//
#ifndef MULTITHREADS_HASH_CALCULATOR_HPP
#define MULTITHREADS_HASH_CALCULATOR_HPP
#include <thread>
#include <iostream>
#include "boost/log/trivial.hpp"
#include "boost/log/sinks.hpp"
#include "boost/log/core.hpp"
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/expressions.hpp>
#include <empty_deleter.hpp>
#include <boost/log/expressions/keyword.hpp>
#include <boost/log/support/date_time.hpp>
#include "../third-party/PicoSHA2/picosha2.h"
#include <fstream>
#include <atomic>
#include <mutex>
#include "nlohmann/json.hpp"

class hash_calculator {
 public:
  int calculate_hash();
  void print_to_json(const std::string &data, const std::string &hash);
  static std::atomic<bool> close_threads;
  static std::mutex json_mutex;
  static struct json_structure{
    nlohmann::json* json; //указатель на объект json, туда пишут все
    std::ofstream* json_file; // указатель на поток файлового вывода, туда пишется один раз в конце программы из объекта json
    explicit json_structure(const std::string& file_name);
    ~json_structure();
  } *json_struct;
};

#endif  // MULTITHREADS_HASH_CALCULATOR_HPP
