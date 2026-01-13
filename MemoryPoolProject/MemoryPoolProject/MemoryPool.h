#pragma once
template <typename T>
class MemoryPool
	: public singleton<MemoryPool<T>>
{
	SINGLE(MemoryPool)
public:
	struct FreeNode 
	{ 
		FreeNode* Next = nullptr;
		FreeNode* Prev = nullptr;
	};
	struct IntrusiveList
	{
		FreeNode* Head = nullptr;
		FreeNode* Tail = nullptr;
		size_t	  Size = 0;

		void Pop_Front()
		{
			assert(Head);
			FreeNode* pNext = Head->Next;
			if (pNext) pNext->Prev = nullptr;
			else Tail = nullptr;
			Head = pNext;
			--Size;
		}

		void Push_Back(FreeNode* _Node)
		{
			_Node->Next = nullptr;
			if (Head == nullptr) Head = _Node;
			else Tail->Next = _Node;
			_Node->Prev = Tail;
			Tail = _Node;
			++Size;
		}

		void Remove(FreeNode* _Node)
		{
			assert(_Node && !IsEmpty());
			if (_Node->Prev) _Node->Prev->Next = _Node->Next;
			else Head = _Node->Next;
			if (_Node->Next) _Node->Next->Prev = _Node->Prev;
			else Tail = _Node->Prev;
			_Node->Next = _Node->Prev = nullptr;
			--Size;
		}

		bool IsEmpty() { return (Head == nullptr); }

		void Clear() { Head = Tail = nullptr; Size = 0; }
		void Release(size_t _Offset)
		{
			FreeNode* Node = Head;
			while (Node)
			{
				// 다음 노드를 미리 확보 (소멸자 호출 후에는 Node가 오염될 수 있음)
				FreeNode* pNext = Node->Next;
				// 정확한 객체 위치(Header 뒤)를 계산해서 소멸자 호출
				T* Obj = reinterpret_cast<T*>(reinterpret_cast<char*>(Node) + _Offset);
				// 실제 사용되고 있는 오브젝트의 소멸자 호출
				Obj->~T();
				Node = pNext;
			}
		}
		

		IntrusiveList() : Head(nullptr), Tail(nullptr) {}
	};
	


private:
	void*		m_Buffer;
	size_t		m_Capacity;
	size_t		m_ObjectSize;
	size_t		m_ObjectOffset;

	IntrusiveList	m_FreeList;
	IntrusiveList	m_UsedList;


public:
	// 메모리 풀 초기화
	void Init(size_t _Capacity);
	// 스테이지 전환 등으로 풀 전체를 비워야 할 때
	void Destroy();

	T* Allocate();
	void Deallocate(T* _Obj);
};

template<typename T>
MemoryPool<T>::MemoryPool()
	: m_Buffer(nullptr), m_Capacity(0), m_ObjectSize(0), m_ObjectOffset(0), m_FreeList{}, m_UsedList{}
{}

template<typename T>
MemoryPool<T>::~MemoryPool()
{
	Destroy();
}

template<typename T>
inline void MemoryPool<T>::Init(size_t _Capacity)
{
	// 이전에 사용중인 데이터 있으면 해제해주기
	Destroy();

	m_Capacity = _Capacity;

	// 객체의 정렬(Alignment)을 위해 padding이 들어갈 수 있도록 계산.
	size_t headerSize = sizeof(FreeNode);
	size_t alignment = alignof(T);

	// 객체가 시작될 위치 (헤더 뒤에 정렬을 맞춰서)
	m_ObjectOffset = (headerSize + alignment - 1) & ~(alignment - 1);
	m_ObjectSize = m_ObjectOffset + sizeof(T);

	// 버퍼에 메모리 할당
	m_Buffer = ::operator new(m_ObjectSize * m_Capacity);
	char* Ptr = static_cast<char*>(m_Buffer);
	for (size_t i = 0; i < m_Capacity; ++i)
	{
		FreeNode* Node = reinterpret_cast<FreeNode*>(Ptr);
		m_FreeList.Push_Back(Node);
		Ptr += m_ObjectSize;

	}
}

template<typename T>
inline void MemoryPool<T>::Destroy()
{
	m_UsedList.Release(m_ObjectOffset);
	// 리스트 상태 초기화
	m_UsedList.Clear();
	m_FreeList.Clear();


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
	if (m_FreeList.IsEmpty())
	{
		// 풀 고갈 상황
		return nullptr;
	}
	// FreeList에서 노드 하나 꺼내오기
	FreeNode* Node = m_FreeList.Head;
	m_FreeList.Pop_Front();
	// UsedList에 노드 추가하기
	m_UsedList.Push_Back(Node);
	// 헤더 뒤의 공간에 객체 생성
	char* objPtr = reinterpret_cast<char*>(Node) + m_ObjectOffset;
	// Placement new 호출로 생성자 자동 호출 및 객체 생명주기 시작
	return ::new (objPtr) T();
}

template<typename T>
inline void MemoryPool<T>::Deallocate(T* _Obj)
{
	if (!_Obj) return;

	// 객체의 소멸자 호출
	_Obj->~T();
	// 객체 주소로부터 다시 헤더(FreeNode) 주소를 찾아오기.
	char* objPtr = reinterpret_cast<char*>(_Obj);
	FreeNode* Node = reinterpret_cast<FreeNode*>(objPtr - m_ObjectOffset);
	// UsedList에서 제거
	m_UsedList.Remove(Node);
	// FreeList에 반납
	m_FreeList.Push_Back(Node);
}
