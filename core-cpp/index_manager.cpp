#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <map>
#include <iterator>
#include <regex>
#include "index_manager.hpp"

using namespace std;
using InvertedIndex = std::unordered_map<std::string, std::vector<int>>;

std::vector<string> IndexManager::_split(std::string s, char delimiter)
{
    std::vector<string> tokens;
    std::string tmp_token;
    std::istringstream tokenStream(s);

    while (std::getline(tokenStream, tmp_token, delimiter))
    {
        tokens.push_back(tmp_token);
    }
    return tokens;
}

std::string IndexManager::_inferFieldType(const std::string &token)
{
    if (token.length() > 0 && token[0] == '[')
    {
        return "TIMESTAMP";
    }

    if (std::count(token.begin(), token.end(), '.') >= 3 &&
        std::all_of(token.begin(), token.end(),
                    [](char c)
                    { return std::isdigit(c) || c == '.'; }))
    {
        return "IP";
    }

    if (token.length() > 0 && token[0] == '/')
    {
        return "ENDPOINT";
    }

    if (token == "GET" || token == "POST" || token == "PUT" || token == "DELETE" || token == "PATCH")
    {
        return "METHOD";
    }

    if (token.find("HTTP/") == 0)
    {
        return "VERSION";
    }

    if (token.length() > 2 && token.substr(token.length() - 2) == "ms")
    {
        std::string num_part = token.substr(0, token.length() - 2);
        if (std::all_of(num_part.begin(), num_part.end(), ::isdigit))
        {
            return "MS";
        }
    }

    if (std::all_of(token.begin(), token.end(), ::isdigit))
    {
        try
        {
            int status = std::stoi(token);
            if (status >= 100 && status <= 599)
            {
                return "STATUS";
            }
        }
        catch (const std::exception &e)
        {
            throw make_exception_ptr(e);
        }
    }

    return "FULL_TEXT";
}

void IndexManager::_mapTokens(InvertedIndex &index, const std::vector<std::string> &tokens, int line_id)
{
    const int NGRAM_SIZE = 3;

    for (size_t i = 0; i < tokens.size(); ++i)
    {
        const std::string &token = tokens[i];

        std::string field_type = _inferFieldType(token);

        std::string indexed_key = field_type + ":" + token;
        index[indexed_key].push_back(line_id);

        if (field_type != "STATUS" && field_type != "VERSION")
        {
            if (token.length() >= NGRAM_SIZE)
            {
                for (size_t j = 0; j <= token.length() - NGRAM_SIZE; ++j)
                {
                    std::string ngram = token.substr(j, NGRAM_SIZE);

                    std::string ngram_key = "NGRAM:" + ngram;
                    index[ngram_key].push_back(line_id);
                }
            }
        }
    }
}

std::vector<int> IndexManager::_intereset_vectors(const std::vector<int> &v1, const std::vector<int> &v2)
{
    std::vector<int> result;

    std::set_intersection(v1.begin(), v1.end(),
                          v2.begin(), v2.end(),
                          std::back_inserter(result));
    return result;
}

pair<InvertedIndex, std::vector<string>> IndexManager::create_index(const std::string &filename)
{
    ifstream file(filename);
    std::vector<string> original_lines;
    InvertedIndex index;

    std::string line;
    int line_id = 0;

    while (std::getline(file, line))
    {
        original_lines.push_back(line);

        std::vector<string> tokens = _split(line, ',');

        if (tokens.size() >= 7)
        {
            _mapTokens(index, tokens, line_id);
        }

        line_id++;
    }

    return {index, original_lines};
}

std::vector<int> IndexManager::search_index(const InvertedIndex &index, const std::string &query)
{

    std::vector<std::string> search_terms = _split(query, ' ');

    std::vector<std::vector<int>> results_lists;

    for (const std::string &term : search_terms)
    {
        if (term.empty())
            continue;

        size_t colon_pos = term.find(':');

        if (colon_pos != std::string::npos)
        {
            if (index.count(term))
            {
                results_lists.push_back(index.at(term));
            }
        }
        else
        {
            std::string field_type = _inferFieldType(term);
            std::string exact_key = field_type + ":" + term;

            if (index.count(exact_key))
            {
                results_lists.push_back(index.at(exact_key));
            }
            else if (term.length() >= 3)
            {
                std::vector<int> current_ngram_results;
                bool first_ngram = true;

                for (size_t j = 0; j <= term.length() - 3; ++j)
                {
                    std::string ngram = term.substr(j, 3);
                    std::string ngram_key = "NGRAM:" + ngram;

                    if (index.count(ngram_key))
                    {
                        const std::vector<int> &list = index.at(ngram_key);

                        if (first_ngram)
                        {
                            current_ngram_results = list;
                            first_ngram = false;
                        }
                        else
                        {
                            current_ngram_results = _intereset_vectors(current_ngram_results, list);
                        }
                    }
                    else
                    {
                        current_ngram_results.clear();
                        break;
                    }
                }
                if (!current_ngram_results.empty() || first_ngram)
                {
                    results_lists.push_back(current_ngram_results);
                }
            }
        }
    }

    if (results_lists.empty())
    {
        return {};
    }

    std::vector<int> final_results = results_lists[0];
    for (size_t i = 1; i < results_lists.size(); ++i)
    {
        final_results = _intereset_vectors(final_results, results_lists[i]);
    }

    return final_results;
}

std::string IndexManager::jsonEscape(const std::string &str)
{
    std::string escaped_str;
    escaped_str.reserve(str.length());
    for (char c : str)
    {
        switch (c)
        {
        case '"':
            escaped_str += "\\\"";
            break;
        case '\\':
            escaped_str += "\\\\";
            break;
        case '\b':
            escaped_str += "\\b";
            break;
        case '\f':
            escaped_str += "\\f";
            break;
        case '\n':
            escaped_str += "\\n";
            break;
        case '\r':
            escaped_str += "\\r";
            break;
        case '\t':
            escaped_str += "\\t";
            break;
        default:
            if (c > 31)
            {
                escaped_str += c;
            }
            break;
        }
    }
    return escaped_str;
}

void IndexManager::save_index(
    const std::string& index_output_path,
    const InvertedIndex& index,
    const std::vector<std::string>& log_lines
) 
{
    std::ofstream outfile(index_output_path, std::ios::binary);

    if (!outfile.is_open()) {
        std::cerr << "ERROR: Cannot open output file for saving index: " << index_output_path << std::endl;
        return;
    }

    size_t line_count = log_lines.size();
    outfile.write(reinterpret_cast<const char*>(&line_count), sizeof(line_count));

    for (const auto& line : log_lines) {
        size_t size = line.size();
        outfile.write(reinterpret_cast<const char*>(&size), sizeof(size));
        outfile.write(line.data(), size);
    }
    
    size_t index_size = index.size();
    outfile.write(reinterpret_cast<const char*>(&index_size), sizeof(index_size));

    for (const auto& pair : index) {
        const std::string& key = pair.first;
        size_t key_size = key.size();
        outfile.write(reinterpret_cast<const char*>(&key_size), sizeof(key_size));
        outfile.write(key.data(), key_size);

        const std::vector<int>& ids = pair.second;
        size_t ids_count = ids.size();
        outfile.write(reinterpret_cast<const char*>(&ids_count), sizeof(ids_count));
        
        if (ids_count > 0) {
            outfile.write(reinterpret_cast<const char*>(ids.data()), ids_count * sizeof(int));
        }
    }

    outfile.close();
}

std::pair<InvertedIndex, std::vector<std::string>> IndexManager::load_index(const std::string& index_input_path) 
{
    InvertedIndex index;
    std::vector<std::string> log_lines;
    
    std::ifstream infile(index_input_path, std::ios::binary);

    if (!infile.is_open()) {
        std::cerr << "ERROR: Cannot open input file for loading index: " << index_input_path << std::endl;
        return {index, log_lines}; 
    }

    size_t line_count = 0;
    infile.read(reinterpret_cast<char*>(&line_count), sizeof(line_count));

    log_lines.reserve(line_count);
    for (size_t i = 0; i < line_count; ++i) {
        size_t size = 0;
        infile.read(reinterpret_cast<char*>(&size), sizeof(size));
        
        std::string line(size, '\0');
        infile.read(&line[0], size);
        log_lines.push_back(std::move(line));
    }

    size_t index_size = 0;
    infile.read(reinterpret_cast<char*>(&index_size), sizeof(index_size));

    for (size_t i = 0; i < index_size; ++i) {
        size_t key_size = 0;
        infile.read(reinterpret_cast<char*>(&key_size), sizeof(key_size));
        
        std::string key(key_size, '\0');
        infile.read(&key[0], key_size);

        size_t ids_count = 0;
        infile.read(reinterpret_cast<char*>(&ids_count), sizeof(ids_count));
        
        std::vector<int> ids(ids_count);

        if (ids_count > 0) {
            infile.read(reinterpret_cast<char*>(ids.data()), ids_count * sizeof(int));
        }
        
        index.emplace(std::move(key), std::move(ids));
    }

    infile.close();
    
    return {index, log_lines};
}