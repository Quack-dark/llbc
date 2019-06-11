#ifndef __LLBC_CORE_OBJECT_POOL_H__
#define __LLBC_CORE_OBJECT_POOL_H__

#include "llbc/common/Common.h"
#include "llbc/core/objectpool/IObjectPool.h"
#include "llbc/core/objectpool/ObjectPoolInst.h"


__LLBC_NS_BEGIN
template <typename PoolLockType = LLBC_DummyLock, typename PoolInstLockType = PoolLockType>
class LLBC_ObjectPool : public LLBC_IObjectPool
{
public:
    LLBC_ObjectPool();
    ~LLBC_ObjectPool();

public:
    template <typename ObjectType>
    ObjectType* Get();

    template <typename ObjectType>
    void Back(ObjectType* obj);

    template <typename ObjectType>
    LLBC_ObjectGuard<ObjectType> Guarder();

protected:
    virtual int Back(const char* objectType, void* obj);

private:
    PoolLockType _lock;
    std::map<const char*, LLBC_IObjectPoolInst*> _poolDict;
};

__LLBC_NS_END

#include "llbc/core/objectpool/ObjectPoolImpl.h"


#endif // !__LLBC_CORE_OBJECT_POOL_H__