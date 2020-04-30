// Copyright [2019] Tyler Mitchell

#ifndef INCLUDE_CLPARSER_H_
#define INCLUDE_CLPARSER_H_

#include <algorithm>
#include <cstdlib>
#include <string>
#include <vector>

class CLParser {
 private:
    std::vector<std::string> tokens;
    std::vector<std::string> opts;

 public:
    CLParser(int&, char**);
    ~CLParser() {}
    bool Flag(const std::string&);
    std::string Option(const std::string&);
    std::vector<std::string> MultiOption(const std::string&, int, int);
    unsigned int *rand_seed;
};

CLParser::CLParser(int &argc, char** argv) {
    for (int i = 0; i < argc; i++) {
        tokens.push_back(std::string(argv[i]));
    }
}

// parse flags (either present or not)
bool CLParser::Flag(const std::string &flag) {
    return std::find(tokens.begin(), tokens.end(), flag) != tokens.end();
}

// parse options (contain argument specifying details)
std::string CLParser::Option(const std::string &flag) {
    auto found = std::find(tokens.begin(), tokens.end(), flag);
    if (found != tokens.end() && ++found != tokens.end()) {
        return *found;
    }
    static const std::string empty("");
    return empty;
}

std::vector<std::string> CLParser::MultiOption(const std::string &flag, int depth, int first = 1) {
    if (first == 1) {
        opts.clear();
    }
    if (depth == 0) {
        return opts;
    }
    auto found = std::find(tokens.begin(), tokens.end(), flag);
    if (found != tokens.end() && ++found != tokens.end()) {
        --depth;
        opts.push_back(*found);
        *found = *found+std::to_string(rand_r(rand_seed));  // can't let it find this one again next time, so add a random ending
        MultiOption(*found, depth, 0);
    }
    return opts;
}

#endif  // INCLUDE_CLPARSER_H_

