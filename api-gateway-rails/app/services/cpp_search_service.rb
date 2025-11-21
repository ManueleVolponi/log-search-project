# app/services/cpp_search_service.rb

class CppSearchService
  CPP_EXECUTABLE = CPP_EXECUTABLE_PATH
  LOG_FILE = LOG_FILE_PATH
  
  def self.call(query_string)
    require 'open3'

    cmd = "#{CPP_EXECUTABLE_PATH} #{LOG_FILE_PATH}  \"#{query_string}\""

    stdout, stderr, status = Open3.capture3(cmd)

    unless status.success?
      Rails.logger.error("C++ Search Error (Status: #{status.exitstatus}): #{stderr}")
      raise StandardError, "Search process failed with status #{status.exitstatus}. Details: #{stderr}" 
    end

    begin
      return JSON.parse(stdout) 
    rescue JSON::ParserError => e
      Rails.logger.error("Failed to parse JSON output: #{e.message}. Output was: #{stdout.inspect}")
      raise StandardError, "Invalid JSON received from search process."
    end
  end
end