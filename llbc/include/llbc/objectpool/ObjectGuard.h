#ifndef __LLBC_CORE_OBJECT_POOL_OBJECT_GUARD_H__
#define __LLBC_CORE_OBJECT_POOL_OBJECT_GUARD_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

class LLBC_IObjectPool;

/**
* \brief 对象守护器
*/
template <typename ObjectType>
class LLBC_ObjectGuard
{
public:
    LLBC_ObjectGuard(ObjectType* obj);
    LLBC_ObjectGuard(const LLBC_ObjectGuard<ObjectType> &rhs);
    ~LLBC_ObjectGuard();

public:
    ObjectType *operator ->();
    ObjectType* GetValue() const;

public:
    LLBC_ObjectGuard &operator =(const LLBC_ObjectGuard &another);

private:
    LLBC_IObjectPool* GetPool() const;
    LLBC_ObjectGuard<ObjectType>& SetPool(LLBC_IObjectPool *pool);

    void MakeWeakRef();

private:
    ObjectType * _obj;
    LLBC_IObjectPool* _pool;

    bool _weakRef;
};

__LLBC_NS_END

#include "llbc/core/objectpool/ObjectGuardImpl.h"

#endif // !__LLBC_CORE_OBJECT_POOL_OBJECT_GUARD_H__
