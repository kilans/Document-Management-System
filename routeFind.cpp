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
//open opendir close closedir mk mkdir cd rm rmdir�����õ�
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
	vector<File*> target = DirS;		//�µ�һ��Ŀ¼ջ
	for (int j = 0; j < routes.size() - 1; j++)		//���һ����������
	{
		string& fname = routes[j];					//ÿһ���ļ���
		if (routeAnlyse(fname, target) == 0)
		{
			int inodeID = findFile(fname, target.back());	//�ڵ�ǰ�ļ��������ļ�
			if (inodeID == 0)					//û�ҵ�
			{
				cout << "can not find ";
				for (auto it : target.back()->absolutePath)
					cout << it << "/";
				cout << fname << endl;
				target.clear();					//����
				return false;
			}
			Inode* inode = findInode(inodeID, Memory, SB);
			if (permission(DirS, inode, 1) == false)			//�򿪲���
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
			if (inode->i_mode == MY_LNK)	//�������ļ�
			{
				readFile(&tempd, Memory,SB);		//���������ļ�
				inodeID = routeFind(Memory, tempd->text, SB, DirS, NULL, NULL);	//����Ŀ���ļ�
				if (inodeID == 0)				//Ŀ¼��û�ҵ�
					return false;
				else if (inodeID == -2)
					return false;
				inode = findInode(inodeID, Memory, SB);		//������inode
				if (permission(DirS, inode, 1) == false)	//������inode��ҪȨ�޽���
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
			readFile(&tempd, Memory,SB);				//�����ļ���
			target.push_back(tempd);
		}
	}
	Inode* inode = target.back()->fileinode;
	string& finalname = routes.back();
	int anlyse = routeAnlyse(finalname, target);
	//Ӧ���� �򿪡�������ɾ����cd
	if (workplace)
	{
		*workplace = target.back();		//�ļ���д����
		//cout<<"workplce:"<<(*workplace)->filename << endl;
	}
	else if (newDirS)						//cd ../../..ָ�����Ҫ���⴦��
	{
		*newDirS = target;
	}
	if (anlyse == 0)		//���һ�����ļ���
	{
		int inodeID = findFile(finalname, target.back());
		if (inodeID == 0)						//û���ҵ�
			return 0;
		//cout << "route find inode " << inodeID << endl;
		return inodeID;
	}
	else                    //���һ���Ǽ�ƣ����Ѵ������
	{
		//cout << "route find inode " << target.back()->fileinode->i_ID << endl;
		return target.back()->fileinode->i_ID;
	}
}