#include "filesys.h"
void visitList(OpenList *SystemList, OpenList* UserList)
{
	cout << "SystemList:" << endl;
	for_each(SystemList->begin(), SystemList->end(), [](pair<int, File*> p) {
		if(p.second->fileinode->i_mode == MY_DIR)
			printf((COLOR_BrightGreen "%d  %s"  COLOR_DEFAULT), p.first, p.second->filename.c_str());
		else if(p.second->fileinode->i_mode == MY_REG)
			printf((COLOR_BrightYellow "%d  %s"  COLOR_DEFAULT), p.first, p.second->filename.c_str());
		else if(p.second->fileinode->i_mode == MY_LNK)
			printf((COLOR_BrightBlue "%d  %s"  COLOR_DEFAULT), p.first, p.second->filename.c_str());
		cout << endl;
		});
	cout << endl;
	cout << "UserList:" << endl;
	for_each(UserList->begin(), UserList->end(), [](pair<int, File*> p) {
		if (p.second->fileinode->i_mode == MY_DIR)
			printf((COLOR_BrightGreen "%d  %s"   COLOR_DEFAULT), p.first,p.second->filename.c_str());
		else if (p.second->fileinode->i_mode == MY_REG)
			printf((COLOR_BrightYellow "%d  %s"   COLOR_DEFAULT), p.first, p.second->filename.c_str());
		else if (p.second->fileinode->i_mode == MY_LNK)
			printf((COLOR_BrightBlue "%d  %s"   COLOR_DEFAULT), p.first, p.second->filename.c_str());
		cout << endl;
		});
	cout << endl;
}
string Pwd(vector<File*> &DirS)
{
	File* file = DirS.back();
	string S;
	S += DirS[0]->filename;
	for (int i = 1; i < file->absolutePath.size(); i++)
	{
		S += '/' + file->absolutePath[i];
	}

	cout << S << endl;
	return S;
}

