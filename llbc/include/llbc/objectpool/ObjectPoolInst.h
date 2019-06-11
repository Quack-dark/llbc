#ifndef __LLBC_CORE_OBJECT_POOL_INSTANCE_H__
#define __LLBC_CORE_OBJECT_POOL_INSTANCE_H__

#include "llbc/common/Common.h"
#include "llbc/core/objectpool/ObjectGuard.h"
#include "llbc/core/objectpool/ObjectManipulator.h"

#if LLBC_TARGET_PLATFORM_WIN32
 #pragma warning(push)
 #pragma warning(disable:4200)
#endif // LLBC_TARGET_PLATFORM_WIN32

__LLBC_INTERNAL_NS_BEGIN
#if LLBC_TARGET_PLATFORM_WIN32
    const LLBC_NS sint64 Zero = 0I64;
    const LLBC_NS sint64 One = 1I64;
#else
    const LLBC_NS sint64 Zero = 0LL;
    const LLBC_NS sint64 One = 1LL;
#endif

#if LLBC_DEBUG
 #define CheckSymbolSize 8
 #if LLBC_TARGET_PLATFORM_WIN32
    const sint64 BeginingSymbol = 0xcdcdcdcdcdcdcdcdI64;
    const sint64 EndingSymbol = 0xcdcdcdcdcdcdcdcdI64;
 #else // Non-Win32
    const sint64 BeginingSymbol = 0xcdcdcdcdcdcdcdcdLL;
    const sint64 EndingSymbol = 0xcdcdcdcdcdcdcdcdLL;
 #endif // LLBC_TARGET_PLATFORM_WIN32
#else // NDEBUG
 #define  CheckSymbolSize 0
#endif // LLBC_DEBUG

__LLBC_INTERNAL_NS_END

__LLBC_NS_BEGIN

/**
* \brief 对象池实例抽象类
*/

class LLBC_IObjectPoolInst
{
public:
    virtual void* Get() = 0;
    virtual void Clear(void *obj) = 0;
};

template <typename ObjectType, typename LockType = LLBC_DummyLock>
class LLBC_ObjectPoolInst : public LLBC_IObjectPoolInst
{
private:
    // 内存单元数据结构
    struct MemoryUnit
    {
        bool inited;            // 是否已构造 
        short seq;              // 编号
        char buff[0];
    };

    // 内存位示图数据结构
    struct MemoryBitView
    {
        short idelCount;        // 空闲内存块数量
        char bits[0];
    };

    //  内存块数据结构
    struct MemoryBlock
    {
        short seq;            // 编号
        char buff[0];         // 
    };

public:
    LLBC_ObjectPoolInst();
    ~LLBC_ObjectPoolInst();

public:
    virtual void* Get();
    virtual void Clear(void* obj);
    LLBC_ObjectGuard<ObjectType> Guarder();

protected:
    void _AllocateMemoryBlock();
    short _DetectIdelMemoryUit(sint64* bitView);
    void* _GetIdelObj(MemoryBlock* memBlock);
    void _MaskBitState(MemoryBitView* bitView, short index, bool used);

private:
    const int _elementSize;
    const short _elementCount;

    int _blockNum;
    MemoryBlock** _arrBlock;
    MemoryBitView** _arrBitView;

    LockType _lock;
};

__LLBC_NS_END

#include "llbc/core/objectpool/ObjectPoolInstImpl.h"

#if LLBC_TARGET_PLATFORM_WIN32
#pragma warning(pop)
#endif // LLBC_TARGET_PLATFORM_WIN32

#endif // !__LLBC_CORE_OBJECT_POOL_INSTANCE_H__