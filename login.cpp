#include "filesys.h"
bool Login(vector<string>ins, vector<File*>&DirS,fstream* Memory,SuperBlock *SB,OpenList *SystemList,vector<OpenList*>&Lists)
{

	if (ins.size() < 2)
		return false;
	string &Dirname = ins[1];
	string password;
	if (ins.size() == 3)
		password = ins[2];
	/*Dirname = "~/user/" + Dirname;
	vector<string> route = move(routesAnlyse(Dirname));
	File* workplace = NULL;
	int inodeID = routeFind(Memory, route, SB, DirS,NULL, &workplace);*/

	Inode* userinode = readInode(3, Memory, SB);
	File* Userdir = new File("user", userinode);
	readFile(&Userdir, Memory, SB);
	SystemList->insert({ 2,Userdir });
	auto it = Userdir->fileDircoty.find(Dirname);
	if (it == Userdir->fileDircoty.end())
		return false;

	int inodeID = it->second;

	auto iT = SystemList->find(inodeID);
	if (iT != SystemList->end())		//已经被登录过
	{
		DirS[0] = iT->second;
		return true;
	}
	Inode *inode = readInode(inodeID, Memory, SB);
	File* file = new File(Dirname, inode);
	readFile(&file, Memory,SB);
	File* user = file;
	if (password != user->password)
		return false;
	DirS[0] = user;
	OpenList* UserList = Lists[inode->i_uid];
	File* root = SystemList->at(2);
	if (UserList->empty())
	{
		UserList->insert({2, root });//根目录打开
	}
	SystemList->insert({inodeID, user});		//系统打开文件表加入了用户
	SystemList->insert({ userinode->i_ID, Userdir });
	return true;
}
bool logout(fstream*Memory,SuperBlock *SB,vector<string> instuct, vector<File*>& DirS, OpenList* SystemList, vector<OpenList*>&Lists)
{
	File* user = DirS[0];
	OpenList* UserList = Lists[user->fileinode->i_uid];
	for (auto it = UserList->begin(); it != UserList->end(); it++)
	{
		Close(Memory, SB, it->second, SystemList);
	}
	SystemList->erase(user->fileinode->i_ID);	
	//已经值为空了
	return true;
}
bool Switch(vector<string>ins, vector<File*>& DirS, fstream* Memory, SuperBlock* SB, OpenList* SystemList,vector<OpenList*>&Lists)
{
	Login(ins, DirS, Memory, SB, SystemList, Lists);
	OpenList* UserList = Lists[DirS[0]->fileinode->i_uid];					//获取文件打开表
	UserList->insert({ DirS.back()->fileinode->i_ID,DirS.back() });			//工作环境添加
	return true;
}
