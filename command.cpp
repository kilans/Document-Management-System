#include "filesys.h"
Command::Command()
{
	command.insert({"halt",0 });	//���
	command.insert({ "pwd",1 });	//���
	command.insert({ "logout",2 });	//���
	command.insert({ "rw",3 });		//���
	command.insert({ "reset",4 });	//���
	command.insert({"login",10});	//���
	command.insert({"open",11});	//���
	command.insert({"close",12});	//���
	command.insert({"rm",13});		//���
	command.insert({"rmdir",14});	//���
	command.insert({"mk",15});		//���
	command.insert({"mkdir",16});	//���
	command.insert({"ls",17});		//���
	command.insert({"switch",18});	//���
	command.insert({ "cd",19 });	//���
	command.insert({ "read",20 });	//���
	command.insert({ "write",21 });	//���
	command.insert({ "write+",22 });//���
	command.insert({ "chmod",23 });	//���
	command.insert({ "mod",24 });	//���
	command.insert({ "hlink",25});	//���
	command.insert({ "slink",26 });	//���

	route.insert({"~",1});
	route.insert({"..",3});
	route.insert({".",2});
}
