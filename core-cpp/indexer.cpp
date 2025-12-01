// indexer.cpp (Corretto)

#include <iostream>
#include <string>
#include "index_manager.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <log_file_input> <index_file_output>" << std::endl;
        return 1;
    }

    std::string log_file_path = argv[1];
    std::string index_output_path = argv[2];
    
    IndexManager index_manager;

    auto [index, log_lines] = index_manager.create_index(log_file_path);

    index_manager.save_index(index_output_path, index, log_lines);

    std::cout << "Index successfully created and saved to: " << index_output_path << std::endl;

    return 0;
}