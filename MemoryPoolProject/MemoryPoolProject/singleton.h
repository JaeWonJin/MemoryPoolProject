#pragma once
template<typename T>
class singleton
{
private:
	static T* g_Inst;

public:
	static T* GetInst()
	{
		if (nullptr == g_Inst) g_Inst = new T;
		return g_Inst;
	}

	static void Destroy()
	{
		if (nullptr != g_Inst)
		{
			delete g_Inst;
			g_Inst = nullptr;
		}
	}

protected:
	singleton()
	{
		typedef void(*FUNC)(void);
		// 윈도우 종료시 알아서 Destory 호출
		atexit((FUNC)&singleton::Destroy);
	}
	~singleton() {}
};

template<typename T>
T* singleton<T>::g_Inst = nullptr;