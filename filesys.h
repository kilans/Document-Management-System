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
//routeFind func :4 2 1��д��,cd��ִ�У�delete��create�Ƕ��ļ��е�д��ls�Ƕ���open��ִ��
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


class Command		//ָ�
{
public:
	Command();
	unordered_map<string,int> command;
	unordered_map<string, int> route;
};


class File			//�ļ�
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
	string filename;		//�ļ���
	string password;		//���룬�û�Ҳ�����ļ���
	vector<string> absolutePath;	//�ļ��в���Ҫ
	unordered_map<string, int> fileDircoty;			//�ļ�Ŀ¼
	vector<string> text;			//��ʱֻ���ı�����	
	//unordered_map<string, int> dirDircoty;			//�ļ���Ŀ¼
	Inode* fileinode;		//i���
	unsigned long long off;	//�ļ��������λ��
	char mode;
	bool dirty;
};

//class Dir : public File		//�ļ���
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
		addr_num = 1;		//ռ���ڴ��
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
	int i_mode;				//�ļ���ģʽ����
	unsigned short addr_num;	//ռ�õĿ����
	unsigned short i_ID;				//��i���id
	char quoteCount;			//�ļ������ü���
	char permissions[3];		//�ļ���дȨ��				
	char i_nlink;				//�ļ��������ļ����ӵĸ���
	char i_uid;					//�ļ������ߵ�ID
	char i_gid;					//�ļ������ߵ���ID
	unsigned long long i_size0;			//��ʾ�ļ���С
	unsigned short base;				//ǰ���ı�
	unsigned short i_addr;				//����¼8���洢�Ŀ��	
	//
	//
};
//4+4+4+1+1+1+1+32+8+8 = 64
class SuperBlock		//�����ļ�Ŀ¼,h_node
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
	unordered_map<unsigned short, Inode*>	ininode;	//�ڴ�i���ӳ���		
	set<unsigned short >* Linode;						//����i���ָʾ			���2*1024
	set<unsigned short >* Lbnode;						//���п�i���ָ			���2*1024
	int FATList[MemoryNUM];					//FAT��					�ȶ�4*1024
};
//4+4+1024+1024+1024 = 3080



