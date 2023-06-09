#include "filesys.h"
void oplink(vector<string>&instruct,vector<string>routes)
{
	string abroute;
	for (string& s : routes)
	{
		abroute += s + '/';
	}
	abroute.pop_back();
	instruct[1] = abroute;
	return;
}
bool link(vector<string> instruct,fstream *Memory,SuperBlock *SB,vector<File*>&DirS, int mode,int func)//1是硬链接，2是软连接
{
	if (instruct.size() != 3)
		return false;
	string tarroute = instruct[1];
	string linkroute = instruct[2];
	vector<string>routes = move(routesAnlyse(tarroute));
	string tarname = routes.back();
	File* workplace1 = NULL;
	//////////////////////////////
	int tariID = routeFind(Memory, routes, SB, DirS,NULL, &workplace1);	//注意权限
	//////////////////////////////
	if (tariID == -1 || tariID == 0 || tariID == -2)
		return false;
	routes = move(routesAnlyse(linkroute));
	string linkname = routes.back();
	File* workplace2 = NULL;
	int linkiID = routeFind(Memory, routes, SB, DirS, NULL, &workplace2);
	if (tariID == -1 || tariID == -2)
		return false;
	if (permission(DirS, workplace2->fileinode, 2) == false)
		return false;
	if (linkiID != 0)
	{
		cout << linkname << " exist" << endl;
		return false;
	}
	//
	if (func == 2)		//软连接
	{
		Inode* slink = NULL;
		linkiID = getInode(&slink, SB, Memory);
		if (linkiID == 0 || linkiID == -1)
			return false;
		slink->i_mode = MY_LNK;
		slink->i_uid = DirS[0]->fileinode->i_uid;				//所有者的id
		slink->i_gid = DirS[0]->fileinode->i_gid;				//所有者的组id
		File* file = new File(routes.back(), slink);
		file->text = workplace1->absolutePath;
		file->text.push_back(tarname);

		workplace2->fileDircoty.insert({ routes.back(),linkiID });		//链接到快捷方式
		//file->text = tarfile->absolutePath;
		writeInode(slink,  Memory, SB);		//软连接i结点
		writeFile(file, Memory,SB);				//软连接文件
	}
	//
	else if (func == 1)							//硬链接
	{
		workplace2->fileDircoty.insert({ routes.back(),tariID });		//链接到目的文件的i结点
		Inode* tari = findInode(tariID, Memory, SB);			//获取tari结点
		tari->i_nlink++;						//被链接数
		writeInode(tari, Memory, SB);
	}
	//
	writeFile(workplace2, Memory,SB);				//目录写回
	return true;
}
int Create(vector<string> instruct, vector<File*>&DirS,SuperBlock *SB,fstream *Memory, int mode)
{
	if (instruct.size() != 2)
		return false;
	string route = instruct[1];
	vector<string> routes = move(routesAnlyse(route));
	File* workplace = NULL;
	int inodeID = routeFind(Memory, routes, SB, DirS, NULL, &workplace);
	if (inodeID == -1 || inodeID == -2)
		return false;
	if (!permission(DirS, workplace->fileinode, 2))
		return false;			//文件夹不允许写
	string fname = routes.back();
	if (inodeID != 0)		//有同名的
	{
		cout << fname <<" exist" << endl;
		return false;
	}
	Inode* inode = NULL;
	inodeID = getInode(&inode,SB, Memory);						//获取i结点
	if (inodeID == 0 || inodeID == -1)
	{
		return 0;
	}
	inode->i_uid = DirS[0]->fileinode->i_uid;				//所有者的id
	inode->i_gid = DirS[0]->fileinode->i_gid;				//所有者的组id
	inode->i_mode = mode;		//设置文件类型


	////////////////////////////////////////////
	////////////////////////////////////////////
	writeBlock(SB, Memory);		//超级块写回
	File* file = new File(fname, inode);
	if (mode == MY_DIR)
	{
		file->absolutePath = workplace->absolutePath;
		file->absolutePath.push_back(fname);
	}
	inode->calsize(file);
	workplace->fileDircoty.insert({ fname,inodeID });		//当前目录插入
	//for (auto it : workplace->fileDircoty)
	//{
	//	cout << it.first << " ";
	//}
	//cout << endl;
	writeInode(inode, Memory,SB);
	writeFile(file, Memory,SB);					//新创建的文件写回
	writeFile(workplace, Memory, SB);			//当前目录写回(更新目录)
	
	//delete file;
	return true;
}

