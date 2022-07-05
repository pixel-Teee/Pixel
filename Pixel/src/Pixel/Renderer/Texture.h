#pragma once

#include <string>

#include "Pixel/Core/Core.h"
#include "Pixel/Renderer/RendererType.h"

namespace Pixel {

	enum TextureFormat
	{
		RGB,
		RGBA,
		RED
	};

	class DescriptorCpuHandle;
	class DescriptorHandle;
	class Texture
	{
	public:
		virtual ~Texture() = default;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint64_t GetRendererID() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		//virtual void SetData(void* data, )

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool operator==(const Texture& other) const = 0;

		virtual Ref<DescriptorCpuHandle> GetCpuDescriptorHandle() = 0;

		virtual Ref<DescriptorHandle> GetHandle() const = 0;

		virtual Ref<DescriptorHandle> GetRtvHandle() const = 0;

		virtual std::string& GetPath() = 0;
	};

	class Device;
	class ContextManager;
	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height, TextureFormat textureFormat);
		static Ref<Texture2D> Create(const std::string& path);
		static Ref<Texture2D> Create(uint32_t RowPitch, uint32_t width, uint32_t height, ImageFormat textureFormat,
		const void* InitialData);

		virtual bool operator==(const Texture& other) const = 0;

		virtual Ref<DescriptorCpuHandle> GetCpuDescriptorHandle() = 0;
		virtual Ref<DescriptorHandle> GetHandle() const = 0;
		virtual Ref<DescriptorHandle> GetRtvHandle() const = 0;
	};

	enum FaceTarget {
		Right = 0,
		Left = 1,
		Top = 2,
		Bottom = 3,
		Back = 4,
		Front = 5
	};

	class CubeMap
	{
	public:
		virtual ~CubeMap() = default;
		virtual void SetFace(FaceTarget faceIndex, const std::string& path) = 0;
		virtual void GenerateCubeMap() = 0;
		virtual std::vector<std::string>& GetPaths() = 0;
		virtual void Bind(uint32_t slot = 0) = 0;
		virtual void UnBind() = 0;
		virtual void SetDirty(bool value) = 0;
		virtual bool IsDirty() = 0;
		static Ref<CubeMap> Create(std::vector<std::string>& paths);
	};

	class CubeTexture
	{
	public:
		virtual ~CubeTexture() = default;
		virtual Ref<DescriptorHandle> GetRtvHandle(uint32_t mipSlice, uint32_t arraySlice) = 0;
		virtual Ref<DescriptorHandle> GetSrvHandle() = 0;
		virtual Ref<DescriptorHandle> GetUavHandle(uint32_t mipSlice, uint32_t arraySlice) = 0;
		static Ref<CubeTexture> Create(uint32_t width, uint32_t height, ImageFormat format);
		static Ref<CubeTexture> Create(uint32_t width, uint32_t height, ImageFormat format, uint32_t MaxMipLevels);
	};
}
