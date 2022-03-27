#pragma once

#include <string>

#include "Pixel/Core/Core.h"

namespace Pixel {

	enum TextureFormat
	{
		RGB,
		RGBA,
		RED
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		virtual void SetData(void* data, uint32_t size) = 0;

		virtual void Bind(uint32_t slot = 0) const = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(uint32_t width, uint32_t height, TextureFormat textureFormat);
		static Ref<Texture2D> Create(const std::string& path);
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
}
