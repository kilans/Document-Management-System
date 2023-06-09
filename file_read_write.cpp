#include"filesys.h"
unsigned short apply(fstream* Memory, unsigned short lastmem,SuperBlock* SB)
{
	if (SB->FATList[lastmem] != -1)
		return SB->FATList[lastmem];
	int mem = getMem(SB);
	if (mem == 0)
		return false;
	SB->FATList[lastmem] = mem;
	return mem;
}
//int returnMem(SuperBlock* SB, unsigned short curmem)
//{
//	int nextmem = SB->FATList[curmem];
//	int count = 0;
//	while (nextmem != -1)				//当下一个空块有被申请
//	{
//		int temp = SB->FATList[nextmem];
//		SB->FATList[curmem] = -1;
//		curmem = nextmem;
//		nextmem = temp;
//		
//		count++;
//	};
//	cout << count << endl;
//	return count;
//}
void writeFile(File* file, fstream* Memory, SuperBlock*SB)
{
	Inode* inode = file->fileinode;
	//cout << "write file pos = " << inode->i_addr<< endl;
	long long int pos = inode->i_addr * SINGLESIZE;
	Memory->seekp(pos, ios::beg);
	*Memory << file->filename << endl;
	if (inode->i_mode == MY_REG || inode->i_mode == MY_LNK)
	{
		string writes;
		for (string& s : file->text)
			writes += s + '\n';					//终止符
		unsigned tsi = writes.length();
		Memory->write((const char*)&tsi, sizeof tsi);
		if (!tsi)
			return;
		unsigned short length = 0;
		unsigned short mem = apply(Memory, inode->i_addr, SB);
		while (length+SINGLESIZE < tsi)
		{
			pos = mem * SINGLESIZE;
			Memory->seekp(pos, ios::beg);
			*Memory << writes.substr(length, SINGLESIZE);
			length += SINGLESIZE;
			mem = apply(Memory, mem, SB);
		}
		pos = mem * SINGLESIZE;
		Memory->seekp(pos, ios::beg);
		*Memory << writes.substr(length, tsi - length);
		int nextmem = SB->FATList[mem];
		putMem(nextmem, SB);
	}
	else if (inode->i_mode == MY_DIR)
	{
		int rsi = file->absolutePath.size();
		Memory->write((const char*)(&rsi), sizeof rsi);
		for (auto it : file->absolutePath)
			*Memory << it << endl;
		int fsi = file->fileDircoty.size();
		//if(fsi != 0)
		//	cout << "write file size = " << fsi << endl;
		Memory->write((const char*)(&fsi), sizeof fsi);
		for (auto it : file->fileDircoty)
		{
			*Memory << it.first << endl;
			Memory->write((const char*)(&it.second), sizeof it.second);	//写ID
			//cout << it.first << " ";
		}
		//cout << endl;
		*Memory << file->password << endl;
		//if (!file->password.empty())
		//	cout << "write password " << file->password << endl;
	}
	
}
void readFile(File** file, fstream* Memory,SuperBlock* SB)
{
	Inode* inode = (*file)->fileinode;
	//cout << "read pos = " << inode->i_addr << endl;
	long long int pos = inode->i_addr * SINGLESIZE;
	Memory->seekg(pos, ios::beg);
	getline(*Memory, (*file)->filename);
	//cout << "read filename = " << (*file)->filename << endl;
	if (inode->i_mode == MY_REG || inode->i_mode == MY_LNK)
	{
		//unsigned tsi;
		//Memory->read((char*)(&tsi), sizeof tsi);
		//(*file)->text.resize(tsi);
		//for (unsigned i = 0; i < tsi; i++)
		//	getline(*Memory, (*file)->text[i]);			//读取内容
		unsigned tsi;
		Memory->read((char*)(&tsi), sizeof tsi);
		if (!tsi)
			return;
		unsigned short mem = SB->FATList[inode->i_addr];
		string reads;
		char S[1024];
		while (tsi > SINGLESIZE)
		{
			pos = mem * SINGLESIZE;
			Memory->seekg(pos, ios::beg);
			Memory->read(S, SINGLESIZE);
			reads += S;
			tsi -= SINGLESIZE;
			mem = SB->FATList[mem];
		}
		pos = mem * SINGLESIZE;
		Memory->seekg(pos, ios::beg);
		Memory->read(S, tsi);
		//S[tsi] = '\0';
		reads += S;
		if ((*file)->text.empty())
			(*file)->text.push_back("");
		for (auto& c : reads)
		{
			if (c == '\n')
				(*file)->text.push_back("");
			else
				(*file)->text.back() += c;
		}
		(*file)->text.pop_back();
	}
	else if (inode->i_mode == MY_DIR)
	{
		int rsi;
		Memory->read((char*)(&rsi), sizeof rsi);
		(*file)->absolutePath.resize(rsi);
 		for (int i = 0; i < rsi; i++)
			getline( *Memory,(*file)->absolutePath[i]);
		int fsi;
		Memory->read((char*)(&fsi), sizeof fsi);
		//if(fsi != 0)
		//	cout << "read dir size = " << fsi << endl;
		for (int i = 0; i < fsi; i++)
		{
			pair<string, int> p;
			getline(*Memory, p.first);
			Memory->read((char*)(&p.second), sizeof p.second);		//读ID
			(*file)->fileDircoty.insert(p);
			//cout << p.first << " ";
		}
		//cout << endl;
		getline(*Memory, (*file)->password);
	}
	(*file)->dirty = true;
}