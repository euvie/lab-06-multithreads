//
// Created by zhenya on 20.03.2021.
//
#include "hash_calculator.hpp"
#include <iomanip>
#include <iostream>
#include <vector>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;

BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", boost::log::trivial::severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(thread_id, "ThreadID",
boost::log::attributes::current_thread_id::value_type)

void logging_init(){
  typedef sinks::synchronous_sink< sinks::text_ostream_backend > sink_t;
  boost::shared_ptr< sink_t > sink_console = boost::make_shared< sink_t >(); // создаем раковину
  boost::shared_ptr< std::ostream > stream(&std::clog, logging::empty_deleter()); // создаем консольный поток для раковины
  sink_console->locked_backend()->add_stream(stream);// связываешь раковину и поток
  boost::log::formatter formatter = expr::stream
      << std::setw(7) << std::setfill('0') << line_id << std::setfill(' ') << " | "
      << "T." << thread_id << " | "
      << expr::format_date_time(timestamp, "%Y-%m-%d, %H:%M:%S.%f") << " "
      << "[" << severity << "]"
      << " - " << expr::smessage;
  sink_console->set_formatter(formatter);
  logging::core::get()->add_sink(sink_console);

  typedef sinks::synchronous_sink< sinks::text_file_backend > sink_f;
  boost::shared_ptr< sinks::text_file_backend > backend =
      boost::make_shared< sinks::text_file_backend >(
          keywords::file_name = "file_%5N.log",
          keywords::rotation_size = 10 * 1024 * 1024,
          keywords::time_based_rotation =
              sinks::file::rotation_at_time_point(12, 0, 0));
  boost::shared_ptr< sink_f > sink_file(new sink_f(backend));
  sink_file->set_formatter(formatter);
  logging::core::get()->add_sink(sink_file);

  logging::add_common_attributes();
}

void safe_exit() {
  boost::shared_ptr< logging::core > core = logging::core::get();
  core->flush();
  core->remove_all_sinks();
  delete hash_calculator::json_struct;
}

void manage_params(const int& argc, char* argv[], unsigned &number_of_threads){
  const std::string json_extension = ".json";
  switch (argc){
    case 1 :
      number_of_threads = std::thread::hardware_concurrency();
      break;
    case 2 :
      try {
        number_of_threads = std::stoi(argv[1]); //string to int
      } catch (const std::invalid_argument& e) {
        hash_calculator::json_struct = new hash_calculator::json_structure(argv[1] + json_extension);
        number_of_threads = std::thread::hardware_concurrency();
      }
      break;
    case 3:
      try {
        number_of_threads = std::stoi(argv[1]);
      } catch (const std::invalid_argument& e) {
        throw std::invalid_argument("Invalid arguments: number of threads must be integer \n");
      }
      hash_calculator::json_struct = new hash_calculator::json_structure(argv[2] + json_extension);
      break;
    default:
      throw std::invalid_argument("Invalid arguments: Program needs two or less parameters \n");
  }
  std::cout << "Recommended number of threads: "
            << std::thread::hardware_concurrency() << std::endl
            << "number of threads: " << number_of_threads << std::endl;
}