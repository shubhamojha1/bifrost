#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
#include "MurmurHash3.h"
#include <cstring>


int main() {
    std::cout << "Bifrost - An implementation of Hash Tables in database joins" << std::endl;
    uint64_t seed = 1;
    uint64_t hash_otpt[2];
    const char *key = "hi";

    MurmurHash3_x64_128(key, (uint64_t)strlen(key), seed, hash_otpt);

    std::cout << "hashed" << hash_otpt[0] << hash_otpt[1] << std::endl;
}