#include <algorithm>

class CLParser {
    private:
    std::vector<std::string> tokens;
    
    public:
    CLParser(int&, char**);
    bool Flag(const std::string&);
    std::string Option(const std::string&);
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


