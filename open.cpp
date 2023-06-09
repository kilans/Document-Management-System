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
	if ( inodeID == -2 || inodeID == -1)				//Ŀ¼��û�ҵ�
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
	if (permission(DirS, inode, 1) == false)		//�򿪷�ʽ
		return false;
	pair<File*,int> filep = move(listFind(inodeID, SystemList,UserList));
	File* file = filep.first;
	if (filep.second == 2)			//�û����Ѵ�
	{
		cout << "file is opened" << endl;
		return file->fileinode->i_ID;
	}
	if (filep.second == 1)			//�û���δ�򿪣�ϵͳ����
	{
		inode->quoteCount++;
		UserList->insert({ inodeID,file });
		return true;
	}
	
	//��δ��
	//File* file = NULL;
	//�򿪵��ļ���Ҫ���ڴ���ӳ��
	file = new File(fname,inode);
	file->mode = mode;
	inode->quoteCount = 1;
	//cout << int(inode->quoteCount) << endl;
	SystemList->insert({ inodeID,file });		//�ɹ�������ϵ,��ȡ�����ļ����
	UserList->insert({ inodeID,file });
	return inodeID;
}
