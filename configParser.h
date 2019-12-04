#ifndef H_CONFIGPARSER_H
#define H_CONFIGPARSER_H

#include <stdio.h>
#include <string>

using namespace std;

class ConfigParser
{
    std::string m_fileName;

public:
    bool        m_fnameSub;
    std::string m_argStr;

    std::string m_targetProgram;
    std::string m_pindir;
    std::string m_seeddir;
    std::string m_verifieddir;

    ConfigParser(std::string fileName);

    ~ConfigParser();

    bool parseConfigFile();

    std::string get_targetProgram();

    std::string get_pindir();

    std::string get_seeddir();

    std::string get_verifieddir();
    
    std::string get_argStr();

    bool get_fnameSub();
};

#endif
