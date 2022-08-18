#include "pxpch.h"
#include "TexSampler.h"

#include "Pixel/Renderer/3D/Renderer3D.h"
#include "Pixel/Renderer/3D/ShaderStringFactory.h"

namespace Pixel {
	
	TexSampler::TexSampler(const std::string& name, Ref<Material> pMaterial):ShaderFunction(name, pMaterial)
	{
		m_functionType = ShaderFunction::Sampler2D;
	}

	TexSampler::~TexSampler()
	{

	}

	bool TexSampler::GetInputValueString(std::string& OutString) const
	{
		std::string Temp;
		if (m_pInput[0]->GetValueType() == PutNode::VT_1)
		{
			OutString += Renderer3D::Float() + " ";
			Temp = Renderer3D::FloatConst("0");
		}
		else if (m_pInput[0]->GetValueType() == PutNode::VT_2)
		{
			OutString += Renderer3D::Float2() + " ";
			Temp = Renderer3D::FloatConst2("0", "0");
		}
		else if (m_pInput[0]->GetValueType() == PutNode::VT_3)
		{
			OutString += Renderer3D::Float3() + " ";
			Temp = Renderer3D::FloatConst3("0", "0", "0");
		}
		else if (m_pInput[0]->GetValueType() == PutNode::VT_4)
		{
			OutString += Renderer3D::Float4() + " ";
			Temp = Renderer3D::FloatConst4("0", "0", "0", "0");
		}

		//fix: hard code
		if (!m_pInput[0]->GetOutputLink())
		{
			OutString += m_pInput[0]->GetNodeName() + " = " + "v_TexCoord;\n";
			return true;
		}
		OutString += GetValueEqualString(m_pInput[0]->GetOutputLink(), m_pInput[0]);
		return true;
	}

	bool TexSampler::GetOutputValueString(std::string& OutString) const
	{
		std::string Temp;
		if (m_pOutput[0]->GetValueType() == PutNode::VT_1)
		{
			OutString += Renderer3D::Float() + " ";
			Temp = Renderer3D::FloatConst("0");
		}
		else if (m_pOutput[0]->GetValueType() == PutNode::VT_2)
		{
			OutString += Renderer3D::Float2() + " ";
			Temp = Renderer3D::FloatConst2("0", "0");
		}
		else if (m_pOutput[0]->GetValueType() == PutNode::VT_3)
		{
			OutString += Renderer3D::Float3() + " ";
			Temp = Renderer3D::FloatConst3("0", "0", "0");
		}
		else if (m_pOutput[0]->GetValueType() == PutNode::VT_4)
		{
			OutString += Renderer3D::Float4() + " ";
			Temp = Renderer3D::FloatConst4("0", "0", "0", "0");
		}

		OutString += m_pOutput[0]->GetNodeName() + " = " + Temp + ";\n";
		return true;
	}

	bool TexSampler::GetFunctionString(std::string& OutString) const
	{
		//texture(map, texcoord)
		OutString += GetOutputNode(0)->GetNodeName() + " = ";
		OutString += "texture(" + m_ShowName + ", " + GetInputNode(0)->GetNodeName() + ");\n";

		//decode normal
		if (m_bIsNormal)
		{
			//fix: hard code
			OutString += GetOutputNode(0)->GetNodeName() + " = " +
				"FromTangentToWorld(" + GetOutputNode(0)->GetNodeName() + ", v_Normal, v_Pos, v_TexCoord);\n";
		}
		return true;
	}

	bool TexSampler::ResetValueType() const
	{
		//texcoord
		std::string InputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string InputName = "Tex2DSamplerInput" + InputId;
		m_pInput[0]->SetNodeName(InputName);

		//color
		std::string OutputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string OutputName = "Tex2DSamplerOutput" + OutputId;
		m_pOutput[0]->SetNodeName(OutputName);

		std::string OutputNameR = Renderer3D::GetValueElement(GetOutputNode(0), Renderer3D::VE_R);
		m_pOutput[1]->SetNodeName(OutputNameR);

		std::string OutputNameG = Renderer3D::GetValueElement(GetOutputNode(0), Renderer3D::VE_G);
		m_pOutput[2]->SetNodeName(OutputNameG);

		std::string OutputNameB = Renderer3D::GetValueElement(GetOutputNode(0), Renderer3D::VE_B);
		m_pOutput[3]->SetNodeName(OutputNameB);

		std::string OutputNameA = Renderer3D::GetValueElement(GetOutputNode(0), Renderer3D::VE_A);
		m_pOutput[4]->SetNodeName(OutputNameA);

		return true;
	}

	void TexSampler::ConstructPutNodeAndSetPutNodeOwner()
	{
		std::string InputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		//TexCoord
		std::string InputName = "Tex2DSamplerInput" + InputId;
		Ref<InputNode> pInputNode = nullptr;
		pInputNode = CreateRef<InputNode>(PutNode::VT_2, InputName, shared_from_this());
		m_pInput.push_back(pInputNode);

		std::string OutputId = std::to_string(++ShaderStringFactory::m_ShaderValueIndex);
		std::string OutputName = "Tex2DSamplerOutput" + OutputId;
		Ref<OutputNode> pOutputNode = nullptr;
		pOutputNode = CreateRef<OutputNode>(PutNode::VT_4, OutputName, shared_from_this());
		m_pOutput.push_back(pOutputNode);

		//R
		std::string OutputNameR = Renderer3D::GetValueElement(GetOutputNode(0), Renderer3D::VE_R);
		pOutputNode = CreateRef<OutputNode>(PutNode::VT_1, OutputNameR, shared_from_this());
		m_pOutput.push_back(pOutputNode);
		//G
		std::string OutputNameG = Renderer3D::GetValueElement(GetOutputNode(0), Renderer3D::VE_G);
		pOutputNode = CreateRef<OutputNode>(PutNode::VT_1, OutputNameG, shared_from_this());
		m_pOutput.push_back(pOutputNode);
		//B
		std::string OutputNameB = Renderer3D::GetValueElement(GetOutputNode(0), Renderer3D::VE_B);
		pOutputNode = CreateRef<OutputNode>(PutNode::VT_1, OutputNameB, shared_from_this());
		m_pOutput.push_back(pOutputNode);
		//A
		std::string OutputNameA = Renderer3D::GetValueElement(GetOutputNode(0), Renderer3D::VE_A);
		pOutputNode = CreateRef<OutputNode>(PutNode::VT_1, OutputNameA, shared_from_this());
		m_pOutput.push_back(pOutputNode);
	}

	void TexSampler::SetTexture(Ref<Texture2D> texture)
	{
		m_pTexture = texture;
	}

	Ref<Texture2D> TexSampler::GetTexture()
	{
		return m_pTexture;
	}

	bool TexSampler::IsNormal()
	{
		return m_bIsNormal;
	}

	void TexSampler::SetNormal(bool bIsNormal)
	{
		m_bIsNormal = bIsNormal;
	}

}