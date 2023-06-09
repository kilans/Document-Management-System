#include "filesys.h"
bool Halt(fstream *Memory,SuperBlock *SB,OpenList *List)
{
	for (auto it : *List)
	{
		Inode* inode = it.second->fileinode;
		writeInode(inode, Memory, SB);
	}
	Memory->close();
	delete Memory;
	exit(0);
}