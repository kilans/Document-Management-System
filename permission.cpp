#include"filesys.h"
bool permission(vector<File*>DirS,Inode *fileinode,char func)//4��2��1��д��	
{
	char* permissions = fileinode->permissions;
	if (DirS[0] == NULL)	//�ο��û�
		return (func & permissions[2]);
	Inode* userinode = DirS[0]->fileinode;		//��ȡ�û�inode
	
	if (userinode->i_uid == 0)			//����Ա
			return true;
	else if (userinode->i_uid == fileinode->i_uid)		//�ļ�������
	{
		if (func & permissions[0])
			return true;
	}														
	else if(userinode->i_gid == fileinode->i_gid)      //�ļ�������
	{
		if (func & permissions[1])
			return true;
	}
	else if (func & permissions[2])
		return true;
	cout << "insufficient permissions" << endl;
	return false;
}