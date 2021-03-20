//
// Created by zhenya on 20.03.2021.
//
#include "hash_calculator.hpp"

const size_t begin_pos = 60;
const size_t length = 4;
const char null = '0';

int hash_calculator::calculate_hash() {
  while (!close_threads){
    std::string data = std::to_string(std::rand());
    std::string hash = picosha2::hash256_hex_string(data);
    if (hash.substr(begin_pos, length).find_first_not_of(null) == std::string::npos){
      BOOST_LOG_TRIVIAL(info) << "Rand: " << data << std::endl << hash;
      if (json_struct){
        json_mutex.lock();
        print_to_json(data, hash);
        json_mutex.unlock();
      }
    } else {
      BOOST_LOG_TRIVIAL(trace) << "Rand: " << data << std::endl << hash;
    }
  }
  return 0;
}

const std::string timestamp_str = "Timestamp";
const std::string hash_str = "Hash";
const std::string data_str = "Data";

void hash_calculator::print_to_json(const std::string &data, const std::string &hash){
  json_struct->json->emplace_back( nlohmann::json{
      {data_str, data},
      {hash_str, hash},
      {timestamp_str, to_simple_string(boost::posix_time::microsec_clock::local_time())}
  });
}

hash_calculator::json_structure::json_structure(const std::string &file_name) {
  json_file = new std::ofstream(file_name);
  json = new nlohmann::json;
  *json = nlohmann::json::array();
}

hash_calculator::json_structure::~json_structure() {
  *json_file << json->dump(4); // dump  чтобы json красиво выводился, 4 - отступ
  if (json) delete json;
  if (json_file){
    if (json_file->is_open()) json_file->close();
    delete json_file;
  }
}