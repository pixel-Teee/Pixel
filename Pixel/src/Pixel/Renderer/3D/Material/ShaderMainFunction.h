#pragma once

#include "ShaderFunction.h"

namespace Pixel
{
	class InputNode;

	class ShaderMainFunction : public ShaderFunction
	{
	public:
		enum OutputStringType
		{
			OST_MATERIAL,
			OST_MAX
		};

		enum ValueUseString
		{
			VUS_WORLD_POS = BIT(1),
			VUS_VIEW_POS = BIT(2) | VUS_WORLD_POS,
			VUS_PROJ_POS = BIT(3) | VUS_VIEW_POS,
			VUS_WORLD_NORMAL = BIT(4),
			VUS_VIEW_NORMAL = BIT(5) | VUS_WORLD_NORMAL,
			VUS_VIEW_WORLD_DIR = BIT(6),
			VUS_ALL = VUS_PROJ_POS | VUS_VIEW_POS | VUS_VIEW_WORLD_DIR
		};

		ShaderMainFunction() = default;
		virtual ~ShaderMainFunction() = default;

		virtual bool GetInputValueString(std::string& OutString, uint32_t uiOutPutStringType);

		//virtual bool GetFunctionString(std::string& OutString) const override;

		virtual bool GetShaderTreeString(std::string& OutString, uint32_t uiOutPutStringType);

		//------four userful node------
		virtual Ref<InputNode> GetWorldPosNode() = 0;

		virtual Ref<InputNode> GetAlbedoNode() = 0;

		virtual Ref<InputNode> GetRoughness() = 0;

		virtual Ref<InputNode> GetMetallicNode() = 0;

		virtual Ref<InputNode> GetNormalNode() = 0;

		virtual Ref<InputNode> GetEmissiveNode() = 0;
		//------four userful node------

		//------non user declare type---
		void GetValueUseDeclareString(std::string& OutString, uint32_t uiValueUseString);
		//vec3 WorldNormal etc.
		virtual void GetValueUseString(std::string& OutString, uint32_t uiOutPutStringType);
		//------non user declare type---

		//------first get the main function normal node pinput node------
		void GetNormalString(std::string& OutString);
		//------first get the main function normal node pinput node------
	};
}