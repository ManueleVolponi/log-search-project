#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "index_manager.hpp"
#include <ctime>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <log_file> <query>" << std::endl;
        return 1;
    }

    std::string index_input_path = argv[1];
    std::string queryString = argv[2];
    
    IndexManager index_manager;

    auto [index, log_lines] = index_manager.load_index(index_input_path);

    clock_t start = clock();
    std::vector<int> search_results = index_manager.search_index(index, queryString);
    clock_t end = clock();

    double time_in_seconds = ((double) (end - start) / CLOCKS_PER_SEC);

    double time_in_ms = time_in_seconds * 1000.0;

    std::string exec_time_value = std::to_string(time_in_ms);

    std::stringstream json_output;
    
    json_output << "{\n";
    json_output << "  \"query\": \"" << index_manager.jsonEscape(queryString) << "\",\n";
    json_output << "  \"count\": " << search_results.size() << ",\n";
    json_output << " \t\"execution time_ms\": " << exec_time_value << ",\n";
    json_output << "  \"results\": [\n";
    
    for (size_t i = 0; i < search_results.size(); ++i) {
        int line_id = search_results[i];
        
        if (line_id >= 0 && line_id < log_lines.size()) {
            std::string escaped_line = index_manager.jsonEscape(log_lines[line_id]);
            
            json_output << "    {\n";
            json_output << "      \"id\": " << line_id << ",\n";
            json_output << "      \"line\": \"" << escaped_line << "\"\n";
            json_output << "    }";
            
            if (i < search_results.size() - 1) {
                json_output << ",\n";
            } else {
                json_output << "\n";
            }
        }
    }

    json_output << "  ]\n";
    json_output << "}\n";

    std::cout << json_output.str();

    return 0;
}
