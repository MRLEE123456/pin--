#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include "configParser.h"
#include <fcntl.h>

#include <sys/stat.h>

using namespace std;

int copy(const char * src, const char * des)
{
    if(access(src, F_OK) != 0)
    {
	cout << "file not found......" << endl;
	return 0;
    }
 
    int fin = open(src, O_RDONLY, 0777);
    int fout = open(des, O_WRONLY|O_CREAT, 0777);
 
    char buff[1024] = {'\0'};
    int len = 0;
    while((len = read(fin, buff, sizeof(buff))) > 0)
    {
	write(fout, buff, len);
    }
 
    close(fin);
    close(fout);
 
    return 0;
}
char * get_one_file_name(char* dirPath)
{
    static DIR * dir = NULL;
    if (dir == NULL) 
    {
	if ( !(dir = opendir(dirPath)) )
	{
    	    printf("FATAL ERROR: opendir(%s) error !\n", dirPath);
    	    abort();
	}
    }

    dirent* ptr = NULL;
    do
    {
	ptr = readdir(dir);
	if (ptr == NULL)
	{
	    closedir(dir);
	    return NULL;
	}

	printf("dname = %s\n", ptr->d_name);
	if ( strcmp(ptr->d_name,".") == 0  || 
	     strcmp(ptr->d_name,"..") == 0 ||
	     ptr->d_type == DT_DIR
	   )
	{
	    continue;
	}
	else
	{
	    return ptr->d_name;
	}
    } 
    while (1);
    
    return NULL;
}


/* USAGE:
 * ./InputFilter  Pin工具所在目录  测试用例所在目录  被测程序所在目录  被测程序  合格测试用例存放目录
 */
int main(int argc,char *argv[])
{
    ConfigParser * cfg_parser = new ConfigParser("./if-conf.json");
    if ( !cfg_parser->parseConfigFile() )
    {
	delete cfg_parser;
	return 0;
    }

    std::string pinDir  = cfg_parser->get_pindir();
    printf("pinDir = %s\n", pinDir.c_str());

    std::string seedDir = cfg_parser->get_seeddir();
    printf("seedDir = %s\n", seedDir.c_str());

    std::string exe     = cfg_parser->get_targetProgram();
    printf("exe = %s\n", exe.c_str());

    std::string verifiedDir = cfg_parser->get_verifieddir();
    printf("verifiedDir = %s\n", verifiedDir.c_str());

    std::string argStr = cfg_parser->get_argStr();
    printf("argStr = %s\n", argStr.c_str());

    bool fnameSub = cfg_parser->get_fnameSub();

    char * fileName = NULL;

    std::stringstream ss;

    std::string cmd_pin; 
    std::string cmd_copy;

    while (1)
    {
	fileName = get_one_file_name( (char *)(seedDir.c_str()) );
	if (fileName == NULL)
	{
	    break;
	}
	std::string strFileName = fileName;
	//std::string strVerifiedFileName = verifiedDir + "/" + strFileName; 
	strFileName = seedDir + "/" + strFileName;

	// synthesize the PIN command
	ss.str("");
	ss << pinDir << "/pin -t " << pinDir << "/source/tools/MyPinTool/obj-intel64/Classifier.so -- ";
	ss << exe << " " << argStr;
	cmd_pin = ss.str();
	ss.str("");
	//ss.clear();

	if (fnameSub)
	{
	    // replace @@ with fileName
	    cmd_pin = cmd_pin.replace( cmd_pin.find("@@"),
		    		       2,
				       strFileName
		   		     );
	}

	printf("pin cmd: %s\n", cmd_pin.c_str());
	system(cmd_pin.c_str());

	// classify the input
        ifstream in("classifier.out");
        string subdirname;
        if(in) // 有该文件
        {
          while (getline (in, subdirname)) // line中不包括每行的换行符
          { 
             cout << subdirname << endl;
             ss.str("");
             ss << verifiedDir << "/" <<subdirname;
             cout<< ss.str()<<endl;
             if(access(ss.str().c_str(),F_OK)!=0)
             {
                ss.str("");
                ss << "mkdir " << verifiedDir << "/" <<subdirname; 
	        std::string cmd_mkdir = ss.str(); 
                cout<< cmd_mkdir<<endl;
	        system(cmd_mkdir.c_str());
	        ss.clear();
             }
             string desFileName=verifiedDir+ "/"+ subdirname+ "/"+fileName;
            // cout<< strFileName<<endl<<desFileName<<endl;
             copy(strFileName.c_str(),desFileName.c_str());
/*
             ss.str("");
             ss << "cp " << strFileName << " " << verifiedDir<< "/"<< subdirname<< "/"; 
	     std::string cmd_copy = ss.str(); 
             cout<< cmd_copy<<endl;
	     system(cmd_copy.c_str());
	     ss.clear();
    */         
          }
        }
        else // 没有该文件
        {
              cout <<"no such file" << endl;
        }
	
    }// end of while{1}

    delete cfg_parser;
        
    return 0;
}





