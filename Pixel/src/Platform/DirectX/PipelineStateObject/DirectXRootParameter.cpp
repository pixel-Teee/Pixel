#include "pxpch.h"

#include "DirectXRootParameter.h"
#include "Platform/DirectX/TypeUtils.h"

namespace Pixel {

	DirectXRootParameter::DirectXRootParameter()
	{
		m_RootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF;
	}

	DirectXRootParameter::~DirectXRootParameter()
	{
		Clear();
	}

	void DirectXRootParameter::InitAsDescriptorRange(RangeType Type, uint32_t Register, uint32_t Count,
	ShaderVisibility Visibility /*= ShaderVisibility::ALL*/, uint32_t Space /*= 0*/)
	{
		InitAsDescriptorTable(1, Visibility);

		SetTableRange(0, Type, Register, Count, Space);
	}

	void DirectXRootParameter::InitAsDescriptorTable(uint32_t RangeCount, ShaderVisibility Visibility /*= ShaderVisibility::ALL*/)
	{
		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		m_RootParam.ShaderVisibility = ShaderVisibilityToDirectXShaderVisibility(Visibility);
		m_RootParam.DescriptorTable.NumDescriptorRanges = RangeCount;
		m_RootParam.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[RangeCount];
	}

	void DirectXRootParameter::InitAsDescriptorTable(std::initializer_list<std::tuple<RangeType, uint32_t, uint32_t>> list, ShaderVisibility Visibility)
	{
		//throw std::logic_error("The method or operation is not implemented.");
		uint32_t RangeCount = list.size();

		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		m_RootParam.ShaderVisibility = ShaderVisibilityToDirectXShaderVisibility(Visibility);
		m_RootParam.DescriptorTable.NumDescriptorRanges = RangeCount;
		m_RootParam.DescriptorTable.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE[RangeCount];

		D3D12_DESCRIPTOR_RANGE* range = const_cast<D3D12_DESCRIPTOR_RANGE*>(m_RootParam.DescriptorTable.pDescriptorRanges);

		for (uint32_t i = 0; i < RangeCount; ++i)
		{
			D3D12_DESCRIPTOR_RANGE* pRange = range + i;

			RangeType rangeType = std::get<0>((*(list.begin() + i)));

			uint32_t baseRegister = std::get<1>((*(list.begin() + i)));

			uint32_t registerCount = std::get<2>(*(list.begin() + i));
			
			range->RangeType = RangeTypeToDirectXRangeType(rangeType);
			range->NumDescriptors = registerCount;
			range->BaseShaderRegister = baseRegister;
			range->RegisterSpace = 0;
			range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		}
	}

	void DirectXRootParameter::SetTableRange(uint32_t RangeIndex, RangeType Type, uint32_t Register, uint32_t Count, uint32_t Space /*= 0*/)
	{
		//get the range index's descriptor table
		D3D12_DESCRIPTOR_RANGE* range = const_cast<D3D12_DESCRIPTOR_RANGE*>(m_RootParam.DescriptorTable.pDescriptorRanges + RangeIndex);

		//set the descriptor table's start register
		range->RangeType = RangeTypeToDirectXRangeType(Type);
		range->NumDescriptors = Count;
		range->BaseShaderRegister = Register;
		range->RegisterSpace = Space;
		range->OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	}

	void DirectXRootParameter::InitAsConstants(uint32_t Register, uint32_t NumDwords, ShaderVisibility Visibility /*= ShaderVisibility::ALL*/, uint32_t Space /*= 0*/)
	{
		//32bit:double words
		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		m_RootParam.ShaderVisibility = ShaderVisibilityToDirectXShaderVisibility(Visibility);
		m_RootParam.Constants.Num32BitValues = NumDwords;
		m_RootParam.Constants.ShaderRegister = Register;
		m_RootParam.Constants.RegisterSpace = Space;
	}

	void DirectXRootParameter::InitAsConstantBuffer(uint32_t Register, ShaderVisibility Visibility /*= ShaderVisibility::ALL*/, uint32_t Space /*= 0*/)
	{
		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		m_RootParam.ShaderVisibility = ShaderVisibilityToDirectXShaderVisibility(Visibility);
		m_RootParam.Descriptor.ShaderRegister = Register;
		m_RootParam.Descriptor.RegisterSpace = Space;
	}

	void DirectXRootParameter::InitiAsBufferSRV(uint32_t Register, ShaderVisibility Visibility /*= ShaderVisibility::ALL*/, uint32_t Space /*= 0*/)
	{
		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		m_RootParam.ShaderVisibility = ShaderVisibilityToDirectXShaderVisibility(Visibility);
		m_RootParam.Descriptor.ShaderRegister = Register;
		m_RootParam.Descriptor.RegisterSpace = Space;
	}

	void DirectXRootParameter::InitAsBufferUAV(uint32_t Register, ShaderVisibility Visibility /*= ShaderVisibility::ALL*/, uint32_t Space /*= 0*/)
	{
		m_RootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
		m_RootParam.ShaderVisibility = ShaderVisibilityToDirectXShaderVisibility(Visibility);
		m_RootParam.Descriptor.ShaderRegister = Register;
		m_RootParam.Descriptor.RegisterSpace = Space;
	}

	void DirectXRootParameter::Clear()
	{
		//delete descriptor table array
		if (m_RootParam.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
			delete[]m_RootParam.DescriptorTable.pDescriptorRanges;

		m_RootParam.ParameterType = (D3D12_ROOT_PARAMETER_TYPE)0xFFFFFFFF;
	}

}