#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <sstream>
#include <iomanip>
#include <filesystem>

class FileStorageDB {
private:
    static const int NUM_BUCKETS = 1024;
    std::string base_dir = "db_storage";
    
    // Hash function to map keys to buckets
    size_t hash_key(const std::string& key) {
        std::hash<std::string> hasher;
        return hasher(key) % NUM_BUCKETS;
    }
    
    // Get file path for a bucket
    std::string get_bucket_path(size_t bucket_id) {
        return base_dir + "/bucket_" + std::to_string(bucket_id) + ".txt";
    }
    
    // Ensure storage directory exists
    void ensure_directory() {
        std::filesystem::create_directories(base_dir);
    }
    
    // Read all entries from a bucket file
    std::unordered_map<std::string, std::set<int>> read_bucket(size_t bucket_id) {
        std::unordered_map<std::string, std::set<int>> bucket_data;
        std::string filepath = get_bucket_path(bucket_id);
        std::ifstream file(filepath);
        
        if (!file.is_open()) {
            return bucket_data;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            size_t colon_pos = line.find(':');
            if (colon_pos != std::string::npos) {
                std::string key = line.substr(0, colon_pos);
                std::string value_str = line.substr(colon_pos + 1);
                try {
                    int value = std::stoi(value_str);
                    bucket_data[key].insert(value);
                } catch (...) {
                    // Skip invalid lines
                }
            }
        }
        
        return bucket_data;
    }
    
    // Write all entries to a bucket file
    void write_bucket(size_t bucket_id, const std::unordered_map<std::string, std::set<int>>& bucket_data) {
        std::string filepath = get_bucket_path(bucket_id);
        std::ofstream file(filepath);
        
        if (!file.is_open()) {
            return;
        }
        
        for (const auto& [key, values] : bucket_data) {
            for (int value : values) {
                file << key << ":" << value << "\n";
            }
        }
    }
    
public:
    FileStorageDB() {
        ensure_directory();
    }
    
    void insert(const std::string& key, int value) {
        size_t bucket_id = hash_key(key);
        auto bucket_data = read_bucket(bucket_id);
        
        // Check if key-value pair already exists
        if (bucket_data[key].find(value) != bucket_data[key].end()) {
            return; // Already exists, no need to insert
        }
        
        bucket_data[key].insert(value);
        write_bucket(bucket_id, bucket_data);
    }
    
    void delete_entry(const std::string& key, int value) {
        size_t bucket_id = hash_key(key);
        auto bucket_data = read_bucket(bucket_id);
        
        auto it = bucket_data.find(key);
        if (it != bucket_data.end()) {
            it->second.erase(value);
            if (it->second.empty()) {
                bucket_data.erase(it);
            }
            write_bucket(bucket_id, bucket_data);
        }
    }
    
    std::vector<int> find(const std::string& key) {
        size_t bucket_id = hash_key(key);
        auto bucket_data = read_bucket(bucket_id);
        
        auto it = bucket_data.find(key);
        if (it != bucket_data.end()) {
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