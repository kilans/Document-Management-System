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
bool link(vector<string> instruct,fstream *Memory,SuperBlock *SB,vector<File*>&DirS, int mode,int func)//1��Ӳ���ӣ�2��������
{
	if (instruct.size() != 3)
		return false;
	string tarroute = instruct[1];
	string linkroute = instruct[2];
	vector<string>routes = move(routesAnlyse(tarroute));
	string tarname = routes.back();
	File* workplace1 = NULL;
	//////////////////////////////
	int tariID = routeFind(Memory, routes, SB, DirS,NULL, &workplace1);	//ע��Ȩ��
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
	if (func == 2)		//������
	{
		Inode* slink = NULL;
		linkiID = getInode(&slink, SB, Memory);
		if (linkiID == 0 || linkiID == -1)
			return false;
		slink->i_mode = MY_LNK;
		slink->i_uid = DirS[0]->fileinode->i_uid;				//�����ߵ�id
		slink->i_gid = DirS[0]->fileinode->i_gid;				//�����ߵ���id
		File* file = new File(routes.back(), slink);
		file->text = workplace1->absolutePath;
		file->text.push_back(tarname);

		workplace2->fileDircoty.insert({ routes.back(),linkiID });		//���ӵ���ݷ�ʽ
		//file->text = tarfile->absolutePath;
		writeInode(slink,  Memory, SB);		//������i���
		writeFile(file, Memory,SB);				//�������ļ�
	}
	//
	else if (func == 1)							//Ӳ����
	{
		workplace2->fileDircoty.insert({ routes.back(),tariID });		//���ӵ�Ŀ���ļ���i���
		Inode* tari = findInode(tariID, Memory, SB);			//��ȡtari���
		tari->i_nlink++;						//��������
		writeInode(tari, Memory, SB);
	}
	//
	writeFile(workplace2, Memory,SB);				//Ŀ¼д��
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
		return false;			//�ļ��в�����д
	string fname = routes.back();
	if (inodeID != 0)		//��ͬ����
	{
		cout << fname <<" exist" << endl;
		return false;
	}
	Inode* inode = NULL;
	inodeID = getInode(&inode,SB, Memory);						//��ȡi���
	if (inodeID == 0 || inodeID == -1)
	{
		return 0;
	}
	inode->i_uid = DirS[0]->fileinode->i_uid;				//�����ߵ�id
	inode->i_gid = DirS[0]->fileinode->i_gid;				//�����ߵ���id
	inode->i_mode = mode;		//�����ļ�����


	////////////////////////////////////////////
	////////////////////////////////////////////
	writeBlock(SB, Memory);		//������д��
	File* file = new File(fname, inode);
	if (mode == MY_DIR)
	{
		file->absolutePath = workplace->absolutePath;
		file->absolutePath.push_back(fname);
	}
	inode->calsize(file);
	workplace->fileDircoty.insert({ fname,inodeID });		//��ǰĿ¼����
	//for (auto it : workplace->fileDircoty)
	//{
	//	cout << it.first << " ";
	//}
	//cout << endl;
	writeInode(inode, Memory,SB);
	writeFile(file, Memory,SB);					//�´������ļ�д��
	writeFile(workplace, Memory, SB);			//��ǰĿ¼д��(����Ŀ¼)
	
	//delete file;
	return true;
}

