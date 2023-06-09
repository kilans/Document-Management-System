#include "filesys.h"
Command::Command()
{
	command.insert({"halt",0 });	//完成
	command.insert({ "pwd",1 });	//完成
	command.insert({ "logout",2 });	//完成
	command.insert({ "rw",3 });		//完成
	command.insert({ "reset",4 });	//完成
	command.insert({"login",10});	//完成
	command.insert({"open",11});	//完成
	command.insert({"close",12});	//完成
	command.insert({"rm",13});		//完成
	command.insert({"rmdir",14});	//完成
	command.insert({"mk",15});		//完成
	command.insert({"mkdir",16});	//完成
	command.insert({"ls",17});		//完成
	command.insert({"switch",18});	//完成
	command.insert({ "cd",19 });	//完成
	command.insert({ "read",20 });	//完成
	command.insert({ "write",21 });	//完成
	command.insert({ "write+",22 });//完成
	command.insert({ "chmod",23 });	//完成
	command.insert({ "mod",24 });	//完成
	command.insert({ "hlink",25});	//完成
	command.insert({ "slink",26 });	//完成

	route.insert({"~",1});
	route.insert({"..",3});
	route.insert({".",2});
}
