/**
 * @file    SocketDataType.cpp
 * @author  Longwei Lai<lailongwei@126.com>
 * @date    2013/06/14
 * @version 1.0
 *
 * @brief
 */

#include "llbc/common/Export.h"
#include "llbc/common/BeforeIncl.h"

#include "llbc/common/PFConfig.h"

#include "llbc/common/OSHeader.h"
#include "llbc/common/Errno.h"
#include "llbc/common/Errors.h"
#include "llbc/common/Define.h"

#include "llbc/common/SocketDataType.h"

std::ostream &operator <<(std::ostream &o, const LLBC_NS LLBC_SockAddr_IN &a)
{
    LLBC_NS LLBC_String str;
    str.format("%s:%d", a.GetIpAsString().c_str(), a.GetPort());

    return o <<str;
}

__LLBC_NS_BEGIN

LLBC_SockAddr_IN::LLBC_SockAddr_IN(uint16 addrFamily, const char *ip, uint16 port)
{
    _addrFamily = addrFamily;

    _ip = ::inet_addr(ip);
    _port = htons(port);

    ::memset(_zero, 0, sizeof(_zero));
}

LLBC_SockAddr_IN::LLBC_SockAddr_IN(const char *ip, uint16 port)
{
    _addrFamily = AF_INET;

    _ip = ::inet_addr(ip);
    _port = htons(port);

    ::memset(_zero, 0, sizeof(_zero));
}

LLBC_SockAddr_IN::LLBC_SockAddr_IN(const LLBC_SockAddr_IN &right)
{
    ::memcpy(this, &right, sizeof(LLBC_SockAddr_IN));
}

LLBC_SockAddr_IN::~LLBC_SockAddr_IN()
{
}

struct sockaddr_in LLBC_SockAddr_IN::ToOSDataType() const
{
    struct sockaddr_in addr;

    addr.sin_family = _addrFamily;

#if LLBC_TARGET_PLATFORM_NON_WIN32
    addr.sin_addr.s_addr = _ip;
#else
    addr.sin_addr.S_un.S_addr = _ip;
#endif
    addr.sin_port = _port;

    ::memcpy(addr.sin_zero, _zero, sizeof(addr.sin_zero));

    return addr;
}

int LLBC_SockAddr_IN::FromOSDataType(const struct sockaddr_in *sockaddr)
{
    if (!sockaddr)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

    _addrFamily= sockaddr->sin_family;

#if LLBC_TARGET_PLATFORM_NON_WIN32
    _ip = sockaddr->sin_addr.s_addr;
#else
    _ip = sockaddr->sin_addr.S_un.S_addr;
#endif
    _port = sockaddr->sin_port;

    ::memset(_zero, 0, sizeof(_zero));
    ::memcpy(_zero, sockaddr->sin_zero, sizeof(sockaddr->sin_zero));

    return LLBC_RTN_OK;
}

int LLBC_SockAddr_IN::FromOSDataType(const struct sockaddr *sockaddr, LLBC_SocketLen len)
{
    if (!sockaddr || len < sizeof(sockaddr_in))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

    return this->FromOSDataType(
        reinterpret_cast<const struct sockaddr_in *>(sockaddr));
}

uint16 LLBC_SockAddr_IN::GetAddressFamily() const
{
    return _addrFamily;
}

int LLBC_SockAddr_IN::SetAddressFamily(uint16 family)
{
    if (family != AF_INET)
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

    _addrFamily = family;
    return LLBC_RTN_OK;
}

LLBC_String LLBC_SockAddr_IN::GetIpAsString() const
{
    struct in_addr addr = this->GetIpAsINAddr();
    return ::inet_ntoa(addr);
}

int LLBC_SockAddr_IN::GetIpAsNumber() const
{
    return ntohl(_ip);
}

int LLBC_SockAddr_IN::GetIpAsNumberN() const
{
    return _ip;
}

struct in_addr LLBC_SockAddr_IN::GetIpAsINAddr() const
{
    struct in_addr addr;
#if LLBC_TARGET_PLATFORM_NON_WIN32
    addr.s_addr = _ip;
#else
    addr.S_un.S_addr = _ip;
#endif

    return addr;
}

void LLBC_SockAddr_IN::SetIp(const LLBC_String &ip)
{
    if (ip.empty())
    {
        this->SetIp("127.0.0.1");
        return;
    }

    _ip = ::inet_addr(ip.c_str());
}

void LLBC_SockAddr_IN::SetIp(int ip)
{
    _ip = htonl(ip);
}

void LLBC_SockAddr_IN::SetIpN(int ip)
{
    _ip = ip;
}

void LLBC_SockAddr_IN::SetIp(const struct in_addr &addr)
{
#if LLBC_TARGET_PLATFORM_NON_WIN32
    _ip = addr.s_addr;
#else
    _ip = addr.S_un.S_addr;
#endif
}

uint16 LLBC_SockAddr_IN::GetPort() const
{
    return ntohs(_port);
}

uint16 LLBC_SockAddr_IN::GetPortN() const
{
    return _port;
}

void LLBC_SockAddr_IN::SetPort(uint16 port)
{
    _port = htons(port);
}

void LLBC_SockAddr_IN::SetPortN(uint16 port)
{
    _port = port;
}

int LLBC_SockAddr_IN::GetPaddingBuf(void *buf, size_t &len)
{
    if (!buf || len < sizeof(_zero))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

    len = sizeof(_zero);
    ::memcpy(buf, _zero, len);

    return LLBC_RTN_OK;
}

int LLBC_SockAddr_IN::SetPaddingBuf(const char *buf, size_t len)
{
    if (!buf || len < sizeof(_zero))
    {
        LLBC_SetLastError(LLBC_ERROR_ARG);
        return LLBC_RTN_FAILED;
    }

    ::memcpy(_zero, buf, sizeof(_zero));

    return LLBC_RTN_OK;
}

void LLBC_SockAddr_IN::ZeroPaddingBuf()
{
    ::memset(_zero, 0, sizeof(_zero));
}

LLBC_String LLBC_SockAddr_IN::ToString() const
{
    LLBC_String desc;
    desc.format("%s:%d", this->GetIpAsString().c_str(), this->GetPort());

    return desc;
}

LLBC_SockAddr_IN &LLBC_SockAddr_IN::operator =(const LLBC_SockAddr_IN &right)
{
    ::memcpy(this, &right, sizeof(LLBC_SockAddr_IN));
    return *this;
}

bool LLBC_SockAddr_IN::operator ==(const LLBC_SockAddr_IN &right) const
{
    if (this == &right)
        return true;

    return ((_addrFamily == right._addrFamily) &&
             (_ip == right._ip) &&
             (_port == right._port) &&
             memcmp(_zero, right._zero, sizeof(_zero)) == 0);
}

LLBC_Overlapped::LLBC_Overlapped()
: opcode(LLBC_OverlappedOpcode::End)
, sessionId(0)
, sock(LLBC_INVALID_SOCKET_HANDLE)
, acceptSock(LLBC_INVALID_SOCKET_HANDLE)
, data(NULL)
{
#if LLBC_TARGET_PLATFORM_WIN32
    ::memset(this, 0, sizeof(OVERLAPPED));
#endif // LLBC_TARGET_PLATFORM_WIN32
}

LLBC_OverlappedGroup::LLBC_OverlappedGroup()
: _sock(LLBC_INVALID_SOCKET_HANDLE)
, _ols()

, _delDataProc(NULL)
{
}

LLBC_OverlappedGroup::~LLBC_OverlappedGroup()
{
}

LLBC_SocketHandle LLBC_OverlappedGroup::GetSocketHandle() const
{
    return _sock;
}

void LLBC_OverlappedGroup::SetSocketHandle(LLBC_SocketHandle handle)
{
    _sock = handle;
}

void LLBC_OverlappedGroup::SetDeleteDataProc(DeleteDataProc proc)
{
    _delDataProc = proc;
}

void LLBC_OverlappedGroup::InsertOverlapped(LLBC_POverlapped ol)
{
    _ols.insert(ol);
}

void LLBC_OverlappedGroup::DeleteOverlapped(LLBC_POverlapped ol)
{
    _OLContainerIter iter = _ols.find(ol);
    ASSERT(iter != _ols.end() && "LLBC_OverlappedGroup::DeleteOverlapped(): not found overlapped!");

    this->ClearOverlappedMembers(ol);
    delete ol;

    _ols.erase(iter);
}

void LLBC_OverlappedGroup::DeleteAllOverlappeds()
{
    _OLContainerIter iter = _ols.begin();
    for (; iter != _ols.end(); iter++)
    {
        LLBC_POverlapped ol = *iter;
        this->ClearOverlappedMembers(ol);
        delete ol;
    }

    _ols.clear();
}

void LLBC_OverlappedGroup::RemoveOverlapped(LLBC_POverlapped ol)
{
    ASSERT(_ols.erase(ol) != 0);
}

void LLBC_OverlappedGroup::ClearOverlappedMembers(LLBC_POverlapped ol)
{
    if (ol->acceptSock != LLBC_INVALID_SOCKET_HANDLE)
    {
#if LLBC_TARGET_PLATFORM_NON_WIN32
        ::close(ol->acceptSock);
#else // LLBC_TARGET_PLATFORM_NON_WIN32
        ::closesocket(ol->acceptSock);
#endif // LLBC_TARGET_PLATFORM_WIN32
        ol->acceptSock = LLBC_INVALID_SOCKET_HANDLE;
    }

    if (ol->data && _delDataProc)
    {
        (*_delDataProc)(ol->data);
        ol->data = NULL;
    }
}

__LLBC_NS_END

#include "llbc/common/AfterIncl.h"