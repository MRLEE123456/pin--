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
fstream fout_f("classifier_fun.out",ios::out|ios::trunc);//输出目录信息
fstream fout_p("classifier_point.out",ios::out|ios::trunc);//输出目录信息
fstream fout_slot("slot_num.out",ios::out|ios::trunc);//输出目标数量
int pointNumer=1;//目标位置计数
struct slot
{
  INT64 uin_offset_fun ;
  INT64 uin_offset_point;//实际用的偏移量
  INT64 IMG_Address;//块基地址
  string point_offset;
  string nodName;
  string fun_offset;
  string outpath;
  string outpath_fun;
  string outpath_point;
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
        getline(iss, p1->outpath_fun, '&');
        getline(iss, p1->outpath_point, '&');
        istringstream iss1(p1->outpath_fun);
        getline(iss1, p1->nodName, '!');
        getline(iss1, p1->fun_offset, '!');
	istringstream iss2(p1->outpath_point);
        getline(iss2, p1->nodName, '!');
        getline(iss2, p1->point_offset, '!');
        p1->uin_offset_fun=0;
	p1->uin_offset_point=0;
        p1->IMG_Address=0;
        int i = 0;
        for(i=2;i<(int)p1->fun_offset.length();i++)
        {
		 if(p1->fun_offset[i]>='0'&&p1->fun_offset[i]<='9')//数字字符的转换  
            {  
               p1->uin_offset_fun+=(p1->fun_offset[i]-48)*pow(16.0,(int)p1->fun_offset.length()-i-1);  
            }  
            else if(p1->fun_offset[i]>='A'&&p1->fun_offset[i]<='F')//字母字符的转换  
            {  
                p1->uin_offset_fun+=(p1->fun_offset[i]-55)*pow(16.0,(int)p1->fun_offset.length()-i-1);  
            }  
            else
            {
                p1->uin_offset_fun+=(p1->fun_offset[i]-87)*pow(16.0,(int)p1->fun_offset.length()-i-1); 
            }       
        }

	  for(i=2;i<(int)p1->point_offset.length();i++)
        {
		 if(p1->point_offset[i]>='0'&&p1->point_offset[i]<='9')//数字字符的转换  
            {  
               p1->uin_offset_point+=(p1->point_offset[i]-48)*pow(16.0,(int)p1->point_offset.length()-i-1);  
            }  
            else if(p1->fun_offset[i]>='A'&&p1->fun_offset[i]<='F')//字母字符的转换  
            {  
                p1->uin_offset_point+=(p1->point_offset[i]-55)*pow(16.0,(int)p1->point_offset.length()-i-1);  
            }  
            else
            {
                p1->uin_offset_point+=(p1->point_offset[i]-87)*pow(16.0,(int)p1->point_offset.length()-i-1); 
            }       
        }

       cout<<"nodName:"<<p1->nodName<<endl;
       cout<<"fout:"<<p1->outpath_fun<<endl;
       printf( "foffset: %lx \n", (long unsigned int)p1->uin_offset_fun);
       printf( "poffset: %lx \n", (long unsigned int)p1->uin_offset_point);
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
		  if((curslot->IMG_Address+curslot->uin_offset_point)==(INT64)ip)
		 {
			//fprintf(classifier_f, "%s\n",curslot->outpath);
                        fout_p<<curslot->outpath_point<<endl;
                        cout<<"hit the "<<curslot->outpath_point<<endl; 
		  }
		 if((curslot->IMG_Address+curslot->uin_offset_fun)==(INT64)ip)
		 {
			//fprintf(classifier_f, "%s\n",curslot->outpath);
                        fout_f<<curslot->outpath_fun<<endl;
                        cout<<"hit the "<<curslot->outpath_fun<<endl; 
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
				printf("catch the image %s %lx\n",image.c_str(),(long unsigned int)curslot->IMG_Address);
			}
                   curslot=curslot->nest;
                 }               
		
    
}
VOID Fini(INT32 code, VOID *v)
{
    freelist();
    //fclose(classifier_f);
    fout_f.close();
    fout_p.close();
    fout_slot.close();
   
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
	fout_slot<<(pointNumer-1)<<endl;
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
