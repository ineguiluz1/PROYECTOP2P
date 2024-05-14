#include <dirent.h>
#include <iostream>
#include <vector>
#include <fstream>
#ifndef FILE_H
#define FILE_H

using namespace std;

//? Funciones para trabajar con archvios del sistema

vector<string> nombreArchivosDeUnaCarpeta(string ruta);

vector<int> tamanyoArchivosDeUnVector(vector<string> archivos, string folderPath);

vector<string> obtenerExtensionDeArchivos(vector<string> archivos);

#endif
