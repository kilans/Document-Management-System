#include "filesys.h"

bool cd(fstream* Memory, vector<string> &instruct,SuperBlock* SB,vector<File*>&DirS, OpenList* SystemList,OpenList*UserList)
{
	if (instruct.size() != 2)
		return false;
	
	string route = instruct[1];

	vector<string> routes = move(routesAnlyse(route));

	vector<File*>* newDirS = new vector<File*>;

	int inodeID = routeFind(Memory, routes, SB, DirS, newDirS,NULL);
	string fname = routes.back();
	if (inodeID == 0)
	{
		for (auto& it : newDirS->back()->absolutePath)
			cout << it << "/"; 
		cout << fname <<" can not find" << endl;
		return false;
	}
	else if (inodeID == -2)
		return false;
	DirS = *newDirS;		//Ŀ¼�仯
	Inode* inode = findInode(inodeID, Memory, SB);
	if (permission( DirS, inode, 1) == false || !(inode->i_mode == MY_LNK || inode->i_mode == MY_DIR))
		return false;
	File* dir = new File(fname, inode);
	readFile(&dir, Memory,SB);
	if (inode->i_mode == MY_LNK)
	{
		oplink(instruct, dir->text);
		delete dir;
		return cd(Memory, instruct, SB, DirS, SystemList, UserList);
	}
	DirS.push_back(dir);
	UserList->insert({ inodeID,dir });
	SystemList->insert({ inodeID,dir });
	return true;
	
}
