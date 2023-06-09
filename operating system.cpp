
#include "filesys.h"


vector<File*> DirS;					//工作环境栈
fstream* Memory;					//存储空间文件
SuperBlock *SB;						//FAT表

OpenList *Systemlist;
vector<OpenList*>Lists;

int main()
{
	//format(Memory,&SB);
	startup(&Memory,&SB,DirS,Lists,&Systemlist);
	Command* command = new Command;
	string instruct;
	
	while (1)
	{
		while (1)
		{
			Memory->open("test/Memory.dat", ios::in | ios::out | ios::binary);
			getline(cin, instruct);
			if (instruct.empty())
				continue;
			if (Scan(instruct, Memory, &SB, command->command, DirS, Systemlist, Lists, 0) == false)
			{
				cout << "unknow instruct" << endl;
			}
			else
			{
				cout << "login " << DirS[0]->filename << endl;
				Memory->close();
				break;
			}
			Memory->close();
		}	
		
		while (1)
		{
			Memory->open("test/Memory.dat", ios::in | ios::out | ios::binary);
			cout << "[" << DirS[0]->filename << "@OS " << DirS.back()->filename << "]$ ";
			getline(cin, instruct);
			if (instruct.empty())
				continue;
			int cmd = Scan(instruct, Memory, &SB, command->command, DirS, Systemlist,Lists);
			if (cmd == false)
			{
				cout << "unknow instruct" << endl;
			}
			else if (cmd == -1)
			{
				Memory->close();
				break;
			}
			Memory->close();
		}
	}
}
