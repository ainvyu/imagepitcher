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

// 범위 검사 매크로
#define IS_WITHIN(min,max,expr)         (((min) <= (expr)) && ((max) >= (expr)))
#define IS_WITHOUT(min,max,expr)        (((min) > (expr)) || ((max) < (expr)))

// FOUND 매크로
#define IS_FOUND(i,c)           (i != (c).end())
#define IS_NOT_FOUND(i,c)       (i == (c).end())
#define IS_FOUND_PTR(i,p)       (i != (p)->end())
#define IS_NOT_FOUND_PTR(i,p)   (i == (p)->end())

// FOREACH 매크로
#define FOREACH_MAP(i,c)        for( i = (c).begin(); i != (c).end(); ++i )
#define FOREACH_MAP_PTR(i,c)    for( i = (c)->begin(); i != (c)->end(); ++i )

#define FOREACH_VECTOR(i,c)     for( i = 0; i < (c).size(); ++i )
#define FOREACH_VECTOR_PTR(i,c) for( i = 0; i < (c)->size(); ++i )

// CLEANUP 매크로
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
Breakpoint 매크로
Visual Studio 에서 강제로 Breakpoint 를 설정해야 할 경우 유용하게 사용할 수 있다. 
Win32 API 를 사용할 경우에는 DebugBreak() 함수를 사용할 수도 있지만 VC++ 만 사용한다면 
intrin.h 헤더 파일을 포함한 후 __debugbreak() 함수를 사용하면 된다. 
하지만 지원 안되는 버전의 VC++ (Visual Studio 2003 이하) 일 경우 다음과 같이 간단히 선언 후 사용할 수 있다.
*/
#if (_MSC_VER >= 1400)
// Microsoft Visual C++ .NET 2005 이상
#include <intrin.h>
#else
// Microsoft Visual C++ .NET 2003 이하
#define __debugbreak()                  {__asm int 3}
#endif

// 컴파일러 TODO 매크로
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
TODO 매크로 사용 예

// main.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의
//
// main() 함수 구현
int main()
{
#pragma TODO( TODO 메시지 1...! )
#pragma FIXME( FIXME 메시지 2...! )

#pragma FILE_LINE
#pragma NOTE( "NOTE 메시지 3...!\n\
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
대부분 클래스들은 복사될 필요가 없고 복사 생성자나 대입연사자가 필요없다.
그러나 운나쁘게 컴파일러는 복사 생성자나 대입 연산자를 정의, 선언 하지 않아도 컴파일러가 대신 복사생성자와 대입 연산자를 자동적으로 public 영역에 정의, 선언해준다.
복사생성자와 대입연산자가 필요 없는경우 확실히 금해야한다. 이런경우 private섹션에 정의만 하고 구현을 하지 않으면 해결된다.
그럼 클래스간 복사와 대입을 못하게 하며 만약 그럴경우 컴파 일러에러가 발생 한다.
우리는 private섹션에 이런 일을 해주는 매크로를 준비했다.
*/

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
	TypeName(const TypeName&);             \
	void operator=(const TypeName&)
/*
예로 class Foo:  에서는

class Foo {
public:
	Foo(int f);
	~Foo();
private:
	DISALLOW_COPY_AND_ASSIGN(Foo);
};

처럼 하면 된다.

거의 모든경우에 여러분이 사용할 클래스에는 위에 설명한 DISALLOW_COPY_AND_ASSIGN 를 사용하게 될것이다.  
만약 복사, 대입이 필요한 클래스가 인경우 그 클래스의 헤더 파일안에 왜 복사와 대입이 필요한지에 대해 문서화 해야 한다. 그리고 스스로 복사 대입연산자와 복사 생성자를 적절하게 정의해야한다.
operator=. (복사대입연산자)에서 자기대입에 대한 경우도 주의해서 구현해야만한다.
당신은 객체를 STL에 값으로 사용 하기 위해 복사 가능한 클래스가 필요할 수있다.
그러한 경우 당신은 STL컨테이너 안의 데이터타입을 포인터로 선언하는것이 훨씬더 좋다.
이런경우 예외및 안정성을 위해 std::tr1::shared_ptr을 사용하면 더욱 좋을것이다. 
*/

#define MAKEWORD(a, b)      ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#define MAKEDWORD(a, b)     MAKELONG((a), (b))
#define LOWORD(l)           ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#define HIWORD(l)           ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#define LOBYTE(w)           ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#define HIBYTE(w)           ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))