#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

namespace eiennet
{
/** \brief socket库初始化 */
class EIENNET_DLL SocketLib
{
public:
    SocketLib();
    ~SocketLib();
private:
    winux::MembersWrapper<struct SocketLib_Data> _self;
    DISABLE_OBJECT_COPY(SocketLib)
};

/** \brief 套接字错误 */
class EIENNET_DLL SocketError : public winux::Error
{
public:
    SocketError( int errType, winux::AnsiString const & errStr ) : winux::Error( errType, errStr ) { }
};

/** \brief 套接字地址对象基类 */
class EIENNET_DLL Address
{
public:
    Address() { }
    virtual ~Address() { }

    /** \brief 以`void*`形式取得内部的`sockaddr*`值. */
    virtual void * get() const { return NULL; }
    /** \brief 取得地址的数据大小,一般为内部地址结构体的大小. */
    virtual winux::uint size() const { return 0U; }

    virtual winux::String toString() const { return ""; }
};

/** \brief 套接字基类 */
class EIENNET_DLL Socket
{
public:

    enum AddrFamily {
        afUnspec = 0,           /* Unspecified.  */
        afLocal = 1,            /* Local to host (pipes and file-domain).  */
        afUnix = afLocal,       /* POSIX name for PF_LOCAL.  */
        afFile = afLocal,       /* Another non-standard name for PF_LOCAL.  */
        afInet = 2,             /* IP protocol family.  */
        afAx25 = 3,             /* Amateur Radio AX.25.  */
        afIpx = 4,              /* Novell Internet Protocol.  */
        afAppletalk = 5,        /* Appletalk DDP.  */
        afNetrom = 6,           /* Amateur radio NetROM.  */
        afBridge = 7,           /* Multiprotocol bridge.  */
        afAtmpvc = 8,           /* ATM PVCs.  */
        afX25 = 9,              /* Reserved for X.25 project.  */
        afInet6 = 10,           /* IP version 6.  */
        afRose = 11,            /* Amateur Radio X.25 PLP.  */
        afDecnet = 12,          /* Reserved for DECnet project.  */
        afNetbeui = 13,         /* Reserved for 802.2LLC project.  */
        afSecurity = 14,        /* Security callback pseudo AF.  */
        afKey = 15,             /* PF_KEY key management API.  */
        afNetlink = 16,
        afRoute = afNetlink,    /* Alias to emulate 4.4BSD.  */
        afPacket = 17,          /* Packet family.  */
        afAsh = 18,             /* Ash.  */
        afEconet = 19,          /* Acorn Econet.  */
        afAtmsvc = 20,          /* ATM SVCs.  */
        afRds = 21,             /* RDS sockets.  */
        afSna = 22,             /* Linux SNA Project */
        afIrda = 23,            /* IRDA sockets.  */
        afPppox = 24,           /* PPPoX sockets.  */
        afWanpipe = 25,         /* Wanpipe API sockets.  */
        afLlc = 26,             /* Linux LLC.  */
        afUnknown27 = 27,
        afUnknown28 = 28,
        afCan = 29,             /* Controller Area Network.  */
        afTipc = 30,            /* TIPC sockets.  */
        afBluetooth = 31,       /* Bluetooth sockets.  */
        afIucv = 32,            /* IUCV sockets.  */
        afRxrpc = 33,           /* RxRPC sockets.  */
        afIsdn = 34,            /* mISDN sockets.  */
        afPhonet = 35,          /* Phonet sockets.  */
        afIeee802154 = 36,      /* IEEE 802.15.4 sockets.  */
        afMax = 37              /* For now..  */
    };

    enum SockType {
        sockStream = 0, /* Sequenced, reliable, connection-based byte streams.  */
        sockDatagram,   /* Connectionless, unreliable datagrams of fixed maximum length.  */
        sockRaw,        /* Raw protocol interface.  */
        sockRdm,        /* Reliably-delivered messages.  */
        sockSeqPacket,  /* Sequenced, reliable, connection-based, datagrams of fixed maximum length.  */
        sockDccp,       /* Datagram Congestion Control Protocol.  */
        sockPacket,     /* Linux specific way of getting packets at the dev level.  For writing rarp and
                               other similar things on the user level. */

        /* Flags to be ORed into the type parameter of socket and socketpair and used for the flags parameter of paccept.  */

        sockCloexec,    /* Atomically set close-on-exec flag for the new descriptor(s).  */
        sockNonblock    /* Atomically mark descriptor(s) as non-blocking.  */
    };

    enum Protocol {
        protoIp = 0,    /* Dummy protocol for TCP.  */
        protoIcmp,      /* Internet Control Message Protocol.  */
        protoIgmp,      /* Internet Group Management Protocol. */
        protoIpip, protoIpv4 = protoIpip, /* IPIP tunnels (older KA9Q tunnels use 94).  */
        protoTcp,      /* Transmission Control Protocol.  */
        protoEgp,      /* Exterior Gateway Protocol.  */
        protoPup,      /* PUP protocol.  */
        protoUdp,      /* User Datagram Protocol.  */
        protoIdp,      /* XNS IDP protocol.  */
        protoTp,       /* SO Transport Protocol Class 4.  */
        protoDccp,     /* Datagram Congestion Control Protocol.  */
        protoIpv6,     /* IPv6 header.  */
        protoRsvp,     /* Reservation Protocol.  */
        protoGre,      /* General Routing Encapsulation.  */
        protoEsp,      /* encapsulating security payload.  */
        protoAh,       /* authentication header.  */
        protoMtp,      /* Multicast Transport Protocol.  */
        protoBeetph,   /* IP option pseudo header for BEET.  */
        protoEncap,    /* Encapsulation Header.  */
        protoPim,      /* Protocol Independent Multicast.  */
        protoComp,     /* Compression Header Protocol.  */
        protoSctp,     /* Stream Control Transmission Protocol.  */
        protoUdplite,  /* UDP-Lite protocol.  */
        protoRaw,      /* Raw IP packets.  */
        protoMax
    };

    // static methods ---------------------------------------------------------------------

    // constructor/destructor -------------------------------------------------------------

    /** \brief 构造函数1,包装现有socket描述符
     *
     *  \param sock socket描述符
     *  \param isNewSock 指示是否为新建socket. 如果为true,则会在Socket destroy时,自动close(sock). */
    explicit Socket( int sock = 0, bool isNewSock = false );

    /** \brief 构造函数2,根据'地址簇','类型','协议'新建一个socket. */
    Socket( AddrFamily af, SockType sockType, Protocol proto );

#ifndef MOVE_SEMANTICS_DISABLED
    /** \brief 移动构造函数 */
    Socket( Socket && other );
    /** \brief 移动赋值操作 */
    Socket & operator = ( Socket && other );
#endif

    virtual ~Socket();

    // methods ----------------------------------------------------------------------------

    /** \brief 根据'地址簇','类型','协议'创建一个socket */
    Socket & create( AddrFamily af, SockType sockType, Protocol proto );

    /** \brief 关闭socket描述符 */
    void close();

    /** \brief 发送数据 */
    int send( void const * data, int size );
    int send( winux::AnsiString const & data ) { return this->send( data.c_str(), data.size() ); }
    int send( winux::Buffer const & buf ) { return this->send( buf.getBuf(), buf.getSize() ); }
    /** \brief 接收数据 */
    int recv( void * buf, int size );

    int sendTo( Address const & addr, void const * data, int size );
    int recvFrom( Address & addr, void * buf, int size );

    Socket & connect( Address const & addr );

    Socket & bind( Address const & addr );
    Socket & listen( int backlog );
    winux::SharedPointer<Socket> accept();

    Socket & fcntl();
    Socket & ioctl();

    // attributes -------------------------------------------------------------------------
    bool isServer() const;

    int get() const;
    operator int() const { return this->get(); }
    operator bool() const { return this->get() > -1; }

protected:
    winux::MembersWrapper<struct Socket_Data> _self;
    DISABLE_OBJECT_COPY(Socket)
};

///////////////////////////////////////////////////////////////////////////////////////////

namespace select
{

class EIENNET_DLL Client
{
public:
    Client() { }
    virtual ~Client() { }
protected:
    virtual void onCanReceive() { }
    virtual void onCanSend() { }
    virtual void onClosed() { }
    virtual void onError() { }
private:
    DISABLE_OBJECT_COPY(Client)
};

class EIENNET_DLL Server
{
public:
    /** \brief 给一个socket将被当作服务器socket对待,自动bind()并listen(). */
    explicit Server( winux::SimplePointer<Socket> servSock ) : _servSock(servSock) { }
    virtual ~Server() { }

    virtual void run()
    {

    }

protected:
    virtual void onAccpet( winux::SharedPointer<Client> clientSock ) { }
    virtual void onCanReceive( Client* client ) { }
    virtual void onCanSend( Client* client ) { }
    virtual void onClosed( Client* client ) { }
    virtual void onError( Client* client ) { }

    winux::SimplePointer<Socket> _servSock;
private:
    DISABLE_OBJECT_COPY(Server)
};


}

///////////////////////////////////////////////////////////////////////////////////////////
/** \brief IP地址族套接字 */
namespace ip
{

/** \brief IP地址结构对象 */
class EIENNET_DLL Address : public eiennet::Address
{
public:
    Address();
    Address( winux::String const & ipAddr, winux::ushort port );
    Address( Address const & other );
    virtual ~Address();
    Address & operator = ( Address const & other );

    /** \brief 以`void*`形式取得内部的`sockaddr_in*`值. */
    virtual void * get() const;
    /** \brief 取得内部的`sockaddr`结构大小. */
    virtual winux::uint size() const;
    virtual winux::String toString() const;
private:
    winux::MembersWrapper<struct Address_Data> _self;
};

namespace tcp
{
/** \brief TCP/IP套接字 */
class EIENNET_DLL Socket : public eiennet::Socket
{
public:
    typedef eiennet::Socket BaseClass;

    /** \brief 构造函数1,包装现有socket描述符
     *
     *  \param sock socket描述符
     *  \param isNewSock 指示是否为新建socket. 如果为true,则会在Socket destroy时,自动close(sock). */
    explicit Socket( int sock, bool isNewSock = false ) : BaseClass( sock, isNewSock ) { }

    /** \brief 构造函数2,新建一个tcp socket. */
    Socket() : BaseClass( BaseClass::afInet, BaseClass::sockStream, BaseClass::protoTcp ) { }

#ifndef MOVE_SEMANTICS_DISABLED
    /** \brief 移动构造函数 */
    Socket( Socket && other ) : BaseClass( std::move(other) ) { }
    /** \brief 移动赋值操作 */
    Socket & operator = ( Socket && other )
    {
        BaseClass::operator = ( std::move(other) );
        return *this;
    }
#endif

    Socket & connect( winux::String const & ipAddr, winux::ushort port )
    {
        BaseClass::connect( Address( ipAddr, port ) );
        return *this;
    }

    Socket & bind( winux::String const & ipAddr, winux::ushort port )
    {
        BaseClass::bind( Address( ipAddr, port ) );
        return *this;
    }
    Socket & listen( int backlog )
    {
        BaseClass::listen(backlog);
        return *this;
    }
    winux::SharedPointer<Socket> accept()
    {
        return BaseClass::accept();
    }
};

} // namespace tcp

namespace udp
{
/** \brief UDP/IP套接字 */
class EIENNET_DLL Socket : public eiennet::Socket
{
public:
    typedef eiennet::Socket BaseClass;

    /** \brief 构造函数1,包装现有socket描述符
     *
     *  \param sock socket描述符
     *  \param isNewSock 指示是否为新建socket. 如果为true,则会在Socket destroy时,自动close(sock). */
    explicit Socket( int sock, bool isNewSock = false ) : BaseClass( sock, isNewSock ) { }

    /** \brief 构造函数2,新建一个udp socket. */
    Socket() : BaseClass( BaseClass::afInet, BaseClass::sockDatagram, BaseClass::protoUdp ) { }

#ifndef MOVE_SEMANTICS_DISABLED
    /** \brief 移动构造函数 */
    Socket( Socket && other ) : BaseClass( std::move(other) ) { }
    /** \brief 移动赋值操作 */
    Socket & operator = ( Socket && other )
    {
        BaseClass::operator = ( std::move(other) );
        return *this;
    }
#endif

    int sendTo( winux::String const & ipAddr, winux::ushort port, void const * data, int size )
    {
        return BaseClass::sendTo( Address( ipAddr, port ), data, size );
    }
    int recvFrom( winux::String const & ipAddr, winux::ushort port, void * buf, int size )
    {
        Address addr( ipAddr, port );
        return BaseClass::recvFrom( addr, buf, size );
    }

};

} // namespace udp

} // namespace ip

} // namespace eiennet



#endif // __SOCKET_HPP__
