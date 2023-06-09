#include "filesys.h"
unsigned short getMem(SuperBlock* SB)							//文件创建
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
int putMem(int pos ,SuperBlock *SB)			//文件删除
{
	int count = 0;
	while (pos != -1)
	{
		int temp = SB->FATList[pos];			//记录链接的下一块
		//cout << "put " << pos << endl;			//
		SB->Lbnode->insert(pos);				//这一块进入分配表
		SB->FATList[pos] = -1;					//这一块的链接块为-1
		pos = temp;				//进入下一链接块
		count++;
	}												//根据FAT指示回收
	/*cout << "real put mem = "<< count << endl;
	cout << "free_m = " << SB->freeb_num() << endl;*/
	return count;
}
int getInode(Inode **inode,SuperBlock* SB, fstream* Memory)	//用于文件创建
{
	
	if (SB->freei_num() == 0)
	{
		cout << "inode not enough" << endl;
		return false;
	}
	int index = *(SB->Linode->begin());
	SB->Linode->erase(SB->Linode->begin());
	int free = getMem(SB);	//获取的空闲块
	if (free == 0)
		return -1;
	(*inode) = new Inode(index,free);
	(*inode)->i_nlink = 1;
	SB->ininode.insert({ index,*inode });				//get i要插入到内存i结点中
	//cout << "get inode " << index << endl;
	//cout << "free_i = " << SB->freei_num() << endl;
	return index;
}
int putInode(SuperBlock* SB, Inode* inode)		//用于文件删除
{
	putMem(inode->i_addr, SB);								//归还内存块
	int inodeID = inode->i_ID;
	SB->ininode.erase(inodeID);				//get i要插入到内存i结点中
	SB->Linode->insert(inodeID);

	//cout << "put inode " << inodeID << endl;
	delete inode;
	return inodeID;
}
int writeInode(Inode *inode,fstream* Memory,SuperBlock *SB)	//用于文件保存与关闭
{
	int inodeID = inode->i_ID;
	long long int relative = inodeID * INODESIZE;
	long long int BASE = SBBLOCKNUM * SINGLESIZE;
	long long int pos = BASE + relative;
	Memory->seekp(pos, ios::beg);
	//cout << "write inode pos = " << Memory->tellp() << endl;
	Memory->write((const char*)inode, sizeof *inode);


	//cout << "uid = " << inode->i_uid << " addr_num = " << inode->addr_num << " ";
	SB->ininode.erase(inode->i_ID);							//移除内存inode
	//cout << "write inode " << inodeID << "mode = " << inode->i_mode << endl;
	return inodeID;
}

Inode* readInode(int inodeID , fstream* Memory, SuperBlock* SB)	//用于文件读取
{
	long long int relative = inodeID * INODESIZE;
	long long int BASE = SBBLOCKNUM*SINGLESIZE;
	long long int pos = BASE + relative;
	Inode* inode = new Inode();
	Memory->seekg(pos, ios::beg);
	//cout << "read inode " << inodeID << " pos = " << Memory->tellg() << endl;
	
	Memory->read((char*)inode, sizeof Inode);
	
	//cout << "uid = " << inode->i_uid << " addr_num = " << inode->addr_num << " ";

	SB->ininode.insert({ inodeID,inode });				//插入内存inode
	//cout << "read inode " << inodeID <<" mode = "<< inode->i_mode<< endl;
	
	return inode;
}