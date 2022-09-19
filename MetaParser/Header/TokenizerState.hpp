#include "PreCompiled.h"

#include "Header/TokenizerState.h"

template<typename TokenEnumType, typename InputType>
TokenizerState<TokenEnumType, InputType>::TokenizerState(TokenEnumType acceptingType)
	:m_AcceptingType(acceptingType),
	m_DefaultEdge(nullptr)
{
}

template<typename TokenEnumType, typename InputType>
void TokenizerState<TokenEnumType, InputType>::SetAcceptingType(TokenEnumType acceptingType)
{
	m_AcceptingType = acceptingType;
}

template<typename TokenEnumType, typename InputType>
void TokenizerState<TokenEnumType, InputType>::AddEdge(Handle to, char character)
{
	m_Edges[character] = to;
}

template<typename TokenEnumType, typename InputType>
void TokenizerState<TokenEnumType, InputType>::SetDefaultEdge(Handle defaultEdge)
{
	m_DefaultEdge = defaultEdge;
}

template<typename TokenEnumType, typename InputType>
template<typename ...CharacterList>
inline void TokenizerState<TokenEnumType, InputType>::AddEdge(Handle to, char character, CharacterList && ...list)
{
	AddEdge(to, character);//match first terminal function
	AddEdge(to, list...);
}

template<typename TokenEnumType, typename InputType>
template<typename ...CharacterList>
inline void TokenizerState<TokenEnumType, InputType>::SetLooping(CharacterList && ...list)
{
	AddEdge(Handle(this), list...);
}

