#pragma once

#include <glm/glm.hpp>

namespace Pixel {
	struct alignas(16) GenerateMipsCB
	{
		uint32_t SrcMipLevel;//texture level of source mip
		uint32_t NumMipLevels;//number of outmips to write:[1-4]
		uint32_t SrcDimension;//width and height of the source texture
		uint32_t IsSRGB;//in hlsl, bool is 32bit
		glm::vec2 TexelSize;//1.0 / OutMip1.Dimensions
	};

	enum GenerateMipsRootBinds
	{
		GenerateMipsCB = 0,
		SrcMip = 1,
		OutMip = 2,
		NumRootParameters = 3
	};

	class RootSignature;
	class PSO;
	class DescriptorHeap;
	class Shader;
	class DescriptorHandle;
	class CubeTexture;
	class GenerateMips
	{
	public:
		GenerateMips();

		virtual Ref<CubeTexture> GenerateCubeMapMips(Ref<CubeTexture> pCubeTexture);
	private:

		Ref<RootSignature> m_RootSignature;
		Ref<PSO> m_PipelineState;
		Ref<Shader> m_GenerateMipMapsCS;

		Ref<DescriptorHandle> m_UAVDescriptorHandle;
		Ref<DescriptorHeap> m_UAVDescriptorHeap;
	};
}