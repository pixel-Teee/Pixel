#include "PreCompiled.h"

#include "Header/MetaUtils.h"
#include "Header/Namespace.h"

namespace Utils {
	void ToString(const CXString& str, std::string& output)
	{
		//give the const char
		auto cstr = clang_getCString(str);

		output = cstr;

		//decrease the reference count
		clang_disposeString(str);
	}

	std::string GetQualifiedName(const CursorType& type)
	{
		if (type.GetKind() != CXType_Typedef)
			return type.GetDisplayName();

		auto declaration = type.GetDeclaration();

		auto parent = declaration.GetLexicalParent();

		Namespace parentNamespace;

		//walk up to the root namespace
		while (parent.GetKind() == CXCursor_Namespace)
		{
			parentNamespace.emplace(parentNamespace.begin(), parent.GetDisplayName());

			parent = parent.GetLexicalParent();
		}

		//add the display name as the end of the namespace
		parentNamespace.emplace_back(type.GetDisplayName());

		std::string result;

		if (parentNamespace.size() == 0)
		{
			return "";
		}
		else if (parentNamespace.size() == 1)
		{
			return parentNamespace[0];
		}
		else if(parentNamespace.size() >= 2)
		{
			int32_t count = static_cast<int32_t>(parentNamespace.size()) - 1;

			//std::cout << "number:";
			//std::cout << count << std::endl;
			for (int32_t i = 0; i < count; ++i)
			{
				result += parentNamespace[i];

				result += "::";
			}
			result += parentNamespace.back();

			return result;
		}

		return "";
	}

	void LoadText(const std::string& fileName, std::string& output)
	{
		std::ifstream input(fileName);

		if (!input)
		{
			std::stringstream error;

			error << "unable to open file \""
				<< fileName
				<< "\" for reading." << std::endl;
			error << strerror(errno);

			throw std::runtime_error(error.str());
		}

		//move the file pointer to end
		input.seekg(0, std::ios::end);

		output.reserve(static_cast<std::string::size_type>(input.tellg()));

		input.seekg(0, std::ios::beg);

		output.assign((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

		input.close();

		//std::cout << output << std::endl;
	}

	void WriteText(const std::string& fileName, const std::string& text)
	{
		std::ofstream output(fileName);

		if (!output)
		{
			std::stringstream error;

			error << "unable to open file \""
				<< fileName << "\" for writing."
				<< std::endl;

			error << strerror(errno);

			throw std::runtime_error(error.str());
		}

		output << text;

		output.close();
	}

	void FatalError(const std::string& error)
	{
		std::cerr << "error: " << error << std::endl;

		exit(EXIT_FAILURE);
	}

	std::string GetQualifiedName(const std::string& displayName, const Namespace& currentNamespace)
	{
		std::string name;

		if (currentNamespace.size() == 1)
		{
			name = currentNamespace[0] + "::";
		}
		else if (currentNamespace.size() >= 2)
		{
			for (size_t i = 0; i < currentNamespace.size() - 1; ++i)
				name = name + currentNamespace[i] + "::";
			name += currentNamespace.back();
		}

		name += displayName;

		return name;
	}

	std::string GetQualifiedName(const Cursor& cursor, const Namespace& currentNamespace)
	{
		return GetQualifiedName(cursor.GetSpelling(), currentNamespace);
	}

	kainjow::mustache::data::type TemplateBool(bool value)
	{
		return value ? kainjow::mustache::data::type::bool_true : kainjow::mustache::data::type::bool_false;
	}
}