#pragma once

namespace Pixel {
	enum class RangeType
	{
		SRV = 0,
		UAV,
		CBV,
		SAMPLER
	};

	enum class ShaderVisibility
	{
		ALL = 0,
		Vertex,
		Hull,
		Domain,
		Geometry,
		Pixel,
		Amplification,
		Mesh
	};

	class RootParameter
	{
	public:
		//------initialize as descriptor range------
		virtual void InitAsDescriptorRange(RangeType Type, uint32_t Register,
		uint32_t Count, ShaderVisibility Visibility = ShaderVisibility::ALL, uint32_t Space = 0) = 0;
		virtual void InitAsDescriptorTable(uint32_t RangeCount, ShaderVisibility Visibility = ShaderVisibility::ALL) = 0;
		virtual void SetTableRange(uint32_t RangeIndex, RangeType Type, uint32_t Register, uint32_t Count, uint32_t Space = 0) = 0; 
		//------initialize as descriptor range------

		//------initialize as root constant------
		virtual void InitAsConstants(uint32_t Register, uint32_t NumDwords, ShaderVisibility Visibility = ShaderVisibility::ALL, uint32_t Space = 0) = 0;
		//------initialize as root constant------

		//------initialize as constant buffer view------
		virtual void InitAsConstantBuffer(uint32_t Register, ShaderVisibility Visibility = ShaderVisibility::ALL, uint32_t Space = 0) = 0;
		//------initialize as constant buffer view------

		//------initialize as shader resource view------
		virtual void InitiAsBufferSRV(uint32_t Register, ShaderVisibility Visibility = ShaderVisibility::ALL, uint32_t Space = 0) = 0;
		//------initialize as shader resource view------

		//------initialize as unordered accessed view------
		virtual void InitAsBufferUAV(uint32_t Register, ShaderVisibility Visibility = ShaderVisibility::ALL, uint32_t Space = 0) = 0;
		//------initialize as unordered accessed view------

		virtual void Clear() = 0;

		static Ref<RootParameter> Create();
	};
}
