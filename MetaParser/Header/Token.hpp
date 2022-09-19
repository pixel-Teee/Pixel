#include "PreCompiled.h"

#include "Header/Token.h"

template<typename TokenEnumType, typename InputType>
Token<TokenEnumType, InputType>::Token()
	:type(TokenEnumType::Invalid)
{

}

template<typename TokenEnumType, typename InputType>
Token<TokenEnumType, InputType>::Token(TokenEnumType type, const InputType& value)
	:type(type),
	value(value)
{

}