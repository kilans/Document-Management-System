#include "filesys.h"
void readBlock(SuperBlock* SB, fstream* Memory)
{
	//¿ÕµÄSB
	Memory->seekg(ios::beg);

	unsigned int freeb_num;
	unsigned int freei_num;
	Memory->read((char*)&freeb_num, 4);
	Memory->read((char*)&freei_num, 4);

	for (unsigned int  i = 0; i < freeb_num; i++)
	{
		unsigned int  tempv;
		Memory->read((char*)&tempv,4);
		SB->Lbnode->insert(tempv);
	}

	for (unsigned int  i = 0; i < freei_num; i++)
	{
		unsigned int  tempv;
		Memory->read((char*)&tempv,4);
		SB->Linode->insert(tempv);
	}

	Memory->read((char*)SB->FATList, sizeof SB->FATList);

	//cout << "read block" << endl;
}
void writeBlock(SuperBlock* SB , fstream *Memory)
{
	Memory->seekp(ios::beg);

	unsigned int  freeb_num = SB->freeb_num();
	unsigned int  freei_num = SB->freei_num();
	
	Memory->write((const char*)&freeb_num,4);
	
	Memory->write((const char*)&freei_num,4);
	
	for (unsigned int  v : *(SB->Lbnode))
	{
		Memory->write((const char*)&v,4);
	}
	for (unsigned int  v : *(SB->Linode))
	{
		Memory->write((const char*)&v,4);
	}
	Memory->write((const char*)SB->FATList, sizeof SB->FATList);
	
	//cout << "write block" << endl;
}