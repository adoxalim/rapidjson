#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <cstdint>
#include <cstddef>

using namespace rapidjson;

int FuzzOneInput(const uint8_t *data, size_t size) {
    std::string json(reinterpret_cast<const char *>(data), size);
    Document d;
    d.Parse(json.c_str());
    if (!d.HasParseError() && d.IsObject()) {
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        d.Accept(writer);
    }
    return 0;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    return FuzzOneInput(data, size);
}

#if __AFL_COMPILER
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

int main(int argc, char *argv[]) {
    FILE *file = (argc == 2) ? std::fopen(argv[1], "rb") : std::freopen(nullptr, "rb", stdin);
    if (!file) {
        std::cerr << "File cannot be opened\n";
        return EXIT_FAILURE;
    }
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    uint8_t *data = new uint8_t[size];
    fread(data, 1, size, file);
    fclose(file);
    int result = FuzzOneInput(data, size);
    delete[] data;
    return result;
}
#endif
