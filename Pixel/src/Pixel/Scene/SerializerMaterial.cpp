#include "pxpch.h"
#include "SerializerMaterial.h"
#include "Pixel/Renderer/3D/ShaderFunction.h"
#include "Pixel/Renderer/3D/ShaderGeometryFunction.h"
#include "Pixel/Renderer/3D/TexSampler.h"
#include "Pixel/Renderer/3D/Mul.h"

#include "yaml-cpp/yaml.h"

namespace YAML {
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};

}

namespace Pixel {

	extern YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v);

	void SerializerConstFloat4(YAML::Emitter& out, Ref<ShaderFunction> pShaderFunction)
	{
		Ref<ConstFloatValue> pConstValue = std::dynamic_pointer_cast<ConstFloatValue>(pShaderFunction);
		if (pConstValue != nullptr)
		{
			out << YAML::Key << "Custom" << YAML::Value << pConstValue->m_bIsCustom;
			out << YAML::Key << "Value" << YAML::BeginSeq;
			for (uint32_t i = 0; i < pConstValue->m_Value.size(); ++i)
			{
				out << pConstValue->m_Value[i];
			}
			out << YAML::EndSeq;
			out << YAML::Key << "ValueNumber" << YAML::Value << pConstValue->m_valueNumber;
		}
	}

	void SerializerMul(YAML::Emitter& out, Ref<ShaderFunction> pShaderFunction)
	{
		//...
	}

	void SerializerSampler2D(YAML::Emitter& out, Ref<ShaderFunction> pShaderFunction)
	{
		Ref<TexSampler> pTexSampler = std::dynamic_pointer_cast<TexSampler>(pShaderFunction);
		if (pTexSampler != nullptr)
		{
			out << YAML::Key << "TexturePath" << YAML::Value << pTexSampler->GetTexture()->GetPath();
			out << YAML::Key << "IsNormal" << YAML::Value << pTexSampler->IsNormal();
		}
	}

	uint32_t FindOutPinLocation(Ref<OutputNode> pOutput, Ref<ShaderFunction> pShaderFunction)
	{
		for (uint32_t i = 0; i < pShaderFunction->GetOutputNodeNum(); ++i)
		{
			if (pShaderFunction->GetOutputNode(i) == pOutput)
			{
				return i;
			}
		}
		return -1;
	}

	uint32_t FindInputPinLocation(Ref<InputNode> pInput, Ref<ShaderFunction> pShaderFunction)
	{
		for (uint32_t i = 0; i < pShaderFunction->GetInputNodeNum(); ++i)
		{
			if (pShaderFunction->GetInputNode(i) == pInput)
			{
				return i;
			}
		}
		return -1;
	}

	void SerializerMaterial::SerializerMaterialAsset(std::string& filePath, Ref<Material> pMaterial, Ref<MaterialInstance> pMaterialInstance)
	{
		/*------------
		Serializer Material
		------------*/

		YAML::Emitter out;
		out << YAML::BeginMap;

		//Asset Type
		out << YAML::Key << "Type" << YAML::Value << "Material";

		//Node Name
		out << YAML::Key << "ShowName" << YAML::Value << pMaterial->m_ShowName;

		out << YAML::Key << "ShaderFunction" << YAML::Value << YAML::BeginSeq;

		//serializer shader function array
		for (uint32_t i = 0; i < pMaterial->m_pShaderFunctionArray.size(); ++i)
		{
			Ref<ShaderFunction> pShaderFunction = pMaterial->m_pShaderFunctionArray[i];

			out << YAML::BeginMap;
			out << YAML::Key << "ShowName" << YAML::Value << pShaderFunction->m_ShowName;
			out << YAML::Key << "FunctionType" << YAML::Value << pShaderFunction->m_functionType;

			//in terms of function type to seralize different shader function
			switch (pShaderFunction->m_functionType)
			{
			case ShaderFunction::ShaderFunctionType::ConstFloat4:
				SerializerConstFloat4(out, pShaderFunction);
				break;
			case ShaderFunction::ShaderFunctionType::Mul:
				SerializerMul(out, pShaderFunction);
				break;
			case ShaderFunction::ShaderFunctionType::Sampler2D:
				SerializerSampler2D(out, pShaderFunction);
				break;
			}

			out << YAML::Key << "Pos" << YAML::Value << pShaderFunction->m_Pos;

			out << YAML::Key << "InputNode" << YAML::Value << YAML::BeginSeq;
			//InputNode and OutputNode
			for (uint32_t j = 0; j < pShaderFunction->GetInputNodeNum(); ++j)
			{
				if (pShaderFunction->GetInputNode(j)->GetOutputLink() != nullptr)
				{
					out << YAML::BeginMap;
					out << YAML::Key << "OwnerShowName" << YAML::Value;
					out << pShaderFunction->GetInputNode(j)->GetOutputLink()->GetOwner()->GetShowName();
					out << YAML::Key << "PinLocation" << YAML::Value;
					out << FindOutPinLocation(pShaderFunction->GetInputNode(j)->GetOutputLink(), pShaderFunction->GetInputNode(j)->GetOutputLink()->GetOwner());
					out << YAML::EndMap;
				}			
				else
				{
					out << YAML::BeginMap;
					out << YAML::Key << "OwnerShowName" << YAML::Value;
					out << "nullptr";
					out << YAML::Key << "PinLocation" << YAML::Value;
					out << -1;
					out << YAML::EndMap;
				}
			}
			out << YAML::EndSeq;

			out << YAML::Key << "OutputNode" << YAML::Value << YAML::BeginSeq;
			for (uint32_t j = 0; j < pShaderFunction->GetOutputNodeNum(); ++j)
			{
				if (pShaderFunction->GetOutputNode(j)->GetInputLink().size() != 0)
				{
					out << YAML::BeginMap;
					out << YAML::Key << "OwnerShowName" << YAML::Value;
					out << pShaderFunction->GetOutputNode(j)->GetInputLink()[0]->GetOwner()->GetShowName();
					out << YAML::Key << "PinLocation" << YAML::Value;
					out << FindInputPinLocation(pShaderFunction->GetOutputNode(j)->GetInputLink()[0], pShaderFunction->GetOutputNode(j)->GetInputLink()[0]->GetOwner());
					out << YAML::EndMap;
				}
				else
				{
					out << YAML::BeginMap;
					out << YAML::Key << "OwnerShowName" << YAML::Value;
					out << "nullptr";
					out << YAML::Key << "PinLocation" << YAML::Value;
					out << -1;
					out << YAML::EndMap;
				}
			}
			out << YAML::EndSeq;

			out << YAML::EndMap;
		}
		
		out << YAML::EndSeq;
		//------Serializer Main Shader Function------
		//------Serializer Main Shader Function------

		std::ofstream stream(filePath);
		stream << out.c_str();
		stream.close();
	}

	void DeserializerConstFloat4(YAML::Node& node, Ref<ShaderFunction>& pShaderFunction, Ref<Material>& pMaterial)
	{
		pShaderFunction = CreateRef<ConstFloatValue>(node["ShowName"].as<std::string>(), pMaterial, node["ValueNumber"].as<int32_t>(), node["Custom"].as<bool>());
		pShaderFunction->ConstrcutPutNodeAndSetPutNodeOwner();
		pShaderFunction->AddToMaterialOwner();
		uint32_t i = 0;
		for (auto value : node["Value"])
		{
			std::static_pointer_cast<ConstFloatValue>(pShaderFunction)->SetValue(i, value.as<float>());
			++i;
		}
	}

	void DeserializerSampler2D(YAML::Node& node, Ref<ShaderFunction>& pShaderFunction, Ref<Material>& pMaterial)
	{
		pShaderFunction = CreateRef<TexSampler>(node["ShowName"].as<std::string>(), pMaterial);
		pShaderFunction->ConstrcutPutNodeAndSetPutNodeOwner();
		pShaderFunction->AddToMaterialOwner();

		Ref<TexSampler> pTexSampler = std::static_pointer_cast<TexSampler>(pShaderFunction);
		//pTexSampler->Set
		pTexSampler->SetNormal(node["IsNormal"].as<bool>());

		Ref<Texture2D> pTexture = Texture2D::Create(node["TexturePath"].as<std::string>());
		pTexSampler->SetTexture(pTexture);
	}

	void DeserializerMul(YAML::Node& node, Ref<ShaderFunction>& pShaderFunction, Ref<Material>& pMaterial)
	{
		pShaderFunction = CreateRef<Mul>(node["ShowName"].as<std::string>(), pMaterial);
		pShaderFunction->ConstrcutPutNodeAndSetPutNodeOwner();
		pShaderFunction->AddToMaterialOwner();
	}

	void SerializerMaterial::DeserializerMaterialAssetAndCreateMaterial(const std::string& filepath, Ref<Material>& pOutMaterial, Ref<MaterialInstance>& pOutMaterialInstance)
	{
		std::ifstream stream(filepath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());

		if (data["Type"])
		{
			std::string FileType = data["Type"].as<std::string>();
			if (FileType != "Material") return;
			else
			{
				std::string ShowName = data["ShowName"].as<std::string>();

				pOutMaterial = CreateRef<Material>(ShowName);
				pOutMaterialInstance = CreateRef<MaterialInstance>(pOutMaterial);

				//------Link Info------//
				uint32_t i = 0;

				std::vector<std::tuple<uint32_t, uint32_t,
					std::string, uint32_t>> OutPin;

				std::vector<std::tuple<uint32_t, uint32_t,
					std::string, uint32_t>> InputPin;
				//------Link Info------//

				for (auto shaderFunction : data["ShaderFunction"])
				{
					Ref<ShaderFunction> pShaderFunction;
					switch (shaderFunction["FunctionType"].as<uint32_t>())
					{
					case ShaderFunction::ShaderFunctionType::Mul:
						DeserializerMul(shaderFunction, pShaderFunction, pOutMaterial);
						break;
					case ShaderFunction::ShaderFunctionType::ConstFloat4:
						DeserializerConstFloat4(shaderFunction, pShaderFunction, pOutMaterial);
						break;
					case ShaderFunction::ShaderFunctionType::Sampler2D:
						DeserializerSampler2D(shaderFunction, pShaderFunction, pOutMaterial);
						break;
					}

					pShaderFunction->m_Pos = shaderFunction["Pos"].as<glm::vec2>();

					uint32_t j = 0;
					//find the relationship of the outputpin
					for (auto InputNode : shaderFunction["InputNode"])
					{
						std::string OwnerShowName = InputNode["OwnerShowName"].as<std::string>();
						//if (OwnerShowName == "nullptr") continue;
						int32_t PinLocation = InputNode["PinLocation"].as<int32_t>();
						OutPin.push_back(std::make_tuple(
						i, j, OwnerShowName, PinLocation
						));
						++j;
					}

					j = 0;
					//find the relationship of the inputpin
					for (auto OutputNode : shaderFunction["OutputNode"])
					{
						std::string OwnerShowName = OutputNode["OwnerShowName"].as<std::string>();
						//if (OwnerShowName == "nullptr") continue;
						int32_t PinLocation = OutputNode["PinLocation"].as<int32_t>();
						InputPin.push_back(std::make_tuple(
							i, j, OwnerShowName, PinLocation
						));
						++j;
					}
					++i;
				}
				
				//------Link------
				for (auto PinInfo : InputPin)
				{
					Ref<ShaderFunction> pShaderFunction;
					for (auto Item : pOutMaterial->GetShaderFunction())
					{
						if (Item->GetShowName() == std::get<2>(PinInfo))
						{
							pShaderFunction = Item;
						}
					}

					if (pShaderFunction != nullptr)
					{
						pOutMaterial->GetShaderFunction()[std::get<0>(PinInfo)]
							->GetOutputNode(std::get<1>(PinInfo))->Connect(pShaderFunction->GetInputNode(std::get<3>(PinInfo)));

						pShaderFunction->GetInputNode(std::get<3>(PinInfo))->Connection(pOutMaterial->GetShaderFunction()[std::get<0>(PinInfo)]
							->GetOutputNode(std::get<1>(PinInfo)));
					}
				}

				for (auto PinInfo : OutPin)
				{
					Ref<ShaderFunction> pShaderFunction;
					for (auto Item : pOutMaterial->GetShaderFunction())
					{
						if (Item->GetShowName() == std::get<2>(PinInfo))
						{
							pShaderFunction = Item;
						}
					}

					if (pShaderFunction != nullptr)
					{
						pOutMaterial->GetShaderFunction()[std::get<0>(PinInfo)]
							->GetInputNode(std::get<1>(PinInfo))->Connection(pShaderFunction->GetOutputNode(std::get<3>(PinInfo)));

						pShaderFunction->GetOutputNode(std::get<3>(PinInfo))->Connect(pOutMaterial->GetShaderFunction()[std::get<0>(PinInfo)]
							->GetInputNode(std::get<1>(PinInfo)));
					}
				}
				//------Link------
			}		
		}
	}

}