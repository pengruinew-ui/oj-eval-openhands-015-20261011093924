#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <sstream>

class FileStorageDB {
private:
    std::string data_file = "database.txt";
    
    // Read all data from file
    std::map<std::string, std::set<int>> read_all_data() {
        std::map<std::string, std::set<int>> data;
        std::ifstream file(data_file);
        
        if (!file.is_open()) {
            return data;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            size_t colon_pos = line.find(':');
            if (colon_pos != std::string::npos) {
                std::string key = line.substr(0, colon_pos);
                std::string value_str = line.substr(colon_pos + 1);
                try {
                    int value = std::stoi(value_str);
                    data[key].insert(value);
                } catch (...) {
                    // Skip invalid lines
                }
            }
        }
        
        return data;
    }
    
    // Write all data to file
    void write_all_data(const std::map<std::string, std::set<int>>& data) {
        std::ofstream file(data_file);
        
        if (!file.is_open()) {
            return;
        }
        
        for (auto it = data.begin(); it != data.end(); ++it) {
            const std::string& key = it->first;
            const std::set<int>& values = it->second;
            for (auto vit = values.begin(); vit != values.end(); ++vit) {
                file << key << ":" << *vit << "\n";
            }
        }
    }
    
public:
    void insert(const std::string& key, int value) {
        auto data = read_all_data();
        
        // Check if key-value pair already exists
        if (data[key].find(value) != data[key].end()) {
            return; // Already exists, no need to insert
        }
        
        data[key].insert(value);
        write_all_data(data);
    }
    
    void delete_entry(const std::string& key, int value) {
        auto data = read_all_data();
        
        auto it = data.find(key);
        if (it != data.end()) {
            it->second.erase(value);
            if (it->second.empty()) {
                data.erase(it);
            }
            write_all_data(data);
        }
    }
    
    std::vector<int> find(const std::string& key) {
        auto data = read_all_data();
        
        auto it = data.find(key);
        if (it != data.end()) {
            return std::vector<int>(it->second.begin(), it->second.end());
        }
        return {};
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    
    FileStorageDB db;
    
    int n;
    std::cin >> n;
    std::cin.ignore(); // Ignore the newline after n
    
    for (int i = 0; i < n; ++i) {
        std::string line;
        std::getline(std::cin, line);
        
        std::istringstream iss(line);
        std::string command;
        iss >> command;
        
        if (command == "insert") {
            std::string key;
            int value;
            iss >> key >> value;
            db.insert(key, value);
        } else if (command == "delete") {
            std::string key;
            int value;
            iss >> key >> value;
            db.delete_entry(key, value);
        } else if (command == "find") {
            std::string key;
            iss >> key;
            
            auto values = db.find(key);
            if (values.empty()) {
                std::cout << "null";
            } else {
                for (size_t i = 0; i < values.size(); ++i) {
                    if (i > 0) std::cout << " ";
                    std::cout << values[i];
                }
            }
            std::cout << "\n";
        }
    }
    
    return 0;
}