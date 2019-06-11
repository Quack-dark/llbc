#ifdef __LLBC_CORE_OBJECT_POOL_H__


__LLBC_NS_BEGIN

template <typename PoolLockType, typename PoolInstLockType>
LLBC_ObjectPool<PoolLockType, PoolInstLockType>::LLBC_ObjectPool()
{
}

template <typename PoolLockType, typename PoolInstLockType>
LLBC_ObjectPool<PoolLockType, PoolInstLockType>::~LLBC_ObjectPool()
{
    LLBC_LockGuard guard(_lock);
    LLBC_STLHelper::DeleteContainer(_poolDict, false, false);
}

template <typename PoolLockType, typename PoolInstLockType>
template <typename ObjectType>
ObjectType* LLBC_ObjectPool<PoolLockType, PoolInstLockType>::Get()
{
    const char* key = typeid(ObjectType).name();

    LLBC_IObjectPoolInst *poolInst;
    std::map<const char*, LLBC_IObjectPoolInst *>::iterator it;

    _lock.Lock();
    if ((it = _poolDict.find(key)) == _poolDict.end())
        poolInst = _poolDict.insert(std::make_pair(key, new LLBC_ObjectPoolInst<ObjectType, PoolInstLockType>())).first.second;
    else
        poolInst = it->second;

    _lock.Unlock();

    return reinterpret_cast<ObjectType *>(poolInst->Get());
}

template <typename PoolLockType, typename PoolInstLockType>
template <typename ObjectType>
void LLBC_ObjectPool<PoolLockType, PoolInstLockType>::Back(ObjectType* obj)
{
    this->Back(typeid(ObjectType).name(), obj);
}

template <typename PoolLockType, typename PoolInstLockType>
template <typename ObjectType>
LLBC_ObjectGuard<ObjectType> LLBC_ObjectPool<PoolLockType, PoolInstLockType>::Guarder()
{
    LLBC_ObjectGuard<ObjectGuard> guard(Get<>());
    guard.SetPool(this);

    return guard;

    // ==========================================================

    const char* key = typeid(ObjectType).name();
    std::map<const char*, LLBC_IObjectPoolInst *>::iterator it;

    _lock.Lock();
    if ((it = _poolDict.find(key)) == _poolDict.end())
    {
        poolInst = new LLBC_ObjectPoolInst<ObjectType, PoolInstLockType>();
        it = _poolDict.insert(std::make_pair(key, poolInst)).first;
    }
    _lock.Unlock();

    return it->second->Guarder().SetPool(this);
}

template <typename PoolLockType, typename PoolInstLockType>
int LLBC_ObjectPool<PoolLockType, PoolInstLockType>::Back(const char* objectType, void* obj)
{
    LLBC_IObjectPoolInst *poolInst;
    std::map<const char*, LLBC_IObjectPoolInst *>::iterator it;

    _lock.Lock();
    if ((it = _poolDict.find(objectType)) == _poolDict.end())
    {
        _lock.Unlock();

        LLBC_SetLastError(LLBC_ERROR_UNKNOWN);
        return LLBC_FAILED;
    }

    poolInst = it->second;

    _lock.Unlock();

    poolInst->Clear(obj);

    return LLBC_OK;
}

__LLBC_NS_END

#endif //!__LLBC_CORE_OBJECT_POOL_H__