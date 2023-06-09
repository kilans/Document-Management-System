#include "filesys.h"

int Open(fstream* Memory,vector<string> instruct, OpenList* SystemList, OpenList* UserList,
	SuperBlock*SB,vector<File*>& DirS)
{
	if (instruct.size() != 3 || instruct[2].length()!=1)
		return false;
	string route = instruct[1];
	vector<string> routes = move(routesAnlyse(route));
	string fname = routes.back();
	int inodeID = routeFind(Memory, routes, SB, DirS,NULL, NULL);
	if ( inodeID == -2 || inodeID == -1)				//目录中没找到
		return false;
	if (inodeID == 0)
	{
		cout << fname << " can not find" << endl;
		return false;
	}
	Inode* inode = findInode(inodeID, Memory, SB);
	if (inode->i_mode == MY_DIR)
		return false;
	
	char mode = instruct[2].back() - '0';
	if (inode->i_mode == MY_LNK)
	{
		File* file = new File(fname, inode);
		readFile(&file, Memory, SB);
		oplink(instruct, file->text);
		delete file;
		return Open(Memory, instruct, SystemList, UserList, SB, DirS);
	}
	if (permission(DirS, inode, 1) == false)		//打开方式
		return false;
	pair<File*,int> filep = move(listFind(inodeID, SystemList,UserList));
	File* file = filep.first;
	if (filep.second == 2)			//用户级已打开
	{
		cout << "file is opened" << endl;
		return file->fileinode->i_ID;
	}
	if (filep.second == 1)			//用户级未打开，系统级打开
	{
		inode->quoteCount++;
		UserList->insert({ inodeID,file });
		return true;
	}
	
	//均未打开
	//File* file = NULL;
	//打开的文件需要在内存中映射
	file = new File(fname,inode);
	file->mode = mode;
	inode->quoteCount = 1;
	//cout << int(inode->quoteCount) << endl;
	SystemList->insert({ inodeID,file });		//成功建立联系,获取到了文件结点
	UserList->insert({ inodeID,file });
	return inodeID;
}
