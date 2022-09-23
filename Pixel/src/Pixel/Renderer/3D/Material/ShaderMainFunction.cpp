#include "pxpch.h"

//------my library------
#include "Material.h"
#include "ShaderMainFunction.h"
#include "ShaderStringFactory.h"
#include "InputNode.h"
#include "OutputNode.h"
//------my library------

namespace Pixel
{
    ShaderMainFunction::ShaderMainFunction()
    {
    }

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
        pInputNode = CreateRef<InputNode>(ValueType::VT_4, InputName, shared_from_this());
        m_pInputs.push_back(pInputNode);

        InputName = "Albedo";
        pInputNode = CreateRef<InputNode>(ValueType::VT_4, InputName, shared_from_this());
        m_pInputs.push_back(pInputNode);

        InputName = "Roughness";
        pInputNode = CreateRef<InputNode>(ValueType::VT_1, InputName, shared_from_this());
        m_pInputs.push_back(pInputNode);

        InputName = "Metallic";
        pInputNode = CreateRef<InputNode>(ValueType::VT_1, InputName, shared_from_this());
        m_pInputs.push_back(pInputNode);

        InputName = "Ao";
        pInputNode = CreateRef<InputNode>(ValueType::VT_1, InputName, shared_from_this());
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
        //OutString += "PixelOut PS(VertexOut pin)\n{\n";
        OutString += "PixelOut pixelOut = (PixelOut)(0.0f);\n";

        OutString += "pixelOut.gBufferPosition.xyz = pin.PosW;\n";

		//OutString += "pixelOut.gBufferNormal.xyz" 
        OutString += "pixelOut.gBufferNormal.xyz = " + ShaderStringFactory::GetValueElement(m_pInputs[IN_NORMAL], (ShaderStringFactory::ValueElement)(ShaderStringFactory::VE_R | ShaderStringFactory::VE_G | ShaderStringFactory::VE_B)) + ";\n";

        OutString += "pixelOut.gBufferAlbedo.w = " + ShaderStringFactory::GetValueElement(m_pInputs[IN_ALBEDO], (ShaderStringFactory::ValueElement)(ShaderStringFactory::VE_R | ShaderStringFactory::VE_G | ShaderStringFactory::VE_B)) + ";\n";

        OutString += "pixelOut.gVelocity.w = ClearCoat;\n";

        OutString += "pixelOut.gBufferRoughnessMetallicAo.x = " + ShaderStringFactory::GetValueElement(m_pInputs[IN_ROUGHNESS], ShaderStringFactory::VE_R) + ";\n";

        OutString += "pixelOut.gBufferRoughnessMetallicAo.y = " + ShaderStringFactory::GetValueElement(m_pInputs[IN_METALLIC], ShaderStringFactory::VE_G) + ";\n";

        OutString += "pixelOut.gBufferRoughnessMetallicAo.z = " + ShaderStringFactory::GetValueElement(m_pInputs[IN_AO], ShaderStringFactory::VE_G) + ";\n";

        OutString += "pixelOut.gBufferRoughnessMetallicAo.w = ShadingModelID / 255.0f\n";

        OutString += "float2 newPos = ((pin.nowScreenPosition.xy / pin.nowScreenPosition.w) * 0.5f + 0.5f);\n";

        OutString += "float2 prePos = ((pin.preScreenPosition.xy / pin.preScreenPosition.w) * 0.5f + 0.5f);\n";

        OutString += "pixelOut.gVelocity.xy = newPos - prePos;\n";

        OutString += "pixelOut.gVelocity.z = 0.0f;\n";

        //OutString += "return pixelOut;\n}\n";
        return true;
	}

}