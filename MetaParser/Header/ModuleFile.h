#pragma once

class Class;
class Enum;

struct ModuleFile
{
	std::string name;

	std::vector<std::shared_ptr<Class>> classes;
	std::vector<std::shared_ptr<Enum>> enums;
};