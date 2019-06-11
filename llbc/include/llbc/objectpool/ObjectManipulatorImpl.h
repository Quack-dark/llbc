#ifdef __LLBC_CORE_OBJECT_MANIPULATOR_H__

__LLBC_NS_BEGIN

template <typename ObjectType>
inline void LLBC_ObjectManipulator::Create(void *obj)
{
    new (obj) ObjectType();
}

template <typename ObjectType>
inline void LLBC_ObjectManipulator::Destroy(void *obj)
{
    (reinterpret_cast<ObjectType*>(obj))->~ObjectType();
}

template <typename ObjectType>
inline bool LLBC_ObjectManipulator::Clear(void *obj)
{
    return ClearObj<ObjectType>(obj, 0);
}

template <typename ObjectType>
inline bool LLBC_ObjectManipulator::ClearObj(void* obj, serializable_type<ObjectType, &ObjectType::clear> *)
{
    reinterpret_cast<ObjectType *>(obj)->clear();
    return false;
}

template <typename ObjectType>
inline bool LLBC_ObjectManipulator::ClearObj(void* obj, ...)
{
    Destroy<ObjectType>(obj);
    return true;
}

__LLBC_NS_END

#endif // __LLBC_CORE_OBJECT_MANIPULATOR_H__