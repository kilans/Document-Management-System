#include "filesys.h"
int routeAnlyse(string route,vector<File*> &target)
{
	if (route == "..")
	{
		if (target.size() > 2)
			target.pop_back();
		return 1;
	}
	else if (route == "~")
	{
		while (target.size() > 2)
			target.pop_back();
		return 1;
	}
	else if (route == ".")
		return 1;
	return 0;
}
vector<string> routesAnlyse(string &route)
{
	vector<string> routes;
	int i = 0;
	while (i < route.length())
	{
		string fname;
		while (i < route.length() && route[i] != '/')
			fname.push_back(route[i++]);
		i++;
		routes.push_back(fname);
		//cout << "fname = " << fname << endl;
	}
	return routes;
}
//open opendir close closedir mk mkdir cd rm rmdir都会用到
int findFile(string filename, File* Current)
{
	auto it = Current->fileDircoty.find(filename);
	if (it == Current->fileDircoty.end())
		return 0;
	return it->second;
}
Inode* findInode(int inodeID, fstream*Memory,SuperBlock* SB)
{
	auto it = SB->ininode.find(inodeID);
	if (it != SB->ininode.end())
		return it->second;
	return readInode(inodeID, Memory, SB);
}
int routeFind(fstream* Memory, vector<string> routes, SuperBlock* SB, vector<File*>& DirS, vector<File*>* newDirS, File** workplace)
{
	vector<File*> target = DirS;		//新的一级目录栈
	for (int j = 0; j < routes.size() - 1; j++)		//最后一个单独处理
	{
		string& fname = routes[j];					//每一集文件名
		if (routeAnlyse(fname, target) == 0)
		{
			int inodeID = findFile(fname, target.back());	//在当前文件夹下找文件
			if (inodeID == 0)					//没找到
			{
				cout << "can not find ";
				for (auto it : target.back()->absolutePath)
					cout << it << "/";
				cout << fname << endl;
				target.clear();					//析构
				return false;
			}
			Inode* inode = findInode(inodeID, Memory, SB);
			if (permission(DirS, inode, 1) == false)			//打开操作
				return -2;
			if (!(inode->i_mode == MY_DIR || inode->i_mode == MY_LNK))
			{
				for (auto it : target.back()->absolutePath)
					cout << it << "/";
				cout << fname << " not dir" << endl;
				return -1;
			}
			File* tempd = new File(fname, inode);
			////
			if (inode->i_mode == MY_LNK)	//软链接文件
			{
				readFile(&tempd, Memory,SB);		//读入链接文件
				inodeID = routeFind(Memory, tempd->text, SB, DirS, NULL, NULL);	//查找目的文件
				if (inodeID == 0)				//目录中没找到
					return false;
				else if (inodeID == -2)
					return false;
				inode = findInode(inodeID, Memory, SB);		//真正的inode
				if (permission(DirS, inode, 1) == false)	//真正的inode需要权限解析
					return -2;
				if (inode->i_mode != MY_DIR && inode->i_mode != MY_LNK)
				{
					for (int i = 1; i < target.back()->absolutePath.size(); i++)
						cout<< target.back()->absolutePath[i] << "/";
					cout << fname << " not dir" << endl;
					return -1;
				}
			}
			////
			tempd->fileinode = inode;
			readFile(&tempd, Memory,SB);				//读入文件夹
			target.push_back(tempd);
		}
	}
	Inode* inode = target.back()->fileinode;
	string& finalname = routes.back();
	int anlyse = routeAnlyse(finalname, target);
	//应用于 打开、创建、删除、cd
	if (workplace)
	{
		*workplace = target.back();		//文件夹写操作
		//cout<<"workplce:"<<(*workplace)->filename << endl;
	}
	else if (newDirS)						//cd ../../..指令，不需要额外处理
	{
		*newDirS = target;
	}
	if (anlyse == 0)		//最后一个是文件名
	{
		int inodeID = findFile(finalname, target.back());
		if (inodeID == 0)						//没有找到
			return 0;
		//cout << "route find inode " << inodeID << endl;
		return inodeID;
	}
	else                    //最后一个是简称，且已处理完毕
	{
		//cout << "route find inode " << target.back()->fileinode->i_ID << endl;
		return target.back()->fileinode->i_ID;
	}
}