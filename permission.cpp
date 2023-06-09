#include"filesys.h"
bool permission(vector<File*>DirS,Inode *fileinode,char func)//4、2、1读写改	
{
	char* permissions = fileinode->permissions;
	if (DirS[0] == NULL)	//游客用户
		return (func & permissions[2]);
	Inode* userinode = DirS[0]->fileinode;		//获取用户inode
	
	if (userinode->i_uid == 0)			//管理员
			return true;
	else if (userinode->i_uid == fileinode->i_uid)		//文件所有者
	{
		if (func & permissions[0])
			return true;
	}														
	else if(userinode->i_gid == fileinode->i_gid)      //文件所属组
	{
		if (func & permissions[1])
			return true;
	}
	else if (func & permissions[2])
		return true;
	cout << "insufficient permissions" << endl;
	return false;
}