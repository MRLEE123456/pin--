#include <stdio.h>
#include <stdlib.h>

#include "configParser.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <fstream>
#include <string>


ConfigParser::ConfigParser(std::string fileName)
{
    m_fileName = fileName;
}


ConfigParser::~ConfigParser()
{
}


bool ConfigParser::parseConfigFile()
{
    std::ifstream in(m_fileName.c_str());
	
    if (!in.is_open()) {
	fprintf(stderr, "fail to read json configure file: %s\n", m_fileName.c_str());
	abort();
    }

    std::string json_content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();

    rapidjson::Document dom;
    if (!dom.Parse(json_content.c_str()).HasParseError()) 
    {
	if (dom.HasMember("fnameSub") && dom["fnameSub"].IsBool()) 
	{
	    m_fnameSub = dom["fnameSub"].GetBool();
	}
	else
	{
	    return false;
	}

	if (dom.HasMember("argStr") && dom["argStr"].IsString()) 
	{
	    m_argStr = dom["argStr"].GetString();
	}
	else
	{
	    return false;
	}

	if (dom.HasMember("targetProgram") && dom["targetProgram"].IsString()) 
	{
	    m_targetProgram = dom["targetProgram"].GetString();
	}
	else
	{
	    return false;
	}

	if (dom.HasMember("pindir") && dom["pindir"].IsString()) 
	{
	    m_pindir = dom["pindir"].GetString();
	}
	else
	{
	    return false;
	}

	if (dom.HasMember("seeddir") && dom["seeddir"].IsString()) 
	{
	    m_seeddir = dom["seeddir"].GetString();
	}
	else
	{
	    return false;
	}

	if (dom.HasMember("verifieddir") && dom["verifieddir"].IsString()) 
	{
	    m_verifieddir = dom["verifieddir"].GetString();
	}
	else
	{
	    return false;
	}
        
        if (dom.HasMember("arch") && dom["arch"].IsString()) 
	{
	    m_arch = dom["arch"].GetString();
	}
	else
	{
	    return false;
	}
	return true;
    }

    return false;
}

std::string ConfigParser::get_targetProgram()
{
    return m_targetProgram;
}


std::string ConfigParser::get_pindir()
{
    return m_pindir;
}

    
std::string ConfigParser::get_seeddir()
{
    return m_seeddir;
}

    
std::string ConfigParser::get_verifieddir()
{
    return m_verifieddir;
}
    
    
std::string ConfigParser::get_argStr()
{
    return m_argStr;
}

std::string ConfigParser::get_arch()
{
    return  m_arch;
}
    
bool ConfigParser::get_fnameSub()
{
    return m_fnameSub;
}

