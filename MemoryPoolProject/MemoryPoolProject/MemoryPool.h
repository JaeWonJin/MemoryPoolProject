#pragma once
template <typename T>
class MemoryPool
{
public:
	static MemoryPool<T>& GetInst()
	{
		static MemoryPool<T> Inst;
		return Inst;
	}

private:
	void*		m_Buffer;
	size_t		m_Capacity;
	size_t		m_ObjectSize;
	list<T*>	m_listFreeNodes;


public:
	// 메모리 풀 초기화
	void Init(size_t _Capacity);
	// 스테이지 전환 등으로 풀 전체를 비워야 할 때
	void Destroy();

	T* Allocate();
	void Dallocate(T* _Obj);


private:
	MemoryPool() : m_Buffer(nullptr), m_Capacity(0), m_ObjectSize(0), m_listFreeNodes{} {}
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
	m_ObjectSize = sizeof(T);

	// 버퍼에 메모리 할당
	m_Buffer = ::operator new(m_ObjectSize * m_Capacity);
	char* Ptr = static_cast<char*>(m_Buffer);
	for (size_t i = 0; i < m_Capacity; ++i)
	{
		T* Node = new (Ptr) T;
		m_listFreeNodes.push_back(Node);
		Ptr += m_ObjectSize;
	}
}

// 이 풀은 객체의 생명주기를 풀에서만 관리한다. Destroy() 시점은 사실상 월드 전체를 날리는 시점이므로 외부 참조는 남아있지 않다는 가정을 둔다.
template<typename T>
inline void MemoryPool<T>::Destroy()
{
	// 이미 Init()때 모든 객체는 placement new로 생성됐으므로
	// 현재 freeNodes에 있든, 빌려간 상태든 상관없이 전체 버퍼를 순회하며 소멸자를 호출해줘야 안전함.
	// -> 포인터들을 다 들고 있지 않으니까, 그냥 한 번 전체를 깔끔하게 순회한다.

	if (m_Buffer)
	{
		char* Ptr = static_cast<char*>(m_Buffer);
		for (size_t i = 0; i < m_Capacity; ++i)
		{
			T* Obj = reinterpret_cast<T*>(Ptr);
			Obj->~T(); // 소멸자 호출
			Ptr += m_ObjectSize;
		}

		::operator delete(m_Buffer);
		m_Buffer = nullptr;
	}


	// Free 리스트 비우기
	m_listFreeNodes.clear();

	// 메타데이터 초기화
	m_Capacity = 0;
	m_ObjectSize = 0;
}

template<typename T>
inline T* MemoryPool<T>::Allocate()
{
	if (m_listFreeNodes.empty())
	{
		// 풀 고갈 상황
		return nullptr;
	}
	T* Node = m_listFreeNodes.front();
	m_listFreeNodes.pop_front();
	return Node;
}

template<typename T>
inline void MemoryPool<T>::Dallocate(T* _Obj)
{
	// 여기서 _Obj의 상태를 사용자가 재초기화하거나 비활성화하는 건 게임 로직에서 할 일
	// 풀은 그냥 반납만 받는다.
	m_listFreeNodes.push_back(_Obj);
}
