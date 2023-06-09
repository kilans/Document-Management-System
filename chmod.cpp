#include "filesys.h"
bool modcharge(string& mods)
{
	if (mods.length() != 3)
		return false;
	for (char& c : mods)
		if (!(c <= '7' && c >= '0'))
			return false;
	return true;
}
bool readmod(fstream* Memory, vector<string>& instruct, SuperBlock* SB, vector<File*>& DirS)
{
	if (instruct.size() != 2)
		return false;
	string route = instruct[1];
	vector<string> routes = move(routesAnlyse(route));
	File* workplace = NULL;
	int inodeID = routeFind(Memory, routes, SB, DirS,NULL, &workplace);
	if (inodeID == 0)
	{
		cout << "can not find" << endl;
		return true;
	}
	Inode* inode = readInode(inodeID, Memory, SB);
	if (permission( DirS, inode, 4) == false)
		return false;
	string fname = routes.back();
	if (inode->i_mode == MY_LNK)
	{
		File* file = new File(fname, inode);
		readFile(&file, Memory, SB);
		oplink(instruct, file->text);
		delete file;
		return readmod( Memory, instruct, SB, DirS);
	}
	for (int i = 0; i < 3; i++)
		cout << (int)inode->permissions[i] << " ";
	cout << endl;
}
bool chmod(vector<string>instruct,fstream *Memory,SuperBlock *SB,vector<File*>&DirS)
{
	if (instruct.size() != 3)
		return false;
	string route = instruct[1];
	string mods = instruct[2];
	if (modcharge(mods) == false)
	{
		cout << "not mod format" << endl;
		return 0;
	}
	vector<string> routes = move(routesAnlyse(route));
	int inodeID = routeFind(Memory, routes, SB, DirS, NULL ,NULL);//chmod是写操作
	string fname = routes.back();
	if (inodeID == 0)
	{
		cout << fname <<"can not find" << endl;
		return 0;
	}
	if (inodeID == -2 || inodeID == -1)
		return false;
	Inode* inode = findInode(inodeID, Memory, SB);
	if (permission( DirS, inode, 2) == false)
		return false;
	
	if (inode->i_mode == MY_LNK)
	{
		File* file = new File(fname, inode);
		readFile(&file, Memory, SB);
		oplink(instruct, file->text);
		return chmod(instruct, Memory, SB, DirS);
	}

	for (int i = 0; i < 3; i++)
		inode->permissions[i] = mods[i]-'0';			//权限修改
	writeInode(inode,Memory, SB);
	cout << "chmod succced" << endl;
	return true;
}