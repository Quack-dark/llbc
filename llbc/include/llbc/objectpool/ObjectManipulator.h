#ifndef __LLBC_CORE_OBJECT_MANIPULATOR_H__
#define __LLBC_CORE_OBJECT_MANIPULATOR_H__

#include "llbc/common/Common.h"

__LLBC_NS_BEGIN

/**
* \brief 对象操作器
*/
class LLBC_ObjectManipulator
{
public:
    template <typename ObjectType>
    static void Create(void *obj);

    template <typename ObjectType>
    static void Destroy(void *obj);

    template <typename ObjectType>
    static bool Clear(void *obj);

private:
    template <typename ObjectType, void (ObjectType::*)()>
    struct serializable_type;
    template <typename ObjectType>
    static bool ClearObj(void* obj, serializable_type<ObjectType, &ObjectType::clear> *);

    template <typename ObjectType>
    static bool ClearObj(void* obj, ...);
};

__LLBC_NS_END

#include "llbc/core/objectpool/ObjectManipulatorImpl.h"

#endif // !__LLBC_CORE_OBJECT_MANIPULATOR_H__