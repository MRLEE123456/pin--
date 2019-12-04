#include "pin.H"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include<math.h>  
#include <stdio.h>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <string>
using namespace std;
using std::cerr;
using std::string;
using std::endl;

/*!
 *  Print out help message.
 */
//FILE * classifier_f;
fstream fout("classifier.out",ios::out|ios::trunc);//输出目录信息
int pointNumer=1;//目标位置计数
struct slot
{
  INT64 uin_offset ;//实际用的偏移量
  INT64 IMG_Address;//块基地址
  string offset;
  string nodName;
  string outpath;
  slot *nest=NULL;
}*listhead;//判断点

void readjosn()//解析josn文件
{  
   slot * p1=NULL,*p2=NULL;
   std::stringstream curpoint;//当前slot点
   string m_fileName="config.json";
   string point="slot";
   std::ifstream in;
   in.open(m_fileName.c_str());	
   if (!in.is_open()) {
	fprintf(stderr, "fail to read json configure file: %s\n", m_fileName.c_str());
	abort();
       }
  std::string json_content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  in.close();
  rapidjson::Document dom;
  if (!dom.Parse(json_content.c_str()).HasParseError()) 
  {   
      curpoint.str("");
      curpoint << point << pointNumer;//eg:slot1
     // cout<<curpoint.str()<<endl;
      while(dom.HasMember(curpoint.str().c_str()))
      {   
        p1=new(slot);
        p1->outpath=dom[curpoint.str().c_str()].GetString();
        cout<<dom[curpoint.str().c_str()].GetString()<<endl;
        istringstream iss(p1->outpath);
        getline(iss, p1->nodName, '!');
        getline(iss, p1->offset, '!');
        p1->uin_offset=0;
        p1->IMG_Address=0;
        int i = 0;
        for(i=2;i<(int)p1->offset.length();i++)
        {
		 if(p1->offset[i]>='0'&&p1->offset[i]<='9')//数字字符的转换  
            {  
               p1->uin_offset+=(p1->offset[i]-48)*pow(16.0,(int)p1->offset.length()-i-1);  
            }  
            else if(p1->offset[i]>='A'&&p1->offset[i]<='F')//字母字符的转换  
            {  
                p1->uin_offset+=(p1->offset[i]-55)*pow(16.0,(int)p1->offset.length()-i-1);  
            }  
            else
            {
                p1->uin_offset+=(p1->offset[i]-87)*pow(16.0,(int)p1->offset.length()-i-1); 
            }
            
        }
       // cout<<"nodName:"<<p1->nodName;
        //printf( "offset: %lx \n", p1->uin_offset);
        if(pointNumer==1)
        { 
           listhead=p1;
           p2=listhead;
        }
        else
        {
            p2->nest=p1;
            p2=p1;             
         }
        pointNumer++;
        curpoint.str("");
        curpoint << point << pointNumer;//eg:slot1
        //cout<<curpoint.str()<<endl;         
       }
     
  }
  
  p2=NULL;
  p1=NULL;
  //cout<<(pointNumer-1)<<endl;
}
void freelist()
{
   slot *p1=NULL;
   p1=listhead;
   int i=0;
   for(i=1;i<=pointNumer-1;i++)
   { 
      p1=listhead->nest;
      delete(listhead);
      listhead=p1;
   }
 }
INT32 Usage()
{
    cerr << "This tool picks up the test cases that reach the pointing position. " << endl;

    cerr << KNOB_BASE::StringKnobSummary() << endl;

    return -1;
}
VOID classifier(VOID *ip) 
	{ 
		//printf("%p\n", ip);
                int i=0;
                slot *curslot=NULL;
                curslot=listhead;
                for(i=1;i<=pointNumer-1;i++)
               {
		  if((curslot->IMG_Address+curslot->uin_offset)==(INT64)ip)
		 {
			//fprintf(classifier_f, "%s\n",curslot->outpath);
                        fout<<curslot->outpath<<endl;
                        cout<<"hit the "<<curslot->outpath<<endl; 
		  }
                 curslot=curslot->nest;
                }
	}

VOID Instruction(INS ins, VOID *v)  
{  
    // Insert a call to docount before every instruction, no arguments are passed  
    INS_InsertCall(ins, IPOINT_BEFORE, (AFUNPTR)classifier,IARG_INST_PTR, IARG_END);  
}  

void Image(IMG img, void * v) {      
                string image="";
                image=IMG_Name(img).substr(IMG_Name(img).find_last_of("/\\") + 1);
                int i=0;
                slot *curslot=NULL;
                curslot=listhead;
                for(i=1;i<=pointNumer-1;i++)
               {
		  if(strcmp(curslot->nodName.c_str(),image.c_str())==0)
			{
				// IMG_Address=(INT64)((void *)IMG_StartAddress(img));
				curslot->IMG_Address=(INT64)((void *)IMG_LowAddress(img));                                
				// fprintf(debug_f,"catch the image%s %ld\n",image.c_str(),IMG_Address);
				printf("catch the image %s %lx\n",image.c_str(),curslot->IMG_Address);
			}
                   curslot=curslot->nest;
                 }               
		
    
}
VOID Fini(INT32 code, VOID *v)
{
    freelist();
    //fclose(classifier_f);
    fout.close();
   
}

int main(int argc,char *argv[])
{
	
	// Initialize pin 
	if(PIN_Init(argc,argv))
	{
		return Usage();
	}
        //classifier_f = fopen("classifier.out", "w");
        readjosn();
        cout<<"共解析出"<<(pointNumer-1)<<"个slotd点"<<endl;
	//Register Image to be called when a image to be loaded  
	IMG_AddInstrumentFunction(Image, 0);
        
	// Register Instruction to be called to instrument instructions  
       INS_AddInstrumentFunction(Instruction, 0);  
       
    // Register Fini to be called when the application exits  
    PIN_AddFiniFunction(Fini, 0);  
      
    // Start the program, never returns  
    PIN_StartProgram();  
      
    return 0;  
	
}
