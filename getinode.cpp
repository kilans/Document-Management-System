#include "filesys.h"
unsigned short getMem(SuperBlock* SB)							//�ļ�����
{
	if (SB->freeb_num() == 0)
	{
		cout << "Memory not enough" << endl;
		return false;
	}
	unsigned short bindex = *(SB->Lbnode->begin());
	SB->Lbnode->erase(SB->Lbnode->begin());

	//cout << "get m = " << bindex << endl;
	//cout << "free_m = " << SB->freeb_num() << endl;

	return bindex;
}
int putMem(int pos ,SuperBlock *SB)			//�ļ�ɾ��
{
	int count = 0;
	while (pos != -1)
	{
		int temp = SB->FATList[pos];			//��¼���ӵ���һ��
		//cout << "put " << pos << endl;			//
		SB->Lbnode->insert(pos);				//��һ���������
		SB->FATList[pos] = -1;					//��һ������ӿ�Ϊ-1
		pos = temp;				//������һ���ӿ�
		count++;
	}												//����FATָʾ����
	/*cout << "real put mem = "<< count << endl;
	cout << "free_m = " << SB->freeb_num() << endl;*/
	return count;
}
int getInode(Inode **inode,SuperBlock* SB, fstream* Memory)	//�����ļ�����
{
	
	if (SB->freei_num() == 0)
	{
		cout << "inode not enough" << endl;
		return false;
	}
	int index = *(SB->Linode->begin());
	SB->Linode->erase(SB->Linode->begin());
	int free = getMem(SB);	//��ȡ�Ŀ��п�
	if (free == 0)
		return -1;
	(*inode) = new Inode(index,free);
	(*inode)->i_nlink = 1;
	SB->ininode.insert({ index,*inode });				//get iҪ���뵽�ڴ�i�����
	//cout << "get inode " << index << endl;
	//cout << "free_i = " << SB->freei_num() << endl;
	return index;
}
int putInode(SuperBlock* SB, Inode* inode)		//�����ļ�ɾ��
{
	putMem(inode->i_addr, SB);								//�黹�ڴ��
	int inodeID = inode->i_ID;
	SB->ininode.erase(inodeID);				//get iҪ���뵽�ڴ�i�����
	SB->Linode->insert(inodeID);

	//cout << "put inode " << inodeID << endl;
	delete inode;
	return inodeID;
}
int writeInode(Inode *inode,fstream* Memory,SuperBlock *SB)	//�����ļ�������ر�
{
	int inodeID = inode->i_ID;
	long long int relative = inodeID * INODESIZE;
	long long int BASE = SBBLOCKNUM * SINGLESIZE;
	long long int pos = BASE + relative;
	Memory->seekp(pos, ios::beg);
	//cout << "write inode pos = " << Memory->tellp() << endl;
	Memory->write((const char*)inode, sizeof *inode);


	//cout << "uid = " << inode->i_uid << " addr_num = " << inode->addr_num << " ";
	SB->ininode.erase(inode->i_ID);							//�Ƴ��ڴ�inode
	//cout << "write inode " << inodeID << "mode = " << inode->i_mode << endl;
	return inodeID;
}

Inode* readInode(int inodeID , fstream* Memory, SuperBlock* SB)	//�����ļ���ȡ
{
	long long int relative = inodeID * INODESIZE;
	long long int BASE = SBBLOCKNUM*SINGLESIZE;
	long long int pos = BASE + relative;
	Inode* inode = new Inode();
	Memory->seekg(pos, ios::beg);
	//cout << "read inode " << inodeID << " pos = " << Memory->tellg() << endl;
	
	Memory->read((char*)inode, sizeof Inode);
	
	//cout << "uid = " << inode->i_uid << " addr_num = " << inode->addr_num << " ";

	SB->ininode.insert({ inodeID,inode });				//�����ڴ�inode
	//cout << "read inode " << inodeID <<" mode = "<< inode->i_mode<< endl;
	
	return inode;
}