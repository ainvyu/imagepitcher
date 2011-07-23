#pragma once

// #ifndef SAFE_DELETE
// #define SAFE_DELETE(ptr) { if (ptr) { delete (ptr); (ptr) = NULL; } }
// #endif
// 
// #ifndef SAFE_DELETE_ARRAY
// #define SAFE_DELETE_ARRAY(ptr) { if (ptr) { delete [] (ptr); (ptr) = NULL; } }
// #endif
// 
// #ifndef SAFE_FREE 
// #define SAFE_FREE(ptr) { if (ptr) { free (ptr); (ptr) = NULL; } } 
// #endif // SAFE_FREE
// 
// #ifndef SAFE_RELEASE 
// #define SAFE_RELEASE(comPointer)  { if (comPointer) { (comPointer)->Release(); (comPointer)=NULL; } } 
// #endif // SAFE_RELEASE
// 
// #ifndef SAFE_CALLPTR 
// #define SAFE_CALLPTR(classPointer, functionName)  { if (classPointer) (classPointer)->functionName } 
// #endif // SAFE_CALLPTR
// 
// #ifndef SAFE_DELETE_HICON 
// #define SAFE_DELETE_HICON(hIcon)  { if (hIcon) {::DestroyIcon(hIcon); (hIcon) = NULL;} } 
// #endif // SAFE_DELETE_HICON
// 
// #ifndef SAFE_DELETE_OBJ 
// #define SAFE_DELETE_OBJ(obj)  { if ((obj).GetSafeHandle()) { (obj).DeleteObject(); } } 
// #endif // SAFE_DELETE_OBJ

template <typename T> inline void SAFE_DELETE(T& p)						{ delete p; (p) = NULL; } 
template <typename T> inline void SAFE_DELETE_ARRAY(T& p)				{ delete[] p; (p) = NULL; } 
template <typename T> inline void SAFE_DELETE_ARRAY_OBJ(T& p, int n)	{ if (p) { for (int i = 0; i < n; ++i) { SAFE_DELETE((p)[i]); } delete[] (p); p = NULL; } } 
template <typename T> inline void SAFE_DELETE_ARRAY_ARRAY(T& p, int n)	{ if (p) { for (int i = 0; i < n; ++i) { SAFE_DELETE_ARRAY((p)[i]); } delete[] (p); p = NULL; } }

// ���� �˻� ��ũ��
#define IS_WITHIN(min,max,expr)         (((min) <= (expr)) && ((max) >= (expr)))
#define IS_WITHOUT(min,max,expr)        (((min) > (expr)) || ((max) < (expr)))

// FOUND ��ũ��
#define IS_FOUND(i,c)           (i != (c).end())
#define IS_NOT_FOUND(i,c)       (i == (c).end())
#define IS_FOUND_PTR(i,p)       (i != (p)->end())
#define IS_NOT_FOUND_PTR(i,p)   (i == (p)->end())

// FOREACH ��ũ��
#define FOREACH_MAP(i,c)        for( i = (c).begin(); i != (c).end(); ++i )
#define FOREACH_MAP_PTR(i,c)    for( i = (c)->begin(); i != (c)->end(); ++i )

#define FOREACH_VECTOR(i,c)     for( i = 0; i < (c).size(); ++i )
#define FOREACH_VECTOR_PTR(i,c) for( i = 0; i < (c)->size(); ++i )

// CLEANUP ��ũ��
#define CLEANUP_MAP(i,c)        for( i = (c).begin(); i != (c).end(); ++i ) \
	delete (*i).second; \
	(c).clear();
#define CLEANUP_MAP_PTR(i,c)    for( i = (c)->begin(); i != (c)->end(); ++i ) \
	delete (*i).second; \
	(c)->clear();

#define CLEANUP_VECTOR(i,c)     for( i = 0; i < (c).size(); ++i ) \
	delete c[i]; \
	(c).clear();
#define CLEANUP_VECTOR_PTR(i,c) for( i = 0; i < (c)->size(); ++i ) \
	delete c->at(i); \
	(c)->clear();

/*
Breakpoint ��ũ��
Visual Studio ���� ������ Breakpoint �� �����ؾ� �� ��� �����ϰ� ����� �� �ִ�. 
Win32 API �� ����� ��쿡�� DebugBreak() �Լ��� ����� ���� ������ VC++ �� ����Ѵٸ� 
intrin.h ��� ������ ������ �� __debugbreak() �Լ��� ����ϸ� �ȴ�. 
������ ���� �ȵǴ� ������ VC++ (Visual Studio 2003 ����) �� ��� ������ ���� ������ ���� �� ����� �� �ִ�.
*/
#if (_MSC_VER >= 1400)
// Microsoft Visual C++ .NET 2005 �̻�
#include <intrin.h>
#else
// Microsoft Visual C++ .NET 2003 ����
#define __debugbreak()                  {__asm int 3}
#endif

// �����Ϸ� TODO ��ũ��
//------------------------------------------------------------------------------
// TODO / FIXME / NOTE macros
//------------------------------------------------------------------------------

#define _QUOTE(x)               # x
#define QUOTE(x)                _QUOTE(x)
#define __FILE__LINE__          __FILE__ "(" QUOTE(__LINE__) "): "

#define NOTE( x )               message( x )
#define FILE_LINE               message( __FILE__LINE__ )

#define TODO( x )               message( __FILE__LINE__"[TODO]: " #x "\n" )
#define FIXME( x )              message( __FILE__LINE__"[FIXME]: " #x "\n" )

/*
TODO ��ũ�� ��� ��

// main.cpp : �ܼ� ���� ���α׷��� ���� �������� ����
//
// main() �Լ� ����
int main()
{
#pragma TODO( TODO �޽��� 1...! )
#pragma FIXME( FIXME �޽��� 2...! )

#pragma FILE_LINE
#pragma NOTE( "NOTE �޽��� 3...!\n\
------------------------------------------------------------")

	return 0;
}
*/

#define BX_(x)		((x) - (((x) >> 1) & 0x77777777)  \
					- (((x) >> 2) & 0x33333333)       \
					- (((x) >> 3) & 0x11111111))
#define BITCOUNT(x)     (((BX_(x)+(BX_(x)>>4)) & 0x0F0F0F0F) % 255)

// Radian to Degree
#define RAD2DEG(x) (x * 180.0 / 3.14159265358979323846) 
// Degree to Radian
#define DEG2RAD(x) (x * 3.14159265358979323846 / 180.0)

/*
��κ� Ŭ�������� ����� �ʿ䰡 ���� ���� �����ڳ� ���Կ����ڰ� �ʿ����.
�׷��� ��ڰ� �����Ϸ��� ���� �����ڳ� ���� �����ڸ� ����, ���� ���� �ʾƵ� �����Ϸ��� ��� ��������ڿ� ���� �����ڸ� �ڵ������� public ������ ����, �������ش�.
��������ڿ� ���Կ����ڰ� �ʿ� ���°�� Ȯ���� ���ؾ��Ѵ�. �̷���� private���ǿ� ���Ǹ� �ϰ� ������ ���� ������ �ذ�ȴ�.
�׷� Ŭ������ ����� ������ ���ϰ� �ϸ� ���� �׷���� ���� �Ϸ������� �߻� �Ѵ�.
�츮�� private���ǿ� �̷� ���� ���ִ� ��ũ�θ� �غ��ߴ�.
*/

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);             \
	void operator=(const TypeName&)
/*
���� class Foo:  ������

class Foo {
public:
	Foo(int f);
	~Foo();
private:
	DISALLOW_COPY_AND_ASSIGN(Foo);
};

ó�� �ϸ� �ȴ�.

���� ����쿡 �������� ����� Ŭ�������� ���� ������ DISALLOW_COPY_AND_ASSIGN �� ����ϰ� �ɰ��̴�.  
���� ����, ������ �ʿ��� Ŭ������ �ΰ�� �� Ŭ������ ��� ���Ͼȿ� �� ����� ������ �ʿ������� ���� ����ȭ �ؾ� �Ѵ�. �׸��� ������ ���� ���Կ����ڿ� ���� �����ڸ� �����ϰ� �����ؾ��Ѵ�.
operator=. (������Կ�����)���� �ڱ���Կ� ���� ��쵵 �����ؼ� �����ؾ߸��Ѵ�.
����� ��ü�� STL�� ������ ��� �ϱ� ���� ���� ������ Ŭ������ �ʿ��� ���ִ�.
�׷��� ��� ����� STL�����̳� ���� ������Ÿ���� �����ͷ� �����ϴ°��� �ξ��� ����.
�̷���� ���ܹ� �������� ���� std::tr1::shared_ptr�� ����ϸ� ���� �������̴�. 
*/

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#define MAKEDWORD(a, b)     MAKELONG((a), (b))
#define LOWORD(l)           ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l)           ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define LOBYTE(w)           ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w)           ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))