#include "pxpch.h"

#include "TextureCoordinate.h"
#include "ShaderFunction.h"
#include "ShaderStringFactory.h"
#include "InputNode.h"
#include "OutputNode.h"

namespace Pixel {
	TextureCoordinate::TextureCoordinate()
	{
		m_InputNodeDisplayColor = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) };
		m_InputNodeDisplayName = { "x", "y" };//x, y

		m_OutputNodeDisplayColor = { glm::vec3(1.0f, 1.0f, 1.0f) };
		m_OutputNodeDisplayName = { "" };

		m_HeaderColor = { 0.8f, 0.91f, 0.81f };
	}

	TextureCoordinate::TextureCoordinate(const std::string& showName, Ref<Material> pMaterial) : ShaderFunction(showName, pMaterial)
	{
		m_InputNodeDisplayColor = { glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) };
		m_InputNodeDisplayName = { "x", "y" };//x, y

		m_OutputNodeDisplayColor = { glm::vec3(1.0f, 1.0f, 1.0f) };
		m_OutputNodeDisplayName = { "" };

		m_HeaderColor = { 0.8f, 0.91f, 0.81f };
	}

	TextureCoordinate::~TextureCoordinate()
	{

	}

	bool TextureCoordinate::GetFunctionString(std::string& OutString) const
	{
		//TODO:in the future, will use CreatePixelInputDeclare's variable
		OutString += GetOutputNode(OUT_COLOR)->GetNodeName() + " = " + "pin.TexCoord;\n";
		return true;
	}

	void TextureCoordinate::ConstructPutNodeAndSetPutNodeOwner()
	{
		ShaderFunction::ConstructPutNodeAndSetPutNodeOwner();

		std::string OutputId = std::to_string(ShaderStringFactory::m_ShaderValueIndex);
		std::string OutputName = "TexCoordinate" + OutputId;
		Ref<OutputNode> pOutputNode = nullptr;
		pOutputNode = CreateRef<OutputNode>(ValueType::VT_2, OutputName, shared_from_this());
		m_pOutputs.push_back(pOutputNode);
	}

	void TextureCoordinate::ResetInShaderName()
	{
		std::string OutputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string OutputName = "TexCoordinate" + OutputId;
		m_pOutputs[0]->SetNodeName(OutputName);
	}

}