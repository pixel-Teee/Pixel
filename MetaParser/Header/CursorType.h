#pragma once

class Cursor;

class CursorType
{
public:
	CursorType(const CXType& handle);

	std::string GetDisplayName() const;

	//get the funtion's argument count
	int32_t GetArgumentCount() const;

	CursorType GetArgument(uint32_t index) const;

	CursorType GetCanonicalType() const;

	//get the cursor for the type, then construct the Cursor class
	
	Cursor GetDeclaration() const;
	
	CXTypeKind GetKind() const;

	bool IsConst() const;
private:
	//type of the cursor
	CXType m_Handle;
};