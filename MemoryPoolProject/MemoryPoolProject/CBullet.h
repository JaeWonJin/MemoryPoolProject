#pragma once

struct Vec2
{
	float x;
	float y;

public:
	float Length() 
	{
		return sqrt(x * x + y * y);
	}
	void Normalize() 
	{
		float Len = Length();
		assert(Len);
		x /= Len;
		y /= Len;
	}
	float Dot(Vec2 _Other) 
	{
		return x * _Other.x + y * _Other.y;
	}

public:
	Vec2 operator + (Vec2 _Other) { return Vec2(x + _Other.x, y + _Other.y); }
	Vec2 operator + (float _f) { return Vec2(x + _f, y + _f); }
	void operator += (Vec2 _Other) { x += _Other.x; y += _Other.y; }
	void operator += (float _f) { x += _f; y += _f; }

	Vec2 operator - (Vec2 _Other) { return Vec2(x - _Other.x, y - _Other.y); }
	Vec2 operator - (float _f) { return Vec2(x - _f, y - _f); }
	void operator -= (Vec2 _Other) { x -= _Other.x; y -= _Other.y; }
	void operator -= (float _f) { x -= _f; y -= _f; }

	Vec2 operator * (Vec2 _Other) { return Vec2(x * _Other.x, y * _Other.y); }
	Vec2 operator * (float _f) { return Vec2(x * _f, y * _f); }
	void operator *= (Vec2 _Other) { x *= _Other.x; y *= _Other.y; }
	void operator *= (float _f) { x *= _f; y *= _f; }

	Vec2 operator / (Vec2 _Other) { assert(_Other.x && _Other.y); return Vec2(x / _Other.x, y / _Other.y); }
	Vec2 operator / (float _f) { assert(_f); return Vec2(x / _f, y / _f); }
	void operator /= (Vec2 _Other) { assert(_Other.x && _Other.y); x /= _Other.x; y /= _Other.y; }
	void operator /= (float _f) { assert(_f); x /= _f; y /= _f; }

	Vec2() : x(0.f), y(0.f) {}
	Vec2(float _x, float _y) : x(_x), y(_y) {}
	~Vec2() {}
};


class CBullet
{
private:
	Vec2	m_Pos;
	Vec2	m_TargetPos;
	Vec2	m_Scale;
	float	m_Velocity;
	float	m_Acceleration;
	float	m_Direction;
	float	m_TurnRadius;

	bool	m_IsMemoryPool;

public:
	void Tick();

public:
	CBullet();
	~CBullet();
};

