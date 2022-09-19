#pragma once

#include "LanguageType.h"

class Class;

class Field : public LanguageType
{
public:
	Field(const Cursor& cursor, const Namespace& currentNamespace, Class* parent = nullptr);

	virtual ~Field() {}

	bool ShouldCompile() const;

	kainjow::mustache::data CompileTemplate(const ReflectionParser* context) const override;

private:

	bool m_IsConst;

	bool m_HasExplicitGetter;

	bool m_HasExplicitSetter;

	//class depend filed, just pointer
	Class* m_parent;

	std::string m_Name;
	std::string m_DisplayName;
	std::string m_Type;

	std::string m_ExplicitGetter;
	std::string m_ExplicitSetter;

	std::string m_VeryExplicitGetter;
	std::string m_VeryExplicitSetter;

	bool IsAccessible() const;
	bool IsGetterAccessible() const;
	bool IsSetterAccessible() const;
};