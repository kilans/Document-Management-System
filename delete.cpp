#include "filesys.h"
bool rmdir(File*dir ,int inodeID, SuperBlock* SB,fstream *Memory)
{
	for (auto& it : dir->fileDircoty)
	{
		int nextinodeID = it.second;
		Inode* nextinode = findInode(nextinodeID, Memory, SB);
		if (--nextinode->i_nlink == 0)
		{
			if (nextinode->i_mode == MY_DIR)
			{
				File* next = new File(it.first, nextinode);
				readFile(&next, Memory, SB);
				rmdir(next, nextinodeID, SB, Memory);
			}
			putInode(SB, nextinode);
		}
	}
	delete dir;
	return true;
}
bool Delete(vector<string> instruct, vector<File*>& DirS, SuperBlock* SB, fstream* Memory, int mode,OpenList* SystemList, OpenList* UserList)
{
	if (instruct.size() != 2)
		return false;
	string route = instruct[1];
	//被打开的文件不能删除
	vector<string> routes = move(routesAnlyse(route));
	File* workplace = NULL;
	int inodeID = routeFind(Memory, routes, SB, DirS, NULL ,&workplace);	//说明存在该文件
	if (inodeID == -1 || inodeID == -2)
		return false;
	if (!permission(DirS, workplace->fileinode, 2))
		return false;			//文件夹不允许写
	string fname = routes.back();
	Inode* inode = findInode(inodeID, Memory, SB);
	if (inodeID == 0 || (inode->i_mode != mode && inode->i_mode !=MY_LNK ))
	{
		for (int i = 1;i<workplace->absolutePath.size();i++)
			cout << workplace->absolutePath[i] << "/";
		cout << fname<<" not exist" << endl;
		return false;
	}

	if (UserList->find(inodeID) != UserList->end())
	{
		cout << "file openning,can not delete" << endl;
		return false;
	}
	else if (SystemList->find(inodeID) != SystemList->end())
	{
		cout << "other user opening,can not delete" << endl;
		return false;
	}
	
	if (--inode->i_nlink == 0)
	{
		if (inode->i_mode == MY_DIR)
		{
			File* file = new File(fname, inode);
			readFile(&file, Memory,SB);
			rmdir(file, inodeID, SB, Memory);
		}
		putInode(SB, inode);								//inode回归
		writeBlock(SB, Memory);										//超级块写回
	}
	workplace->fileDircoty.erase(fname);				//获取文件夹的内存i结点
	writeFile(workplace, Memory,SB);						//目录写回

	return true;
}