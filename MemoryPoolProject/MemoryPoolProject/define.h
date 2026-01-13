#pragma once
#define SINGLE(Type) friend class singleton<Type>;\
					Type& operator = (const Type& _Other) = delete;\
					Type(const Type& _Origin) = delete;\
					Type();\
					~Type();