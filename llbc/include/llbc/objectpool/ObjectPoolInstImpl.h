#ifdef __LLBC_CORE_OBJECT_POOL_INSTANCE_H__


__LLBC_NS_BEGIN

template <typename ObjectType, typename LockType>
inline LLBC_ObjectPoolInst<ObjectType, LockType>::LLBC_ObjectPoolInst()
: _elementSize((short)(sizeof(MemoryBlock) + sizeof(ObjectType) + 2 * CheckSymbolSize))
, _elementCount(LLBC_CFG_OBJECT_POOL_MEMORY_BLOCK_SIZE / _elementSize)
{
    _blockNum = 0;
    _arrBlock = NULL;
    _arrBitView = NULL;
}

template <typename ObjectType, typename LockType>
inline LLBC_ObjectPoolInst<ObjectType, LockType>::~LLBC_ObjectPoolInst()
{
    if (_arrBlock)
        delete[] _arrBlock;

    if (_arrBitView)
        delete[] _arrBitView;
}

template <typename ObjectType, typename LockType>
inline void* LLBC_ObjectPoolInst<ObjectType, LockType>::Get()
{
    _lock.Lock();

    int oldNum = _blockNum;
    if (oldNum == 0)
    {
        _AllocateMemoryBlock();
        void* obj = _GetIdelObj(_arrBlock[oldNum]);
        _lock.Unlock();
        return obj;
    }
    else
    {
        void* obj = NULL;
        for (int i = 0; i < _blockNum; ++i)
        {
            obj = _GetIdelObj(_arrBlock[i]);
            if (obj)
            {
                _lock.Unlock();
                return obj;
            }
        }

        _AllocateMemoryBlock();
        obj = _GetIdelObj(_arrBlock[oldNum]);
        _lock.Unlock();
        return obj;
    }
}

template <typename ObjectType, typename LockType>
inline void LLBC_ObjectPoolInst<ObjectType, LockType>::Clear(void* obj)
{
    _lock.Lock();

    bool destoryed = LLBC_ObjectManipulator::Clear<ObjectType>(obj);
    auto *memUnit = reinterpret_cast<MemoryUnit *>((unsigned char *)(obj) - CheckSymbolSize - sizeof(MemoryBlock));
    if (destoryed)
        memUnit->inited = false;

    auto *memBlock = reinterpret_cast<MemoryBlock *>(reinterpret_cast<unsigned char *>(memUnit) - memUnit->seq * _elementSize - sizeof(MemoryBlock));
    _MaskBitState(_arrBitView[memBlock->seq], memUnit->seq, false);

    _lock.Unlock();
}

template <typename ObjectType, typename LockType>
inline LLBC_ObjectGuard<ObjectType> LLBC_ObjectPoolInst<ObjectType, LockType>::Guarder()
{
    return LLBC_ObjectGuard<ObjectType>(static_cast<ObjectType*>(this->Get()));
}

template <typename ObjectType, typename LockType>
inline void LLBC_ObjectPoolInst<ObjectType, LockType>::_AllocateMemoryBlock()
{
    if (_blockNum == 0)
    {
        _arrBlock = (MemoryBlock**)::malloc(sizeof(MemoryBlock*));
        _arrBitView = (MemoryBitView**)::malloc(sizeof(MemoryBitView*));
    }
    else
    {
        _arrBlock = (MemoryBlock**)::realloc(_arrBlock, sizeof(MemoryBlock*)*(_blockNum + 1));
        _arrBitView = (MemoryBitView**)::realloc(_arrBitView, sizeof(MemoryBitView*)*(_blockNum + 1));
    }

    auto* memBlock = (MemoryBlock*)::malloc(sizeof(MemoryBlock) + _elementCount * _elementSize);
#if defined(_DEBUG)
    ::memset(memBlock->buff, 0, _elementCount * _elementSize);
#endif
    memBlock->seq = _blockNum;
    for (short idx = 0; idx < _elementCount; ++idx)
    {
        auto* memUnit = reinterpret_cast<MemoryUnit*>((unsigned char*)memBlock->buff + idx * _elementSize);
        memUnit->inited = false;
        memUnit->seq = idx;
#if defined(_DEBUG) 
        *(reinterpret_cast<sint64*>(memUnit->buff)) = BeginingSymbol;
        *(reinterpret_cast<sint64*>((unsigned char*)memUnit + _elementSize - CheckSymbolSize)) = EndingSymbol;
#endif
    }
    _arrBlock[_blockNum] = memBlock;

    auto* bitView = (MemoryBitView*)::malloc(sizeof(MemoryBitView) + _elementCount);
#if defined(_DEBUG)
    ::memset(bitView->bits, 0, _elementCount);
#endif
    bitView->idelCount = _elementCount;
    _arrBitView[_blockNum] = bitView;

    ++_blockNum;
}

template <typename ObjectType, typename LockType>
short LLBC_ObjectPoolInst<ObjectType, LockType>::_DetectIdelMemoryUit(sint64* bitView)
{
    auto* ptrBits = reinterpret_cast<int *>(bitView);
    if (*ptrBits ^ 0xFFFFFFFF)
    {
        for (short idx = 0; idx < 32; ++idx)
        {
            if (*ptrBits & 1 << idx)
                continue;

            return idx;
        }
    }
    else
    {
        ptrBits++;
        for (short idx = 0; idx < 32; ++idx)
        {
            if (*ptrBits & 1 << idx)
                continue;

            return idx + 32;
        }
    }

    return 0;
}

template <typename ObjectType, typename LockType>
void* LLBC_ObjectPoolInst<ObjectType, LockType>::_GetIdelObj(MemoryBlock* memBlock)
{
    auto* bitView = _arrBitView[memBlock->seq];
    if (bitView->idelCount == 0)
        return NULL;

    auto* ptrBits = reinterpret_cast<sint64 *>(bitView->bits);
    short index = 0;
    while (index <= _elementCount)
    {
        if (*ptrBits ^ ~LLBC_INL_NS Zero)
        {
            index += _DetectIdelMemoryUit(ptrBits);
            _MaskBitState(bitView, index, true);
            auto* memUnit = reinterpret_cast<MemoryUnit*>((unsigned char*)memBlock->buff + index * _elementSize);

#if defined(_DEBUG) 
            ASSERT(*(reinterpret_cast<sint64*>(memUnit->buff)) == BeginingSymbol && "LLBC_ObjectPoolInst::Get() memory unit is dirty");
            ASSERT(*(reinterpret_cast<sint64*>((unsigned char*)memUnit + _elementSize - CheckSymbolSize)) == EndingSymbol && "LLBC_ObjectPoolInst::Get() memory unit is dirty");
#endif
            void* obj = memUnit->buff + CheckSymbolSize;
            if (!memUnit->inited)
            {
                LLBC_ObjectManipulator::Create<ObjectType>(obj);
                memUnit->inited = true;
            }

            return obj;
        }

        index += 64;
        ptrBits++;
    }

    return NULL;
}

template <typename ObjectType, typename LockType>
void LLBC_ObjectPoolInst<ObjectType, LockType>::_MaskBitState(MemoryBitView* bitView, short index, bool used)
{
    if (used)
    {
        --bitView->idelCount;
        *(reinterpret_cast<sint64*>(bitView->bits) + (index >> 6)) |= One << (index & 0x3f);
    }
    else
    {
        ++bitView->idelCount;
        *(reinterpret_cast<sint64*>(bitView->bits) + (index >> 6)) &= ~(One << (index & 0x3f));
    }
}

__LLBC_NS_END

#endif //__LLBC_CORE_OBJECT_POOL_INSTANCE_H__