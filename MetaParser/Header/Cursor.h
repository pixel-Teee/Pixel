#pragma once

#include "CursorType.h"

class Cursor
{
public:
	using List = std::vector<Cursor>;

	//visitor pattern
	using Visitor = CXCursorVisitor;

	Cursor(const CXCursor& handle);

	CXCursorKind GetKind() const;

	//get lexical parent
	Cursor GetLexicalParent() const;

	//for template specialization, return main
	Cursor GetTemplateSpecialization() const;

	//get this cursor string
	std::string GetSpelling() const;

	//display name, will have extra information for spelling name
	std::string GetDisplayName() const;

	//mangled name
	//for overloaded function name
	std::string GetMangledName() const;

	//unified symbol resolution name, for cross reference
	std::string GetUSR() const;

	std::string GetSourceFile() const;

	bool IsDefinition() const;

	//bool IsOverload() const;

	//method is const?
	bool IsConst() const;

	//method is static?
	bool IsStatic() const;

	//asscess level specifier
	CX_CXXAccessSpecifier GetAccessModifier() const;

	//get the variable or member's class
	CX_StorageClass GetStorageClass() const;

	CursorType GetType() const;

	//get the method or funtion's return type
	CursorType GetReturnType() const;

	//get the actual type
	CursorType GetTypedefType() const;

	List GetChildren() const;

	//custom visitor
	void VisitChildren(Visitor visitor, void* data = nullptr);

	//get the cursor's hash
	uint32_t GetHash() const;
private:
	CXCursor m_Handle;
};