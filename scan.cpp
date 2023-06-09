#include "filesys.h"
void filterate(string S,vector<string> &ins)
{
	int i = 0;
	while (i < S.length())
	{
		string temps;
		while (i<S.length() && S[i] == ' ')
			i++;
		while (i<S.length() && S[i] != ' ')
			temps += S[i++];
		if(!temps.empty())
			ins.push_back(temps);
	}
}
bool Scan(string& instruct, fstream*Memory,SuperBlock** SB,unordered_map<string, int>& command,vector<File*>&DirS,OpenList *SystemList,vector<OpenList*>&Lists,int tag)
{
	vector<string> ins;
	filterate(instruct,ins);
	if (ins.empty() || command.count(ins[0]) == 0)
		return false;
	int comID = command[ins[0]];
	switch (comID)
	{
	case 0:
		exit(0);
	case 10:
		return Login(ins, DirS, Memory, *SB, SystemList,Lists);
	case 4:format(Memory, SB); 
		break;
	default:
		return false;
	}
	return true;
}
int Scan(string &instruct,fstream *Memory,SuperBlock **SB, unordered_map<string,int> &command , vector<File*> &DirS,  OpenList*SystemList,vector<OpenList*>&Lists)
{
	vector<string> ins;
	filterate(instruct,ins);
	if (ins.empty() || command.count(ins[0]) == 0)
		return false;
	int comID = command[ins[0]];
	OpenList* UserList = Lists[DirS[0]->fileinode->i_uid];

	switch (comID)
	{
	//case 1:Login(appS,Dir,US);
		//	break;
	case 0:Halt(Memory, *SB, SystemList);
		break;
	case 1:Pwd(DirS);
		break;
	case 2:logout(Memory, *SB, ins, DirS, SystemList, Lists);
		return -1; 
	case 3:visitList(SystemList, Lists[DirS[0]->fileinode->i_uid]);
		break;
	case 4:format(Memory, SB);
		break;
	case 10:Login(ins, DirS, Memory, *SB, SystemList, Lists);
		break;
	case 11:Open(Memory,ins,SystemList,UserList, *SB,DirS);
		break;
	case 12:Close(ins,Memory,*SB,DirS,SystemList, UserList,MY_REG );
		break;
	case 13:Delete(ins, DirS, *SB, Memory,MY_REG , SystemList, UserList);
		break;
	case 14:Delete(ins, DirS, *SB, Memory,MY_DIR , SystemList, UserList);
		break;
	case 15:Create(ins, DirS, *SB, Memory, MY_REG );
		break;
	case 16:Create(ins, DirS, *SB, Memory, MY_DIR );
		break;
	case 17:LS(Memory, ins, DirS, SystemList, UserList, *SB);
		break;
	case 18:Switch(ins, DirS, Memory, *SB, SystemList,Lists);
		break;
	case 19:cd(Memory, ins, *SB, DirS, SystemList,UserList);
		break;
	case 20:read(Memory, ins, DirS,SystemList,UserList,*SB);
		break;
	case 21:write(Memory,ins,DirS, SystemList,UserList,*SB,0);
		break;
	case 22:write(Memory,ins, DirS,SystemList, UserList,*SB,1);
		break;
	case 23:chmod(ins, Memory, *SB, DirS);
		break;
	case 24:readmod(Memory, ins, *SB, DirS);
		break;
	case 25:link(ins, Memory, *SB, DirS, MY_REG,1);
		break;
	case 26:link(ins, Memory, *SB, DirS, MY_REG, 2);
		break;
	default:
		return false;
	}

	return true;
}