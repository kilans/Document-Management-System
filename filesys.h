#pragma once
#define SINGLESIZE 1024
#define MemoryNUM 1024
#define ININODENUM	16
#define INODE_BUFFER_SIZE 16
#define INODESIZE	32
#define SBBLOCKNUM 13
#define INODEBLOCKNUM 64
#define USERNUM 4
#define MY_LNK 0x66
#define MY_REG 0x55
#define MY_DIR 0x44
//#define R	4
//#define W	2
//#define X	1




#define COLOR_DEFAULT       "\033[0m"
#define COLOR_Blue          "\033[0;34m"
#define COLOR_Green         "\033[0;32m"
#define COLOR_Red           "\033[0;31m"
#define COLOR_Yellow        "\033[0;33m"
#define COLOR_White         "\033[0;37m"
#define COLOR_Cyan          "\033[0;36m"
#define COLOR_Magenta       "\033[0;35m"

#define COLOR_BrightBlue    "\033[1;34m"
#define COLOR_BrightGreen   "\033[1;32m"
#define COLOR_BrightRed     "\033[1;31m"
#define COLOR_BrightYellow  "\033[1;33m"
#define COLOR_BrightWhite   "\033[1;37m"
#define COLOR_BrightCyan    "\033[1;36m"
#define COLOR_BrightMagenta "\033[1;35m"



#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <set>
#include <algorithm>
#include <Windows.h>
#include <conio.h>
#include <string>
using namespace std;

class Inode;
class File;
class Command;
class FAT;
class BaseDircoty;
class SuperBlock;
typedef unordered_map<unsigned short, File*> OpenList;

void format(fstream* , SuperBlock**);
void boot(fstream*, SuperBlock**, vector<File*>&,vector<OpenList*>&,OpenList**);
void startup(fstream** , SuperBlock**, vector<File*>& , vector<OpenList*>& ,OpenList**);
bool Scan(string&, fstream*, SuperBlock**, unordered_map<string, int>&, vector<File*>&, OpenList*, vector<OpenList*>&,int);
int Scan(string&, fstream* , SuperBlock** , unordered_map<string, int>&,vector<File*>& , OpenList* , vector<OpenList*>&);
bool Login(vector<string>, vector<File*>& , fstream* , SuperBlock* , OpenList*, vector<OpenList*>&);
bool Halt(fstream*, SuperBlock*, OpenList*);
int Create(vector<string>, vector<File*>&, SuperBlock*, fstream*, int);
bool Delete(vector<string>, vector<File*>&,  SuperBlock*, fstream*, int, OpenList*, OpenList*);
int Open(fstream*, vector<string>, OpenList*, OpenList*,SuperBlock*, vector<File*>&);
pair<File*,int> listFind(int, OpenList*, OpenList*);
bool Close(vector<string>,fstream*, SuperBlock*, vector<File*>&, OpenList*, OpenList*, int);
bool Close(fstream*, SuperBlock*, File*, OpenList*);
string Pwd(vector<File*>&);
bool Switch(vector<string>, vector<File*>&, fstream*, SuperBlock*, OpenList*, vector<OpenList*>&);
int getInode(Inode**,SuperBlock*,fstream*);
int putInode(SuperBlock*, Inode*);
Inode* readInode(int, fstream* Memory,SuperBlock*);
int writeInode(Inode*,  fstream*, SuperBlock*);
unsigned short getMem(SuperBlock*);
void writeFile(File*,fstream*,SuperBlock*);
void readFile(File** file, fstream*, SuperBlock*);
void writeBlock(SuperBlock* , fstream*);
void readBlock(SuperBlock* , fstream*);
void visitList(OpenList*, OpenList*);
bool cd(fstream*, vector<string>&, SuperBlock*, vector<File*>&,OpenList*, OpenList*);
int routeAnlyse(string,vector<File*>&);
int routeFind(fstream*, vector<string> , SuperBlock* , vector<File*>&, vector<File*>*,File**);
//routeFind func :4 2 1读写改,cd是执行，delete和create是对文件夹的写，ls是读，open是执行
bool read(fstream*, vector<string>, vector<File*>&,OpenList*, OpenList*, SuperBlock*);
bool LS(fstream*, vector<string>, vector<File*>&, OpenList*, OpenList*, SuperBlock*);
bool write(fstream* Memory,vector<string>, vector<File*>&, OpenList*, OpenList*, SuperBlock* ,short);
int findFile(string, File*);
vector<string >routesAnlyse(string&);
void filterate(string , vector<string>& );
bool readmod(fstream*, vector<string>&, SuperBlock*, vector<File*>&);
bool chmod(vector<string>, fstream*, SuperBlock*, vector<File*>&);
bool permission( vector<File*>, Inode*, char);
bool logout(fstream*, SuperBlock*, vector<string>, vector<File*>&, OpenList*, vector<OpenList*>&);
bool link(vector<string>, fstream*, SuperBlock*, vector<File*>&, int,int);
Inode* findInode(int, fstream*, SuperBlock*);
void oplink(vector<string>&, vector<string>);

int putMem(int, SuperBlock*);
//void format(vector<FAT*>FATList, fstream* Memory, BaseDircoty BD, unordered_set<Dir*> US);
//void startup(vector<FAT*>FATList, fstream* Memory, BaseDircoty BD, unordered_set<Dir*> US);
//void forSet(vector<FAT*>FATList, fstream* Memory, BaseDircoty BD, unordered_set<Dir*> US);


class Command		//指令集
{
public:
	Command();
	unordered_map<string,int> command;
	unordered_map<string, int> route;
};


class File			//文件
{
public:
	File() {
		text.resize(1);
		dirty = false;
	};
	File(string Infilename, Inode* Ininode)
	{
		File();
		this->filename = Infilename;
		this->fileinode = Ininode;
	}
	string filename;		//文件名
	string password;		//密码，用户也属于文件夹
	vector<string> absolutePath;	//文件夹才需要
	unordered_map<string, int> fileDircoty;			//文件目录
	vector<string> text;			//暂时只有文本内容	
	//unordered_map<string, int> dirDircoty;			//文件夹目录
	Inode* fileinode;		//i结点
	unsigned long long off;	//文件光标所在位置
	char mode;
	bool dirty;
};

//class Dir : public File		//文件夹
//{
//public:
//	Dir() {};
//	Dir(string Infilename, unsigned int Ininode) : File(Infilename, Ininode)
//	{
//	}
//	
//public:
//};


class Inode			//inode
{
public:
	Inode()
	{
		quoteCount = 0;
	}
	Inode(int InID,int Infree)
	{
		i_ID = InID;
		i_addr = Infree;
		addr_num = 1;		//占用内存块
		quoteCount = 0;
		permissions[0] = 7;
		permissions[1] = 7;
		permissions[2] = 7;
	}
	void calsize(File *file)
	{
		i_size0 += file->filename.length() + 1;
		if (i_mode == MY_REG || i_mode == MY_LNK)
		{
			i_size0 += 4;
			base = i_size0;
			for (string& s : file->text)
				i_size0 += s.length()+1;
		}
		else if (i_mode == MY_DIR)
		{
			i_size0 += 4;
			for (auto it : file->absolutePath)
			{
				i_size0 += it.length() + 1;
			}
			i_size0 += 4;
			base = i_size0;
			for (auto it : file->fileDircoty)
			{
				i_size0 += it.first.length() + 1;
				i_size0 += 4;
			}

			i_size0 += file->password.length() + 1;
		}
	}
	int i_mode;				//文件的模式设置
	unsigned short addr_num;	//占用的块个数
	unsigned short i_ID;				//本i结点id
	char quoteCount;			//文件被引用计数
	char permissions[3];		//文件读写权限				
	char i_nlink;				//文件被其它文件链接的个数
	char i_uid;					//文件所有者的ID
	char i_gid;					//文件所有者的组ID
	unsigned long long i_size0;			//表示文件大小
	unsigned short base;				//前局文本
	unsigned short i_addr;				//最大记录8个存储的块号	
	//
	//
};
//4+4+4+1+1+1+1+32+8+8 = 64
class SuperBlock		//基本文件目录,h_node
{
public:
	SuperBlock()
	{
		Linode = new set<unsigned short>;
		Lbnode = new set<unsigned short>;
	}
	SuperBlock(int i)
	{
		Linode = new set<unsigned short>;
		Lbnode = new set<unsigned short>;
		for (unsigned short i = 0; i < MemoryNUM; i++)
		{
			Linode->insert(i);
			Lbnode->insert(i);
		}
		for (int i = 0; i < MemoryNUM; i++)
			FATList[i] = -1;
	}
	int freei_num()
	{
		return Linode->size();
	}
	int freeb_num()
	{
		return Lbnode->size();
	}
	unordered_map<unsigned short, Inode*>	ininode;	//内存i结点映射表		
	set<unsigned short >* Linode;						//磁盘i结点指示			最大2*1024
	set<unsigned short >* Lbnode;						//空闲块i结点指			最大2*1024
	int FATList[MemoryNUM];					//FAT表					稳定4*1024
};
//4+4+1024+1024+1024 = 3080



