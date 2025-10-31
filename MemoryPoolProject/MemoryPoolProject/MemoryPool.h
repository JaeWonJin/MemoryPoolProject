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
	// �޸� Ǯ �ʱ�ȭ
	void Init(size_t _Capacity);
	// �������� ��ȯ ������ Ǯ ��ü�� ����� �� ��
	void Destroy();

	T* Allocate();
	void Dallocate(T* _Obj);


private:
	MemoryPool() : m_Buffer(nullptr), m_Capacity(0), m_ObjectSize(0), m_listFreeNodes{} {}
	~MemoryPool() { Destroy(); }

	// ����/���� �����ؼ� �̱��� ����
	MemoryPool(const MemoryPool&) = delete;
	MemoryPool& operator=(const MemoryPool&) = delete;
};

template<typename T>
inline void MemoryPool<T>::Init(size_t _Capacity)
{
	// ������ ������� ������ ������ �������ֱ�
	Destroy();

	m_Capacity = _Capacity;
	m_ObjectSize = sizeof(T);

	// ���ۿ� �޸� �Ҵ�
	m_Buffer = ::operator new(m_ObjectSize * m_Capacity);
	char* Ptr = static_cast<char*>(m_Buffer);
	for (size_t i = 0; i < m_Capacity; ++i)
	{
		T* Node = new (Ptr) T;
		m_listFreeNodes.push_back(Node);
		Ptr += m_ObjectSize;
	}
}

// �� Ǯ�� ��ü�� �����ֱ⸦ Ǯ������ �����Ѵ�. Destroy() ������ ��ǻ� ���� ��ü�� ������ �����̹Ƿ� �ܺ� ������ �������� �ʴٴ� ������ �д�.
template<typename T>
inline void MemoryPool<T>::Destroy()
{
	// �̹� Init()�� ��� ��ü�� placement new�� ���������Ƿ�
	// ���� freeNodes�� �ֵ�, ������ ���µ� ������� ��ü ���۸� ��ȸ�ϸ� �Ҹ��ڸ� ȣ������� ������.
	// -> �����͵��� �� ��� ���� �����ϱ�, �׳� �� �� ��ü�� ����ϰ� ��ȸ�Ѵ�.

	if (m_Buffer)
	{
		char* Ptr = static_cast<char*>(m_Buffer);
		for (size_t i = 0; i < m_Capacity; ++i)
		{
			T* Obj = reinterpret_cast<T*>(Ptr);
			Obj->~T(); // �Ҹ��� ȣ��
			Ptr += m_ObjectSize;
		}

		::operator delete(m_Buffer);
		m_Buffer = nullptr;
	}


	// Free ����Ʈ ����
	m_listFreeNodes.clear();

	// ��Ÿ������ �ʱ�ȭ
	m_Capacity = 0;
	m_ObjectSize = 0;
}

template<typename T>
inline T* MemoryPool<T>::Allocate()
{
	if (m_listFreeNodes.empty())
	{
		// Ǯ �� ��Ȳ
		return nullptr;
	}
	T* Node = m_listFreeNodes.front();
	m_listFreeNodes.pop_front();
	return Node;
}

template<typename T>
inline void MemoryPool<T>::Dallocate(T* _Obj)
{
	// ���⼭ _Obj�� ���¸� ����ڰ� ���ʱ�ȭ�ϰų� ��Ȱ��ȭ�ϴ� �� ���� �������� �� ��
	// Ǯ�� �׳� �ݳ��� �޴´�.
	m_listFreeNodes.push_back(_Obj);
}
