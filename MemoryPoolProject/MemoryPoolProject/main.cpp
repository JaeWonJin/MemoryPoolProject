#include "pch.h"

#include "MemoryPool.h"

struct Bullet {
	float x, y, z;          // 위치
	float vx, vy, vz;       // 속도
	int damage;             // 데미지
	int ownerID;            // 소유주
	char padding[32];       // 일부러 크기를 조금 키워 캐시 라인을 채움 (선택)

	Bullet() : x(0), y(0), z(0), vx(1), vy(1), vz(1), damage(10), ownerID(0) {}
	~Bullet() {} // 소멸자가 하는 일이 없어도 호출 오버헤드는 존재함
};

int main()
{
    const int OBJECT_COUNT = 100000;
    const int ITERATIONS = 10; // 반복 횟수

    vector<Bullet*> storage;
    storage.reserve(OBJECT_COUNT);

    // --- 표준 new/delete 테스트 ---
    {
        auto start = chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            for (int j = 0; j < OBJECT_COUNT; ++j) storage.push_back(new Bullet());

            // 해제 순서를 섞어 힙 매니저를 괴롭힘
            //std::shuffle(storage.begin(), storage.end(), std::mt19937{ std::random_device{}() });

            for (auto* p : storage) delete p;
            storage.clear();
        }
        auto end = chrono::high_resolution_clock::now();
        cout << "Standard new/delete: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;
    }

    // --- 메모리 풀 테스트 ---
    {
        MemoryPool<Bullet>::GetInst()->Init(OBJECT_COUNT);
        auto start = chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            for (int j = 0; j < OBJECT_COUNT; ++j) storage.push_back(MemoryPool<Bullet>::GetInst()->Allocate());

            //std::shuffle(storage.begin(), storage.end(), std::mt19937{ std::random_device{}() });

            for (auto* p : storage) MemoryPool<Bullet>::GetInst()->Deallocate(p);
            storage.clear();
        }
        auto end = chrono::high_resolution_clock::now();
        cout << "Custom Memory Pool: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << "ms" << endl;
    }

	return 0;
}