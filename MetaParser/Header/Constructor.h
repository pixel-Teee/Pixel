#pragma once

#include "LanguageType.h"
#include "Invokable.h"

class Class;

class Constructor
	: public LanguageType
	, public Invokable
{
public:
	Constructor(const Cursor& cursor, const Namespace& currentNamespace, Class* parent = nullptr);

	virtual ~Constructor() {}

	bool ShouldCompile() const;

	kainjow::mustache::data CompileTemplate(const ReflectionParser* context) const override;

private:
	Class* m_Parent;

	bool IsAccessible() const;

	std::string GetTemplateParameters(bool IsDynamic) const;
};