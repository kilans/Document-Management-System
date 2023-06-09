#include "filesys.h"
pair<File*,int> listFind(int inodeID, OpenList* SystemList, OpenList *UserList)
{

	auto uit = UserList->find(inodeID);
	if (uit != UserList->end())
		return { uit->second,2 };				//用户打开
	uit = SystemList->find(inodeID);
	if (uit != SystemList->end())
		return { uit->second,1 };				//系统打开
	return { NULL,0 };

}
bool Close(vector<string> instruct,fstream* Memory,SuperBlock *SB,vector<File*>&DirS,OpenList *SystemList, OpenList *UserList, int mode)
{
	if (instruct.size() != 2)
		return false;
	string route = instruct[1];
	vector<string> routes = move(routesAnlyse(route));
	string fname = routes.back();
	int inodeID = routeFind(Memory, routes, SB, DirS,NULL, NULL);
	if (inodeID == 0)
	{
		cout << "can not find" << endl;
		return false;
	}
	if (inodeID == -2 || inodeID == -1)
		return false;
	Inode* inode = findInode(inodeID, Memory, SB);
	if (inode->i_mode == MY_LNK)
	{
		File* file = new File(fname, inode);
		readFile(&file, Memory,  SB);
		oplink(instruct, file->text);
		return Close(instruct, Memory, SB, DirS, SystemList, UserList, mode);
	}
	if (permission(DirS, inode, 1) == false)		//打开方式
		return false;
	pair<File*,int> filep = move(listFind(inodeID, SystemList,UserList));
	if (filep.second == 0 || filep.second == 1)	//用户级未打开
	{
		if (mode == MY_DIR)
			cout << "dir not opened" << endl;
		if (mode == MY_REG)
			cout << "file node opened" << endl;
		return false;
	}
	//用户级打开
	File* file = filep.first;
	inode = file->fileinode;
	inode->quoteCount--;
	//cout << int(inode->quoteCount) << endl;
	UserList->erase(inodeID);
	if (inode->quoteCount == 0)				//引用计数为0，文件关闭并写回
	{
		file->dirty = false;				//脏位恢复
		SystemList->erase(inodeID);
		writeInode(inode, Memory, SB);
		writeFile(file, Memory,SB);	//文件写回
		writeBlock(SB, Memory);
	}
	return true;
}
bool Close(fstream* Memory,SuperBlock*SB,File*file,OpenList *SystemList)		//系统文件直接关闭
{
	Inode* inode = file->fileinode;
	int inodeID = inode->i_ID;

	if (--inode->quoteCount == 0)
	{
		file->dirty = false;
		writeInode(inode, Memory, SB);
		writeFile(file, Memory,SB);
		SystemList->erase(inodeID);
	}
	return true;
}