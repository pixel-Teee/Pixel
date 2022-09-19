#pragma once

template<typename TokenEnumType, typename InputType>
class Tokenizer;

template<typename TokenEnumType, typename InputType>
class TokenizerState
{
public:
	using Handle = TokenizerState*;

	explicit TokenizerState(TokenEnumType acceptingType);

	void SetAcceptingType(TokenEnumType acceptingType);

	//------add edge------
	void AddEdge(Handle to, char character);

	template<typename... CharacterList>
	void AddEdge(Handle to, char character, CharacterList&&... list);
	//------add edge------

	//set looping will call add edge

	//SetLooping('a', 'b', 'c');
	template<typename... CharacterList>
	void SetLooping(CharacterList&&... list);

	void SetDefaultEdge(Handle defaultEdge);
private:
	friend class Tokenizer<TokenEnumType, InputType>;

	TokenEnumType m_AcceptingType;

	//<char*, TokenizerState*>
	//m_Edges£¬±ß
	std::unordered_map<char, Handle> m_Edges;

	Handle m_DefaultEdge;
};

#include "TokenizerState.hpp"