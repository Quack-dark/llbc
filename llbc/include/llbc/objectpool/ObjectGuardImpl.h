#ifdef __LLBC_CORE_OBJECT_POOL_OBJECT_GUARD_H__


__LLBC_NS_BEGIN

template <typename ObjectType>
inline LLBC_ObjectGuard<ObjectType>::LLBC_ObjectGuard(ObjectType* obj)
: _obj(obj)
, _pool(NULL)

, _weakRef(false)
{
}

template <typename ObjectType>
inline LLBC_ObjectGuard<ObjectType>::LLBC_ObjectGuard(const LLBC_ObjectGuard<ObjectType> &rhs)
{
    _obj = rhs.GetValue();
    _pool = rhs.GetPool();

    const_cast<LLBC_ObjectGuard<ObjectType> &>(rhs).MakeWeakRef();
}

template <typename ObjectType>
inline LLBC_ObjectGuard<ObjectType>::~LLBC_ObjectGuard()
{
    if (_weakRef)
        return;

    if (_pool)
        _pool->Back(LLBC_GetTypeName(ObjectType), _obj);
}

template <typename ObjectType>
inline ObjectType *LLBC_ObjectGuard<ObjectType>::operator ->()
{
    return _obj;
}

template <typename ObjectType>
inline ObjectType *LLBC_ObjectGuard<ObjectType>::GetValue() const
{
    return _obj;
}

template <typename ObjectType>
inline LLBC_IObjectPool* LLBC_ObjectGuard<ObjectType>::GetPool() const
{
    return _pool;
}

template <typename ObjectType>
inline LLBC_ObjectGuard<ObjectType>& LLBC_ObjectGuard<ObjectType>::SetPool(LLBC_IObjectPool* pool)
{
    _pool = pool; 
    return *this;
}

template <typename ObjectType>
inline void LLBC_ObjectGuard<ObjectType>::MakeWeakRef()
{
    _weakRef = true;
}

__LLBC_NS_END

#endif // !__LLBC_CORE_OBJECT_POOL_OBJECT_GUARD_H__
