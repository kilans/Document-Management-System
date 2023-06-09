#include"filesys.h"
void filevisit(File* file, fstream* Memory ,SuperBlock *SB,int mode)
{
	
	if (mode == MY_REG)
	{
		cout << "Filename : " << file->filename << endl;
		cout << "Text :" << endl;
		for (string& s : file->text)
			cout << s << endl;
	}
	else if (mode == MY_DIR)
	{
		//cout << "dir size = "<<file->fileDircoty.size() << endl;
		for (auto it : file->fileDircoty)
		{
			int inodeID = it.second;
			Inode *inode = findInode(inodeID, Memory, SB);
			if (inode->i_mode == MY_REG)
				printf((COLOR_BrightYellow "%s "  COLOR_DEFAULT), it.first.c_str());
			else if (inode->i_mode == MY_DIR)
				printf((COLOR_BrightGreen "%s "  COLOR_DEFAULT), it.first.c_str());
			else if (inode->i_mode == MY_LNK)
				printf((COLOR_BrightBlue "%s "  COLOR_DEFAULT), it.first.c_str());
			//cout << it.first << endl;
		}
	}
	cout << endl;
}
char writeAnlse(vector<string> *text,char c, HANDLE &handle,short x,short y)
{
	switch (c)
	{
	case 27:
	{
		system("cls");
		return 0;
	}break;
	case 13:			//换行符
	{
		text->push_back("");
		cout << endl;
	}break;
	case 8:				//Backspace
	{
		//cout << "                                                x = " << x << "y = " << y << endl;
		if (x == 0)
		{
			if (text->size()!=1)
			{
				text->pop_back();
				x = text->back().size();
				y--;
			}
		}
		else
		{
			x--;
			text->back().pop_back();
		}
		SetConsoleCursorPosition(handle, {x,y});
		cout << " ";
		SetConsoleCursorPosition(handle, { x,y });
	}break;
	case 9:				//Tab
	{
		text->back()+=("   ");
		cout << "   ";
	}break;
	default:
	{
		cout << c;
		text->back().push_back(c);
	}break;
	}
	return 1;
}
bool read(fstream* Memory, vector<string> instruct,vector<File*>&DirS, OpenList* SystemList,OpenList* UserList, SuperBlock* SB)
{
	if (instruct.size() > 2)
		return false;
	string route = instruct[1];
	vector<string> routes = move(routesAnlyse(route));
	string fname = routes.back();
	int inodeID = routeFind(Memory, routes, SB, DirS, NULL, NULL);
	if (inodeID == 0)
	{
		cout << fname << " not exist" << endl;
		return false;
	}
	if (inodeID == -2)
		return false;
	Inode* inode = findInode(inodeID, Memory, SB);
	if (inode->i_mode == MY_LNK)
	{
		File* file = new File(fname, inode);
		readFile(&file, Memory, SB);
		oplink(instruct, file->text);
		delete file;
		return read(Memory, instruct, DirS, SystemList, UserList, SB);
	}
	pair<File*, int>filep = listFind(inodeID, SystemList, UserList);
	File* file = filep.first;
	if (filep.second != 2)
	{
		cout << "file not opened" << endl;
		return false;
	}
	if (!(file->mode & 4))
	{
		cout << "insufficient permissions" << endl;
		return false;
	}
	if(!file->dirty)
		readFile(&file, Memory,SB);
	filevisit(file, Memory, SB,MY_REG);
}

bool write(fstream* Memory,vector<string> instruct, vector<File*>&DirS, OpenList* SystemList,OpenList* UserList, SuperBlock* SB, short mode)
{
	if(instruct.size() != 2)
		return false;
	string route = instruct[1];
	vector<string> routes = move(routesAnlyse(route));
	int inodeID = routeFind(Memory, routes, SB, DirS, NULL, NULL);
	if (inodeID == -2 || inodeID == -1)
		return false;
	string fname = routes.back();
	if (inodeID == 0)
	{
		cout << fname << " not exist" << endl;
		return false;
	}
	Inode* inode = findInode(inodeID, Memory, SB);
	if (inode->i_mode == MY_LNK)
	{
		File* file = new File(fname, inode);
		readFile(&file, Memory, SB);
		oplink(instruct, file->text);
		delete file;
		return write(Memory, instruct, DirS, SystemList, UserList, SB, mode);
	}
	pair<File*, int>filep = listFind(inodeID, SystemList, UserList);
	File* file = filep.first;
	if (filep.second != 2)			//用户级未打开
	{
		cout << "file not opened" << endl;
		return false;
	}
	inode = file->fileinode;
	if (!(file->mode & 2))
	{
		cout << "insufficient permissions" << endl;
		return false;
	}
	//找到了且权限足够
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	vector<string>* Text = &file->text;
	
	system("cls");
	if (mode == 1)		//追加写，将前文本打印
	{
		if (!file->dirty)
		{
			readFile(&file, Memory, SB);
			system("cls");
		}
		int x = 0;
		while (1)
		{
			cout << (*Text)[x];
			if (++x == Text->size())
				break;
			cout << endl;
		}
	}
	if (mode == 0)
	{
		Text->clear();
		Text->push_back("");
	}
	
	while (char c = _getch())
	{
		if (writeAnlse(Text, c, handle, Text->back().size(), Text->size()-1) == 0)
			break;
	}
	inode->calsize(file);
	file->dirty = true;			//脏位设置
}

bool LS(fstream* Memory, vector<string> instruct, vector<File*>& DirS, OpenList* SystemList, OpenList* UserList, SuperBlock* SB)
{
	if (instruct.size() > 2)
		return false;
	if (instruct.size() == 1)
		instruct.push_back(".");
	string route = instruct[1];
	vector<string> routes = move(routesAnlyse(route));
	vector<File*>newDirS;
	int inodeID = routeFind(Memory, routes, SB, DirS, &newDirS, NULL);
	if (inodeID == -2 || inodeID == -1)
		return false;
	string fname = routes.back();
	if (inodeID == 0)
	{
		cout << fname << " not exist" << endl;
		return false;
	}
	Inode* inode = findInode(inodeID, Memory, SB);
	File* file = new File(fname, inode);
	if (routeAnlyse(fname, DirS) == 0)
	{
		readFile(&file, Memory, SB);
		file->fileinode = inode;
	}
	else
		file = newDirS.back();
	if (inode->i_mode == MY_LNK)
	{
		oplink(instruct, file->text);
		delete file;
		return LS(Memory, instruct, DirS, SystemList, UserList, SB);
	}
	
	file->fileinode = inode;
	filevisit(file, Memory, SB, MY_DIR);
	
}