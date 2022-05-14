#pragma once

#include "Pixel/Renderer/3D/PutNode.h"
#include "Pixel/Renderer/3D/Material.h"
#include "Pixel/Renderer/EditorCamera.h"
#include "Pixel/Renderer/Framebuffer.h"
#include "Pixel/Scene/Components.h"
#include <string>

namespace Pixel{
	class Renderer3D
	{
	public:
		static void Init();

		//------Draw Model------
		static void DrawModel(const glm::mat4& transform, StaticMeshComponent& MeshComponent, MaterialComponent& Material, int EntityID);
		
		static void BeginScene(const Camera& camera, TransformComponent& trans, Ref<Framebuffer> geometryFramebuffer);
		static void BeginScene(const EditorCamera& camera, Ref<Framebuffer> geometryFramebuffer);

		static void EndScene(const EditorCamera& camera, glm::vec2& gScreenSize, std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights, Ref<Framebuffer> geoPassFramebuffer, Ref<Framebuffer> LightPassFramebuffer);
		static void EndScene(Camera& camera, TransformComponent& trans, glm::vec2& gScreenSize, std::vector<TransformComponent>& Trans, std::vector<LightComponent>& Lights, Ref<Framebuffer> geoPassFramebuffer, Ref<Framebuffer> LightPassFramebuffer);
		//------Draw Model------

		//------give editor use------
		static Ref<CubeMap> GetSkyBox();
		static Ref<CubeMap> GetDefaultSkyBox();

		static void DrawSkyBox(const EditorCamera& camera, Ref<Framebuffer> LightPassFramebuffer, Ref<Framebuffer> geoPassFramebuffer);

		static void DrawSkyBox(Camera& camera, TransformComponent& trans, Ref<Framebuffer> LightPassFramebuffer, Ref<Framebuffer> geoPassFramebuffer);

		static void DrawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3 color, glm::mat4 viewProjection);
		//-------shader generator string-------
		//------type------
		static std::string Float()
		{
			return std::string("float ");
		}

		static std::string Float2()
		{
			return std::string("vec2 ");
		}

		static std::string Float3()
		{
			return std::string("vec3 ");
		}

		static std::string Float4()
		{
			return std::string("vec4 ");
		}

		static std::string Return()
		{
			return std::string("return ");
		}
		//------type------

		//------value------
		static std::string FloatConst(const std::string& value) 
		{
			return value;
		}

		static std::string FloatConst2(const std::string& value1, const std::string& value2)
		{
			return std::string("vec2") + "(" + value1 + ", " + value2 + ")";
		}

		static std::string FloatConst3(const std::string& value1, const std::string& value2, const std::string& value3)
		{
			return std::string("vec3") + "(" + value1 + ", " + value2 + ", " + value3 + ")";
		}

		static std::string FloatConst4(const std::string& value1, const std::string& value2, const std::string& value3, const std::string& value4)
		{
			return std::string("vec4") + "(" + value1 + ", " + value2 + ", " + value3 + ", " + value4 + ")";
		}
		//------value------

		//------extract value element of PutNode, etc xxx.x------
		enum ValueElement
		{
			VE_NONE = 0,
			VE_R = 1,
			VE_G = BIT(1),
			VE_B = BIT(2),
			VE_A = BIT(3),
			DF_ALL = 0x0F
		};

		static std::string GetValueElement(Ref<PutNode> pPutNode, ValueElement valueElement)
		{
			//extract the pPutNode's value element
			std::string Temp = pPutNode->GetNodeName();

			if (valueElement > 0)
			{
				if (pPutNode->GetValueType() == ValueElement::VE_R && (valueElement & ValueElement::VE_R))
				{
					return Temp;
				}
			}
			
			std::string Value[4];
			Value[0] = "x";
			Value[1] = "y";
			Value[2] = "z";
			Value[3] = "w";

			ValueElement Mask[4];
			Mask[0] = VE_R;
			Mask[1] = VE_G;
			Mask[2] = VE_B;
			Mask[3] = VE_A;
			Temp += (".");

			//value element: need to extracted element
			for (uint32_t i = 0; i < 4; ++i)
			{
				if (i <= pPutNode->GetValueType())
				{
					if (valueElement & Mask[i])
					{
						Temp += Value[i];
					}
				}
			}

			return Temp;
		}
		//------extract value element of PutNode, etc xxx.x------

		static std::string Uniform()
		{
			return "uniform";
		}
	};

}