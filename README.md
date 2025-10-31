# MemoryPoolProject
이 프로젝트는 C++에서의 메모리 관리 효율화를 위해
커스텀 메모리 풀(Memory Pool)을 직접 구현해보는 실습용 프로젝트입니다.
new/delete의 오버헤드를 줄이고, 객체 생성/소멸 과정을 직접 제어하기 위한 목적으로 진행 중입니다.

# 학습 포인트
Placement new / explicit destructor 호출
메모리 재사용 전략 및 단편화 최소화
STL 컨테이너와의 비교 (new/delete vs MemoryPool)
객체 수명 주기 수동 제어

# 개발 환경
Language : C++17
Compiler : MSVC (Visual Studio 2022)
기간 : 2025/10/29 ~ 
