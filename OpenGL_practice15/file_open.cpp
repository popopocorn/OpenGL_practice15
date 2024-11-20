#pragma once
#include<fstream>
#include<string>
#include<iostream>
#include"file_open.h"

char* file_to_buf(const char* file_name) {
    FILE* file;
    fopen_s(&file, file_name, "r");
    if (!file) {
        printf("Failed to load shader file\n");
        return NULL;
    }


    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    // 파일 내용 읽기
    char* shaderSource = (char*)malloc(length + 1);
    size_t read_size = fread(shaderSource, 1, length, file);
    shaderSource[read_size] = '\0';

    fclose(file);


    //std::cout << shaderSource << std::endl;

    return shaderSource;
}
