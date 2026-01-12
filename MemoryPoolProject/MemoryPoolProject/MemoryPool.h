#pragma once
template <typename T>
class MemoryPool
{
public:
	struct FreeNode { FreeNode* Next; };
	static MemoryPool<T>& GetInst()
	{
		static MemoryPool<T> Inst;
		return Inst;
	}

private:
	void*		m_Buffer;
	size_t		m_Capacity;
	size_t		m_ObjectSize;

	FreeNode*	m_FreeHead;



public:
	// 메모리 풀 초기화
	void Init(size_t _Capacity);
	// 스테이지 전환 등으로 풀 전체를 비워야 할 때
	void Destroy();

	T* Allocate();
	void Deallocate(T* _Obj);


private:
	MemoryPool() : m_Buffer(nullptr), m_Capacity(0), m_ObjectSize(0), m_FreeHead(nullptr) {}
	~MemoryPool() { Destroy(); }

	// 복사/대입 금지해서 싱글턴 유지
	MemoryPool(const MemoryPool&) = delete;
	MemoryPool& operator=(const MemoryPool&) = delete;
};

template<typename T>
inline void MemoryPool<T>::Init(size_t _Capacity)
{
	// 이전에 사용중인 데이터 있으면 해제해주기
	Destroy();

	m_Capacity = _Capacity;
	// 최소 크기 및 정렬 보정
	size_t size = std::max(sizeof(T), sizeof(FreeNode));
	size_t alignment = std::max(alignof(T), alignof(FreeNode));
	m_ObjectSize = (size + alignment - 1) & ~(alignment - 1);
	m_FreeHead = nullptr;

	// 버퍼에 메모리 할당
	m_Buffer = ::operator new(m_ObjectSize * m_Capacity);
	char* Ptr = static_cast<char*>(m_Buffer);
	for (size_t i = 0; i < m_Capacity; ++i)
	{
		FreeNode* Node = reinterpret_cast<FreeNode*>(Ptr);
		Node->Next = m_FreeHead;
		m_FreeHead = Node;
		Ptr += m_ObjectSize;

	}
}

// 이 풀은 객체의 생명주기를 풀에서만 관리한다. Destroy() 시점은 사실상 월드 전체를 날리는 시점이므로 외부 참조는 남아있지 않다는 가정을 둔다.
template<typename T>
inline void MemoryPool<T>::Destroy()
{

	// void 포인터 해제해주기
	if (m_Buffer)
	{
		::operator delete(m_Buffer);
		m_Buffer = nullptr;
	}

	// 메타데이터 초기화
	m_Capacity = 0;
	m_ObjectSize = 0;
}

template<typename T>
inline T* MemoryPool<T>::Allocate()
{
	if (m_FreeHead == nullptr)
	{
		// 풀 고갈 상황
		return nullptr;
	}
	// Head에서 노드 하나 꺼내오기
	FreeNode* Node = m_FreeHead;
	m_FreeHead = m_FreeHead->Next;
	// Placement new 호출로 생성자 자동 호출 및 객체 생명주기 시작
	return ::new (Node) T();
}

template<typename T>
inline void MemoryPool<T>::Deallocate(T* _Obj)
{
	// 객체의 소멸자 호출
	_Obj->~T();
	// 리스트에 반납
    FreeNode* Node = reinterpret_cast<FreeNode*>(_Obj);
    Node->Next = m_FreeHead;
    m_FreeHead = Node;
}
