#include "filesys.h"
void format(fstream* Memory, SuperBlock **SB)			//格式化
{
	Memory = new fstream("test/Memory.dat", ios::out | ios::binary);

	(*SB) = new SuperBlock(1);
	
	//超级块的初始化，占用3080字节，用四个块存储 0、1、2、3；

	//long long int SBSize = 4 + 4 + MemoryNUM * 4 + MemoryNUM * 4 + MemoryNUM * 4;
	//long long int InodeSize = MemoryNUM * 64;
	//long long int usemem = (SBSIZE/1024) + 1 + (INODESIZE) / 1024;		//占用的块数
	//if ((SBSIZE + INODESIZE) % 1024)
	//	usemem++;
	//一个inode占用64字节，一共256个inode，占用16个KB，一共16个块 :  2~17 , 一个块有16个inode
	//0和1号inode暂时不使用

	//cout << "Dirmem = " << SBBLOCKNUM+INODEBLOCKNUM << endl;


	(*SB)->Linode->erase((*SB)->Linode->begin());
	(*SB)->Linode->erase((*SB)->Linode->begin());		//两个i结点不用
	
	for (int i = 0; i < SBBLOCKNUM + INODEBLOCKNUM; i++)
		getMem(*SB);

	
	//一共占用了20个块，2个i结点不用；

	//拟创建4个用户 ： 2~5号inode记录
	
	
	
	
	
	Inode* rootinode;
	getInode(&rootinode, *SB, Memory);
	rootinode->i_mode = MY_DIR;
	rootinode->i_uid = 0;
	rootinode->i_gid = 0;
	File* root = new File("~", rootinode);						//根目录
	root->absolutePath.push_back(root->filename);

	Inode* userinode;
	getInode(&userinode, *SB, Memory);
	userinode->i_mode = MY_DIR;
	userinode->i_uid = 0;
	userinode->i_gid = 0;
	userinode->permissions[2] = 4;					//只允许有读权限
	File* userDir = new File("user", userinode);						//根目录
	userDir->absolutePath = root->absolutePath;
	userDir->absolutePath.push_back(userDir->filename);

	Inode* inode1;
	getInode(&inode1,*SB, Memory);
	inode1->i_mode = MY_DIR;
	inode1->i_uid = 0;
	inode1->i_gid = 0;
	inode1->permissions[2] = 4;
	File* Dir1 = new File("root", inode1);					//inode = 2;
	Dir1->password = "123456";
	Dir1->absolutePath = userDir->absolutePath;
	Dir1->absolutePath.push_back(Dir1->filename);
	//
	Inode* inode2;
	getInode(&inode2,*SB, Memory);
	inode2->i_mode = MY_DIR;
	inode2->i_uid = 1;								//文件所有者的ID
	inode2->i_gid = 1;								//文件所有者的ID
	inode2->permissions[1] = 4;
	inode2->permissions[2] = 4;
	File* Dir2 = new File("zhuang", inode2);						//inode = 3;
	Dir2->password = "123456";
	Dir2->absolutePath = userDir->absolutePath;
	Dir2->absolutePath.push_back(Dir2->filename);

	Inode* inode3;
	getInode(&inode3,*SB, Memory);
	inode3->i_mode = MY_DIR;
	inode3->i_uid = 2;								//文件所有者的ID
	inode3->i_gid = 2;								//文件所属组的ID
	inode3->permissions[1] = 4;
	inode3->permissions[2] = 4;
	File* Dir3 = new File("junyuan", inode3);
	Dir3->password = "123456";
	Dir3->absolutePath = userDir->absolutePath;
	Dir3->absolutePath.push_back(Dir3->filename);

	Inode* inode4;
	getInode(&inode4,*SB, Memory);
	inode4->i_mode = MY_DIR;
	inode4->i_uid = 3;								//文件所有者的ID
	inode4->i_gid = 2;								//文件所属组的ID
	inode4->permissions[1] = 4;
	inode4->permissions[2] = 4;
	File* Dir4 = new File("tingting", inode4);
	Dir4->password = "123456";
	Dir4->absolutePath = userDir->absolutePath;
	Dir4->absolutePath.push_back(Dir4->filename);
	//inode 用了6个
	
	root->fileDircoty.insert({ userDir->filename,userDir->fileinode->i_ID });
	userDir->fileDircoty.insert({ Dir1->filename,Dir1->fileinode->i_ID });
	userDir->fileDircoty.insert({ Dir2->filename,Dir2->fileinode->i_ID });
	userDir->fileDircoty.insert({ Dir3->filename,Dir3->fileinode->i_ID });
	userDir->fileDircoty.insert({ Dir4->filename,Dir4->fileinode->i_ID });



	writeBlock(*SB, Memory);
	writeInode(rootinode,Memory, *SB);
	writeInode(userinode, Memory, *SB);
	writeInode(inode1, Memory,*SB);
	writeInode(inode2, Memory,*SB);
	writeInode(inode3, Memory,*SB);
	writeInode(inode4,Memory,*SB);
	
	writeFile(root, Memory, *SB);
	writeFile(userDir, Memory, *SB);
	writeFile(Dir1, Memory, *SB);
	writeFile(Dir2, Memory, *SB);
	writeFile(Dir3, Memory,*SB);
	writeFile(Dir4, Memory,  *SB);
	


	if (Memory->is_open())
		cout << "format finished" << endl;
	Memory->close();

	exit(0);
}

void boot(fstream* Memory, SuperBlock** SB, vector<File*> &DirS,vector<OpenList*>&Lists,OpenList **SystemList)
{
	(*SB) = new SuperBlock();
	readBlock(*SB, Memory);
	(*SystemList) = new OpenList;
	Lists.resize(4);
	for (int i = 0; i < 4; i++)
		Lists[i] = new OpenList;
	//for (int i = 2; i <= 7; i++)					//读入各个用户
	//{
	//	File* tempu = NULL;
	//	Inode* inode = readInode(i, Memory,*SB);
	//	if (inode->i_uid == -1)
	//	{
	//		(*SB)->ininode.erase(i);
	//		continue;
	//	}
	//	readFile(&tempu, Memory, inode);
	//	US.insert({ tempu->filename,dynamic_cast<Dir*>(tempu) });							//用户列表插入
	//	cout << dynamic_cast<Dir*>(tempu)->filename << endl;
	//	cout << dynamic_cast<Dir*>(tempu)->password << endl;
	//}
	//
	;
	Inode* inode = readInode(2, Memory, *SB);
	File* root = new File("~", inode);
	readFile(&root, Memory,*SB);
	(*SystemList)->insert({ inode->i_ID, root });

	/*File* user = NULL;
	Inode* inode = readInode(2, Memory, *SB);
	readFile(&root, Memory, inode, *SB);

	File* u1 = NULL;
	Inode* inode = readInode(2, Memory, *SB);
	readFile(&root, Memory, inode, *SB);

	File* u2 = NULL;
	Inode* inode = readInode(2, Memory, *SB);
	readFile(&root, Memory, inode, *SB);

	File* u3 = NULL;
	Inode* inode = readInode(2, Memory, *SB);
	readFile(&root, Memory, inode, *SB);

	File* u4 = NULL;
	Inode* inode = readInode(2, Memory, *SB);
	readFile(&root, Memory, inode, *SB);*/

	DirS.resize(2);
	DirS[1] = root;
	cout << "initialization finished" << endl;
	Memory->close();
	return;
}

void startup(fstream** Memory,SuperBlock **SB,vector<File*>&DirS, vector<OpenList*>& Lists,OpenList **SystemList)
{
	*Memory = new fstream("test/Memory.dat", ios::in | ios::binary);
	if ((*Memory)->is_open())
		cout << "System Startup" << endl;
	else
		cout << "Startup Fail" << endl;
	
	boot(*Memory, SB, DirS,Lists,SystemList);
}