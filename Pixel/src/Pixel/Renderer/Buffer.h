#pragma once

namespace Pixel {

	enum class ShaderDataType : uint8_t
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	//------Semantics------
	enum class Semantics : uint8_t
	{
		POSITION = 0, TEXCOORD = 1, NORMAL = 2, TANGENT = 3, BINORMAL = 4, COLOR = 5, BLENDWEIGHT = 6, BLENDINDICES = 7, 
		Editor = 8, FLOAT = 9,
		MAX = 10
	};
	//------Semantics------

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:  return 4;
			case ShaderDataType::Float2: return 4 * 2;
			case ShaderDataType::Float3: return 4 * 3;
			case ShaderDataType::Float4: return 4 * 4;
			case ShaderDataType::Mat3:   return 4 * 3 * 3;
			case ShaderDataType::Mat4:   return 4 * 4 * 4;
			case ShaderDataType::Int:    return 4;
			case ShaderDataType::Int2:   return 4 * 2;
			case ShaderDataType::Int3:   return 4 * 3;
			case ShaderDataType::Int4:   return 4 * 4;
			case ShaderDataType::Bool:   return 1;
		}
		PX_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Offset;
		uint32_t Size;
		bool Normalized;
		//------Semantics------
		Semantics m_sematics;
		//------Semantics------
		BufferElement(ShaderDataType type, const std::string& name, Semantics sematics, bool normalized = false)
			:Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized),
			m_sematics(sematics)
		{

		}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
			case ShaderDataType::Float:  return 1;
			case ShaderDataType::Float2: return 2;
			case ShaderDataType::Float3: return 3;
			case ShaderDataType::Float4: return 4;
			case ShaderDataType::Mat3:   return 3 * 3;
			case ShaderDataType::Mat4:   return 4 * 4;
			case ShaderDataType::Int:    return 1;
			case ShaderDataType::Int2:   return 2;
			case ShaderDataType::Int3:   return 3;
			case ShaderDataType::Int4:   return 4;
			case ShaderDataType::Bool:   return 1;
			}
			PX_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout(){}
		BufferLayout(const std::initializer_list<BufferElement>& element) :m_Elements(element)
		{
			CalculateOffsetAndStride();
		}
		BufferLayout(const std::vector<BufferElement>& element) : m_Elements(element)
		{
			CalculateOffsetAndStride();
		}

		inline uint32_t GetStride() const { return m_Stride;}
		inline const std::vector<BufferElement>& GetElements() const{ return m_Elements; }

		std::vector<BufferElement>::iterator begin(){ return m_Elements.begin();}
		std::vector<BufferElement>::iterator end(){ return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		void CalculateOffsetAndStride()
		{
			//每个成员的偏移
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};
	class VBV;
	class VertexBuffer
	{
	public:
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;

		/*------*/
		virtual bool HavePosition() = 0;
		virtual bool HaveNormal() = 0;
		virtual bool HaveTangent() = 0;
		virtual bool HaveBinormal() = 0;
		//may have levels vertex color
		virtual bool HaveColors(uint32_t Level) = 0;
		//may have levels vertex texcoord
		virtual bool HaveTexCoord(uint32_t Level) = 0;

		virtual bool HaveBoneIndex(uint32_t Level) = 0;
		virtual bool HaveBoneWeight(uint32_t Level) = 0;
		/*------*/

		virtual Ref<VBV> GetVBV() = 0;

		static Ref<VertexBuffer> Create(uint32_t size);
		static VertexBuffer* Create(float* vertices, uint32_t size);
		static Ref<VertexBuffer> Create(float* vertices, uint32_t ElementCount, uint32_t ElementSize);
	};
	class IBV;
	class IndexBuffer
	{
	public:
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t count) = 0;

		virtual uint32_t GetCount() const = 0;

		virtual Ref<IBV> GetIBV() = 0;

		static Ref<IndexBuffer> Create(uint32_t count);
		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	};
}
