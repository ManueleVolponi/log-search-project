#ifndef INDEX_MANAGER_H
#define INDEX_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <map>

using InvertedIndex = std::unordered_map<std::string, std::vector<int>>;

class IndexManager {
private:
    std::vector<std::string> _split(std::string s, char delimiter);
    std::string _inferFieldType(const std::string& token);
    void _mapTokens(InvertedIndex& index, const std::vector<std::string>& tokens, int line_id);
    std::vector<int> _intereset_vectors(const std::vector<int>& v1, const std::vector<int>& v2);
    
    public:
    std::pair<InvertedIndex, std::vector<std::string>> create_index(const std::string& filename);
    std::vector<int> search_index(const InvertedIndex& index, const std::string& query);
    std::string jsonEscape(const std::string& str);
    void save_index(
        const std::string& index_output_path,
        const InvertedIndex& index,
        const std::vector<std::string>& log_lines
    );
    std::pair<InvertedIndex, std::vector<std::string>> load_index(const std::string& index_input_path);
};

#endif