#pragma once
#include <vector>
#include <DirectXMath.h>

struct BGRAColor
{
	unsigned char a;
	unsigned char r;
	unsigned char g;
	unsigned char b;
};

//描述一个顶点
class VertexLayout
{
public:
	enum ElementType
	{
		Position2D,
		Position3D,
		Texture2D,
		Normal,
		Float3Color,
		Float4Color,
		BGRAColor
	};

	class Element
	{
	public:
		Element(ElementType intype, size_t inoffset)
			:type(intype), offset(inoffset)
		{
		}
		size_t Size()const noexcept(!IS_DEBUG)
		{
			//当前元素的大小
			return SizeOf(type);
		}

		size_t GetOffset()const
		{
			return offset;
		}

		size_t GetOffsetAfter()const noexcept(!IS_DEBUG)
		{
			return Size() + offset;
		}

		ElementType GetType()const noexcept
		{
			return type;
		}
		//constexpr 这个函数就会在编译期直接计算结果
		//constexpr size_t s = SizeOf(VertexLayout::Position3D);
		// 编译期直接替换成: s = 12;
		static constexpr size_t SizeOf(ElementType type) noexcept(!IS_DEBUG)
		{
			using namespace DirectX;

			switch (type)
			{
			case VertexLayout::Position2D:
				return sizeof(XMFLOAT2);
				break;
			case VertexLayout::Position3D:
				return sizeof(XMFLOAT3);
				break;
			case VertexLayout::Texture2D:
				return sizeof(XMFLOAT2);
				break;
			case VertexLayout::Normal:
				return sizeof(XMFLOAT3);
				break;
			case VertexLayout::Float3Color:
				return sizeof(XMFLOAT3);
				break;
			case VertexLayout::Float4Color:
				return sizeof(XMFLOAT3);
				break;
			case VertexLayout::BGRAColor:
				return sizeof(::BGRAColor);
				break;
			}
			assert("Invalid Element Type" && false);
			return 0u;
		}

	private:
		ElementType type;
		//offset表示当前位置 既前面元素的大小
		size_t offset;
	};

public:
	//解析
	template<ElementType Type>
	const Element& Resolve()const noexcept(!IS_DEBUG)
	{
		for (auto& e : elements)
		{
			if (e.GetType() == Type)
			{
				return e;
			}
		}
		assert("Could not resolve element type" && false);
		return elements.front();
	}

	const Element& ResolveByIndex(size_t index)const noexcept(!IS_DEBUG)
	{
		return elements[index];
	}
	template<ElementType Type>
	VertexLayout& Append()noexcept(!IS_DEBUG)
	{
		elements.emplace_back(Type, Size());
		return *this;
	}

	size_t Size()const noexcept(!IS_DEBUG)
	{
		return elements.empty() ? 0u : elements.back().GetOffsetAfter();
	}
	size_t GetElementCount() const noexcept
	{
		return elements.size();
	}
private:
	std::vector<Element> elements;
};


class Vertex
{
	friend class VertexBuffer;
public:
	
	//拿到指向当前元素的指针
	//分支条件在编译期决策，不满足的分支根本不会编译
	template<VertexLayout::ElementType Type>
	auto& Attr()noexcept(!IS_DEBUG)
	{
		using namespace DirectX;

		const auto& element = layout.Resolve<Type>();
		auto pAttribute = pData + element.GetOffset();

		if constexpr (Type == VertexLayout::Position2D)
		{
			return *reinterpret_cast<XMFLOAT2*>(pAttribute);
		}
		else if constexpr (Type == VertexLayout::Position3D)
		{
			return *reinterpret_cast<XMFLOAT3*>(pAttribute);
		}
		else if constexpr (Type == VertexLayout::Texture2D)
		{
			return *reinterpret_cast<XMFLOAT2*>(pAttribute);
		}
		else if constexpr (Type == VertexLayout::Normal)
		{
			return *reinterpret_cast<XMFLOAT3*>(pAttribute);
		}
		else if constexpr (Type == VertexLayout::Float3Color)
		{
			return *reinterpret_cast<XMFLOAT3*>(pAttribute);
		}
		else if constexpr (Type == VertexLayout::Float4Color)
		{
			return *reinterpret_cast<XMFLOAT4*>(pAttribute);
		}
		else if constexpr (Type == VertexLayout::BGRAColor)
		{
			return *reinterpret_cast<BGRAColor*>(pAttribute);
		}
		else
		{
			assert("Bad element Type" && false);
			return *reinterpret_cast<char*>(pAttribute);
		}
		
	}
	//T&&：既能接左值又能接右值  std::forward<T>：保留原来的左值/右值属性进行转发
	//两者结合 = 完美转发
	template<typename T>
	void SetAttributeByIndex(size_t i, T&& val)noexcept(!IS_DEBUG)
	{
		using namespace DirectX;
		const auto& element = layout.ResolveByIndex(i);
		auto pAttribute = pData + element.GetOffset();

		switch (element.GetType())
		{
		case VertexLayout::Position2D:
			SetAttribute<XMFLOAT2>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Position3D:
			SetAttribute<XMFLOAT3>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Texture2D:
			SetAttribute<XMFLOAT2>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Normal:
			SetAttribute<XMFLOAT3>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Float3Color:
			SetAttribute<XMFLOAT3>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::Float4Color:
			SetAttribute<XMFLOAT4>(pAttribute, std::forward<T>(val));
			break;
		case VertexLayout::BGRAColor:
			SetAttribute<BGRAColor>(pAttribute, std::forward<T>(val));
			break;
		default:
			assert("Bad element type" && false);
		}
	}

protected:
	Vertex(char* pDataIn, const VertexLayout& layoutin)
		:pData(pDataIn),layout(layoutin)
	{
	
	}
	//调用此函数
	template<typename First,typename ...Rest>
	void SetAttributeByIndex(size_t i ,First&& first, Rest&& ...rest) noexcept(!IS_DEBUG)
	{
		SetAttributeByIndex(i, std::forward<First>(first));
		SetAttributeByIndex(i +1 , std::forward<Rest>(rest)...);
	}
private:
	//编译时检查类型并赋值
	template<typename Dest,typename Src>
	void SetAttribute(char* pAttribute, Src&& val) noexcept(!IS_DEBUG)
	{
		//::value 表示bool
		if constexpr (std::is_assignable<Dest, Src>::value)
		{
			*reinterpret_cast<Dest*>(pAttribute) = val;
		}
		else
		{
			assert("Parameter attribute type mismatch" && false);
		}
	}
	
	
private:
	//指针指向VertexBuffer
	char* pData = nullptr;
	const VertexLayout& layout;
};


class ConstVertex
{
public:
	ConstVertex(const Vertex& v) noexcept(!IS_DEBUG)
		:
		vertex(v)
	{}
	template<VertexLayout::ElementType Type>
	const auto& Attr() const noexcept(!IS_DEBUG)
	{
		return const_cast<Vertex&>(vertex).Attr<Type>();
	}
private:
	Vertex vertex;
};

class VertexBuffer
{
public:
	VertexBuffer(VertexLayout Inlayout) noexcept(!IS_DEBUG)
		:layout(std::move(Inlayout))
	{
		
	}
	const VertexLayout& GetLayout()const noexcept
	{
		return layout;
	}

	template<typename ...Params>
	void EmplaceBack(Params&&... params) noexcept(!IS_DEBUG)
	{
		//resize buffer 到+一个Layout的大小 既一个元素大小
		assert(sizeof...(params) == layout.GetElementCount() && "Param count doesn't match number of vertex elements");
		buffer.resize(buffer.size() + layout.Size());
		Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
	}
	
	//计算一共多少个顶点
	size_t Size()const noexcept(!IS_DEBUG)
	{
		//总字节数 / 单个元素大小
		return buffer.size() / layout.Size();
	}
	
	Vertex Front() noexcept(!IS_DEBUG)
	{
		assert(buffer.size() != 0u);
		return Vertex{ buffer.data(),layout };
	}

	Vertex Back() noexcept(!IS_DEBUG)
	{
		assert(buffer.size() != 0u);
		return Vertex{ buffer.data() + buffer.size() - layout.Size(),layout};
	}

	Vertex operator[](size_t i) noexcept(!IS_DEBUG)
	{
		assert(i < Size());
		return Vertex{ buffer.data() + layout.Size() * i,layout };
	}

	ConstVertex Back() const noexcept(!IS_DEBUG)
	{
		return const_cast<VertexBuffer*>(this)->Back();
	}
	ConstVertex Front() const noexcept(!IS_DEBUG)
	{
		return const_cast<VertexBuffer*>(this)->Front();
	}
	ConstVertex operator[](size_t i) const noexcept(!IS_DEBUG)
	{
		return const_cast<VertexBuffer&>(*this)[i];
	}

private:
	std::vector<char> buffer;
	VertexLayout layout;
};