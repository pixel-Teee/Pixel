#include "PreCompiled.h"

#include "Header/Invokable.h"
#include "Header/MetaUtils.h"

Invokable::Invokable(const Cursor& cursor)
	:m_ReturnType(Utils::GetQualifiedName(cursor.GetReturnType()))
{
	auto type = cursor.GetType();
	uint32_t count = type.GetArgumentCount();

	m_Signature.clear();

	for (size_t i = 0; i < count; ++i)
	{
		auto argument = type.GetArgument(i);

		m_Signature.emplace_back(Utils::GetQualifiedName(argument));
	}
}