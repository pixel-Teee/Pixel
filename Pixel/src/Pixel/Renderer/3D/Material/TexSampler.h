#pragma once

#include "Pixel/Renderer/Texture.h"
#include "Pixel/Renderer/3D/Material/ShaderFunction.h"

namespace Pixel {
	class TexSampler : public ShaderFunction
	{
	public:
		TexSampler(const std::string& showName, Ref<Material> pMaterial);

		virtual ~TexSampler();

		virtual bool GetInputValueString(std::string& OutString) const override;

		virtual bool GetOutputValueString(std::string& OutString) const override;

		virtual bool GetFunctionString(std::string& OutString) const override;

		virtual bool ResetValueType() const override;

		virtual void ConstructPutNodeAndSetPutNodeOwner() override;

		void SetTexture(Ref<Texture2D> texture);

		Ref<Texture2D> GetTexture();

		bool IsNormal();

		void SetNormal(bool bIsNormal);

	private:
		bool m_bIsNormal = false;
	};
}
