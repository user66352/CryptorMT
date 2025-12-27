#ifndef GLOBAL_H
#define GLOBAL_H

const int maxPasswordLength = 64;
const int keyFileLengthMin = 2;
const int keyFileMaxBytes = 4992;

struct Options {
    std::string infile = "";
    std::string outfile = "";
    std::string keyfile = "";
    std::string pw = "";
    bool help = false;
    bool version = false;
    bool encrypt = false;
    bool decrypt = false;
    bool stdi = false;
    bool stdo = false;
};

#endif