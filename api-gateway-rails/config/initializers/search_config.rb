# config/intializers/search_cinfig.rb
require 'pathname'

RAILS_ROOT_PATH = Rails.root.to_s
PROJECT_ROOT_PATH = Pathname.new(RAILS_ROOT_PATH).parent.to_s


CPP_EXECUTABLE_PATH = File.join(PROJECT_ROOT_PATH, "core-cpp", "build", "searcher")
LOG_FILE_PATH = File.join(PROJECT_ROOT_PATH, "access.log")

unless File.exist?(CPP_EXECUTABLE_PATH)
  raise "C++ Executable not found at #{CPP_EXECUTABLE_PATH}. Did you run CMake and 'make' in core-cpp/?"
end

puts "INFO: C++ Executable path set to #{CPP_EXECUTABLE_PATH}"
puts "INFO: Log File path set to #{LOG_FILE_PATH}"