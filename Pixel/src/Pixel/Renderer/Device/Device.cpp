#include "pxpch.h"

#include "Device.h"

#include "Pixel/Renderer/Renderer.h"
#include "Platform/DirectX/DirectXDevice.h"

namespace Pixel {

	Device::~Device()
	{
		
	}

	Ref<Device> Device::Get()
	{
		if (m_pDevice == nullptr)
			m_pDevice = CreateRef<DirectXDevice>();
		return m_pDevice;
	}

	void Device::SetNull()
	{
		m_pDevice = nullptr;
	}

	Ref<Device> Device::m_pDevice = nullptr;

}