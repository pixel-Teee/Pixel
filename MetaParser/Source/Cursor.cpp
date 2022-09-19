#include "PreCompiled.h"

#include "Header/Cursor.h"
#include "Header/MetaUtils.h"

Cursor::Cursor(const CXCursor& handle)
{
	m_Handle = handle;
}

CXCursorKind Cursor::GetKind() const
{
	return m_Handle.kind;
}

Cursor Cursor::GetLexicalParent() const
{
	return clang_getCursorLexicalParent(m_Handle);
}

Cursor Cursor::GetTemplateSpecialization() const
{
	return clang_getSpecializedCursorTemplate(m_Handle);
}

std::string Cursor::GetSpelling() const
{
	std::string spelling;

	Utils::ToString(clang_getCursorSpelling(m_Handle), spelling);

	return spelling;
}

std::string Cursor::GetDisplayName() const
{
	std::string displayName;

	Utils::ToString(clang_getCursorDisplayName(m_Handle), displayName);

	return displayName;
}

std::string Cursor::GetMangledName() const
{
	std::string mangled;

	Utils::ToString(clang_Cursor_getMangling(m_Handle), mangled);

	return mangled;
}

std::string Cursor::GetUSR() const
{
	std::string usr;

	Utils::ToString(clang_getCursorUSR(m_Handle), usr);

	return usr;
}

std::string Cursor::GetSourceFile() const
{
	auto range = clang_Cursor_getSpellingNameRange(m_Handle, 0, 0);

	auto start = clang_getRangeStart(range);

	CXFile file;

	//------get the cursor position------
	uint32_t line, column, offset;

	clang_getFileLocation(start, &file, &line, &column, &offset);

	std::string fileName;

	Utils::ToString(clang_getFileName(file), fileName);

	return fileName;
	//------get the cursor position------
}

bool Cursor::IsDefinition() const
{
	return clang_isCursorDefinition(m_Handle) ? true : false;
}

bool Cursor::IsConst() const
{
	return clang_CXXMethod_isConst(m_Handle) ? true : false;
}

bool Cursor::IsStatic() const
{
	return clang_CXXMethod_isStatic(m_Handle) ? true : false;
}

CX_CXXAccessSpecifier Cursor::GetAccessModifier() const
{
	return clang_getCXXAccessSpecifier(m_Handle);
}

CX_StorageClass Cursor::GetStorageClass() const
{
	return clang_Cursor_getStorageClass(m_Handle);
}

CursorType Cursor::GetType() const
{
	return clang_getCursorType(m_Handle);
}

CursorType Cursor::GetReturnType() const
{
	return clang_getCursorResultType(m_Handle);
}

CursorType Cursor::GetTypedefType() const
{
	return clang_getTypedefDeclUnderlyingType(m_Handle);
}

Cursor::List Cursor::GetChildren() const
{
	List children;

	//define a lambda
	//only get the one level's child
	auto visitor = [](CXCursor cursor, CXCursor parent, CXClientData data)
	{
		auto container = static_cast<List*>(data);//get the container

		container->emplace_back(cursor);

		if (cursor.kind == CXCursor_LastPreprocessing)
			return CXChildVisit_Break;

		return CXChildVisit_Continue;
	};

	clang_visitChildren(m_Handle, visitor, &children);

	return children;
}

void Cursor::VisitChildren(Visitor visitor, void* data)
{
	clang_visitChildren(m_Handle, visitor, data);
}

uint32_t Cursor::GetHash() const
{
	return clang_hashCursor(m_Handle);
}
