#pragma once

//token consists of the token enum and value(std::string)
template<typename TokenEnumType, typename InputType>
struct Token {
	using EnumType = TokenEnumType;//use for TokenizerResult
	using InputValueType = InputType;

	TokenEnumType type;
	InputType value;

	Token();
	Token(TokenEnumType type, const InputType& value);
};

#include "Token.hpp"