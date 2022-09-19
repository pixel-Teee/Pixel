#include "PreCompiled.h"

#include "Header/CursorType.h"

#include "Header/MetaUtils.h"

CursorType::CursorType(const CXType& handle)
	:m_Handle(handle)
{
}

std::string CursorType::GetDisplayName() const
{
	std::string displayName;

	//------get type string------
	Utils::ToString(clang_getTypeSpelling(m_Handle), displayName);
	//------get type string------

	//etc:ClassDecl CXXAccessSpecifier FieldDecl CXXMethod VarDecl CXXMethod
	return displayName;
}

int32_t CursorType::GetArgumentCount() const
{
	return clang_getNumArgTypes(m_Handle);
}

CursorType CursorType::GetArgument(uint32_t index) const
{
	return clang_getArgType(m_Handle, index);
}

CursorType CursorType::GetCanonicalType() const
{
	//get the typedef's original type
	return clang_getCanonicalType(m_Handle);
}

CXTypeKind CursorType::GetKind() const
{
	return m_Handle.kind;
}

bool CursorType::IsConst() const
{
	return clang_isConstQualifiedType(m_Handle) ? true : false;
}

Cursor CursorType::GetDeclaration() const
{
	return clang_getTypeDeclaration(m_Handle);
}

