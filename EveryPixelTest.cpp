#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <thread>
#include <mutex>
#include <filesystem>

#define replacements_vector std::vector<std::pair<std::string, std::string>>
#define CONFIG_FILE_NAME "config.txt"

namespace fs = std::filesystem;
std::mutex mtx;

// структура для хранения информации из конфигурационного файла
struct ConfigData {
    int numThreads = 0;
    std::string rootDir;
    replacements_vector replacements; // список пар шаблон-замена
};

// чтение конфигурационного файла
ConfigData readConfig(std::string configPath) {
    ConfigData config;
    std::ifstream configFile(configPath);
    if (!configFile.is_open()) {
        std::cerr << "Error: Cannot open configuration file!" << std::endl;
        exit(1);
    }
    std::string line;
    getline(configFile, line);
    config.numThreads = stoi(line.substr(line.find("=") + 1));
    getline(configFile, line);
    config.rootDir = line.substr(line.find("=") + 1);
    while (getline(configFile, line)) {
        std::string from = line.substr(0, line.find("="));
        std::string to = line.substr(line.find("=") + 1);
        config.replacements.push_back(std::make_pair(from, to));
    }
    configFile.close();
    return config;
}

// поиск всех файлов в директории
void parseDirectory(std::string rootDir, std::vector<std::string>& files) {
    for (auto& entry : std::filesystem::recursive_directory_iterator(rootDir)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path().string());
        }
    }
}

// замена всех вхождений шаблонов на текст замены в файле
void replaceInFile(const std::string& filename, const replacements_vector& replacements) {
    std::ifstream inputFile(filename);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Cannot open input file " << filename << std::endl;
        return;
    }
    std::stringstream buffer;
    buffer << inputFile.rdbuf();
    std::string content = buffer.str();
    bool sampleIsFind = false;
    for (const auto& pair : replacements) {
        sampleIsFind = false;
        std::string sample = pair.first;
        std::string repl = pair.second;
        size_t pos = 0;
        while ((pos = content.find(sample, pos)) != std::string::npos) {
            content.replace(pos, sample.length(), repl);
            pos += repl.length();
            sampleIsFind = true;
        }
    }
    inputFile.close();
    if (sampleIsFind) {
        std::ofstream outputFile(filename);
        outputFile << content;
        outputFile.close();
    }
}

// обработка файлов
void processFiles(std::vector<std::string>& files, replacements_vector& replacements) {
    while (!files.empty()) {
        std::string file;
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (!files.empty()) {
                file = files.back();
                files.pop_back();
            }
        }
        if (!file.empty()) {
            replaceInFile(file, replacements);
        }
    }
}

int main() {
    ConfigData config = readConfig(CONFIG_FILE_NAME);
    int numThreads = config.numThreads;
    std::string rootDir = config.rootDir;
    replacements_vector replacements = config.replacements;

    std::vector<std::string> files;
    parseDirectory(rootDir, files);

    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(processFiles, std::ref(files), std::ref(replacements));
    }

    // ожидание завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "Work is done" << std::endl;
    
    return 0;
}