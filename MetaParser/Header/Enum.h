#pragma once

#include "LanguageType.h"

class Enum : public LanguageType
{
public:
	struct Value
	{
		Value(Enum* parent, const Cursor& cursor);

		std::string key;
		std::string value;
	};

	friend struct Value;

	Enum(const Cursor& cursor, const Namespace& currentNamespace);

	virtual ~Enum() {}

	bool ShouldCompile() const;

	kainjow::mustache::data CompileTemplate(const ReflectionParser* context) const override;

private:

	std::string m_Name;

	std::string m_DisplayName;

	std::string m_QualifiedName;

	std::vector<Value> m_Values;

	bool IsAccessible() const;
};