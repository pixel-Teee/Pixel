#include "pxpch.h"

//------my library------
#include "ShaderMainFunction.h"
#include "ShaderStringFactory.h"
#include "InputNode.h"
#include "OutputNode.h"
//------my library------

namespace Pixel
{
    bool ShaderMainFunction::GetShaderTreeString(std::string& OutString)
    {
        if (m_bIsVisited)
            return true;
        else
        {
            m_bIsVisited = true;

            //declare non user variable

            //get the normal shader code
            GetNormalString(OutString);

            for (size_t i = 0; i < m_pInputs.size(); ++i)
            {
                if (m_pInputs[i]->GetOutputLink() == nullptr)
                    continue;
                else if (m_pInputs[i] == GetNormalNode())
                    continue;
                else
                {
                    std::static_pointer_cast<ShaderFunction>(m_pInputs[i]->GetOutputLink()->GetOwner())
                       ->GetShaderTreeString(OutString);
                }
            }
        }

        if (!GetInputValueString(OutString))
            return false;
        if (!GetOutputValueString(OutString))
            return false;

        //get function string, but my render pipeline is deferred rendering
        GetFunctionString(OutString);

        return true;
    }

	Ref<InputNode> ShaderMainFunction::GetNormalNode()
	{
        return m_pInputs[PutNodeType::IN_NORMAL];
	}

	void ShaderMainFunction::ConstructPutNodeAndSetPutNodeOwner()
	{
        std::string InputName = "Normal";
        Ref<InputNode> pInputNode;
        pInputNode = CreateRef<InputNode>(PutNode::ValueType::VT_4, InputName, shared_from_this());
        m_pInputs.push_back(pInputNode);

        InputName = "Albedo";
        pInputNode = CreateRef<InputNode>(PutNode::ValueType::VT_4, InputName, shared_from_this());
        m_pInputs.push_back(pInputNode);

        InputName = "Roughness";
        pInputNode = CreateRef<InputNode>(PutNode::ValueType::VT_1, InputName, shared_from_this());
        m_pInputs.push_back(pInputNode);

        InputName = "Metallic";
        pInputNode = CreateRef<InputNode>(PutNode::ValueType::VT_1, InputName, shared_from_this());
        m_pInputs.push_back(pInputNode);

        InputName = "Ao";
        pInputNode = CreateRef<InputNode>(PutNode::ValueType::VT_1, InputName, shared_from_this());
        m_pInputs.push_back(pInputNode);
	}

    void ShaderMainFunction::GetNormalString(std::string& OutString)
    {
        if (GetNormalNode()->GetOutputLink())
        {
            (std::static_pointer_cast<ShaderFunction>(GetNormalNode()->GetOutputLink()->GetOwner()))->GetShaderTreeString(OutString);
        }
    }

	bool ShaderMainFunction::GetFunctionString(std::string& OutString) const
	{
		//OutString += "pixelOut.gBufferNormal.xyz" 

        return true;
	}

}