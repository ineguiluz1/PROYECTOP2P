#include "files.h"
#include <dirent.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring> 

vector<string> nombreArchivosDeUnaCarpeta(string folderPath){
    DIR* dir = opendir(folderPath.c_str());

    if (dir) {
        std::vector<std::string> filenames;
        while (dirent* entry = readdir(dir)) {
            // Skip . and .. entries
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                filenames.push_back(entry->d_name);
            }
        }
        closedir(dir);
        return filenames;
    } else {
        std::cerr << "Couldn't open the directory." << std::endl;
        return {};
    }
};

vector<int> tamanyoArchivosDeUnVector(vector<string> archivos, string folderPath){
    std::vector<int> tamanyos;
    for (const std::string& filename : archivos) {
            std::string file_path = folderPath + "/" + filename;
            std::ifstream in_file(file_path, std::ios::binary);
            in_file.seekg(0, std::ios::end);
            int file_size = in_file.tellg();
            tamanyos.push_back(file_size);
    }
    return tamanyos;
};

vector<string> obtenerExtensionDeArchivos(vector<string> archivos){
    std::vector<std::string> extensiones;
    for (const std::string& filename : archivos) {
        std::string extension = filename.substr(filename.find_last_of(".") + 1);
        extensiones.push_back(extension);
    }
    return extensiones;
};