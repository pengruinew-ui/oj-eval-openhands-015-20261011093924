#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <set>
#include <sstream>
#include <iomanip>
#include <sys/stat.h>
#include <sys/types.h>

class FileStorageDB {
private:
    static const int NUM_BUCKETS = 1024;
    std::string base_dir = ".";
    
    // Hash function to map keys to buckets
    size_t hash_key(const std::string& key) {
        std::hash<std::string> hasher;
        return hasher(key) % NUM_BUCKETS;
    }
    
    // Get file path for a bucket
    std::string get_bucket_path(size_t bucket_id) {
        return "bucket_" + std::to_string(bucket_id) + ".txt";
    }
    
    // Ensure storage directory exists
    void ensure_directory() {
        // Current directory always exists
    }
    
    // Check if key-value pair exists in bucket file
    bool exists_in_bucket(size_t bucket_id, const std::string& key, int value) {
        std::string filepath = get_bucket_path(bucket_id);
        std::ifstream file(filepath);
        
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        std::string target = key + ":" + std::to_string(value);
        while (std::getline(file, line)) {
            if (line == target) {
                return true;
            }
        }
        return false;
    }
    
    // Append key-value pair to bucket file
    void append_to_bucket(size_t bucket_id, const std::string& key, int value) {
        std::string filepath = get_bucket_path(bucket_id);
        std::ofstream file(filepath, std::ios::app);
        
        if (file.is_open()) {
            file << key << ":" << value << "\n";
        }
    }
    
    // Remove specific key-value pair from bucket file
    void remove_from_bucket(size_t bucket_id, const std::string& key, int value) {
        std::string filepath = get_bucket_path(bucket_id);
        std::string temp_filepath = filepath + ".tmp";
        
        std::ifstream file(filepath);
        std::ofstream temp_file(temp_filepath);
        
        if (!file.is_open() || !temp_file.is_open()) {
            return;
        }
        
        std::string target = key + ":" + std::to_string(value);
        std::string line;
        bool found = false;
        
        while (std::getline(file, line)) {
            if (line != target) {
                temp_file << line << "\n";
            } else {
                found = true;
            }
        }
        
        file.close();
        temp_file.close();
        
        if (found) {
            remove(filepath.c_str());
            rename(temp_filepath.c_str(), filepath.c_str());
        } else {
            remove(temp_filepath.c_str());
        }
    }
    
    // Get all values for a key from bucket file
    std::vector<int> get_values_from_bucket(size_t bucket_id, const std::string& key) {
        std::string filepath = get_bucket_path(bucket_id);
        std::ifstream file(filepath);
        std::vector<int> values;
        
        if (!file.is_open()) {
            return values;
        }
        
        std::string line;
        std::string prefix = key + ":";
        
        while (std::getline(file, line)) {
            if (line.substr(0, prefix.length()) == prefix) {
                try {
                    int value = std::stoi(line.substr(prefix.length()));
                    values.push_back(value);
                } catch (...) {
                    // Skip invalid values
                }
            }
        }
        
        std::sort(values.begin(), values.end());
        return values;
    }
    
public:
    FileStorageDB() {
        ensure_directory();
    }
    
    void insert(const std::string& key, int value) {
        size_t bucket_id = hash_key(key);
        
        // Check if key-value pair already exists
        if (exists_in_bucket(bucket_id, key, value)) {
            return; // Already exists, no need to insert
        }
        
        append_to_bucket(bucket_id, key, value);
    }
    
    void delete_entry(const std::string& key, int value) {
        size_t bucket_id = hash_key(key);
        remove_from_bucket(bucket_id, key, value);
    }
    
    std::vector<int> find(const std::string& key) {
        size_t bucket_id = hash_key(key);
        return get_values_from_bucket(bucket_id, key);
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