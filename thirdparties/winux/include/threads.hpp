#ifndef __THREADS_HPP__
#define __THREADS_HPP__
//
// threads 提供线程相关的功能
//

namespace winux
{
// 类型定义
#if defined(OS_WIN)
typedef DWORD pid_t;
#else
typedef int pid_t;
#endif

//=========================================================================================
/** \brief 线程相关错误
 *
 *  可通过getErrCode()取得pthread函数错误返回代码 */
class ThreadSysError : public Error
{
public:
    ThreadSysError( int errCode, AnsiString const & errStr ) throw() : Error( errCode, errStr ) {  }
    /** \brief 获取错误代码 */
    int getErrCode() const throw() { return this->getErrType(); }
};

//=========================================================================================
// 线程相关错误返回值为errno.h定义的错误码
//=========================================================================================

/** \brief 线程调度策略 */
enum SchedulePolicy
{
    schedOther = 0, //!< 它是默认的线程分时调度策略，所有的线程的优先级别都是0，线程的调度是通过分时来完成的。如果系统使用这种调度策略，程序将无法设置线程的优先级。请注意，这种调度策略也是抢占式的，当高优先级的线程准备运行的时候，当前线程将被抢占并进入等待队列。这种调度策略仅仅决定线程在可运行线程队列中的具有相同优先级的线程的运行次序。
    schedFifo, //!< 它是一种实时的先进先出调用策略，且只能在超级用户下运行。这种调用策略仅仅被使用于优先级大于0的线程。它意味着，使用SCHED_FIFO的可运行线程将一直抢占使用SCHED_OTHER的运行线程。此外SCHED_FIFO是一个非分时的简单调度策略，当一个线程变成可运行状态，它将被追加到对应优先级队列的尾部((POSIX 1003.1)。当所有高优先级的线程终止或者阻塞时，它将被运行。对于相同优先级别的线程，按照简单的先进先运行的规则运行。我们考虑一种很坏的情况，如果有若干相同优先级的线程等待执行，然而最早执行的线程无终止或者阻塞动作，那么其他线程是无法执行的，除非当前线程调用如pthread_yield之类的函数，所以在使用SCHED_FIFO的时候要小心处理相同级别线程的动作。
    schedRr, //!< 鉴于SCHED_FIFO调度策略的一些缺点，SCHED_RR对SCHED_FIFO做出了一些增强功能。从实质上看，它还是SCHED_FIFO调用策略。它使用最大运行时间来限制当前进程的运行，当运行时间大于等于最大运行时间的时候，当前线程将被切换并放置于相同优先级队列的最后。这样做的好处是其他具有相同级别的线程能在“自私“线程下执行。
    schedMin = schedOther,
    schedMax = schedRr
};

/** \brief 调度参数 */
struct ScheduleParam
{
    int schedPriority;
};

/** \brief 线程调度相关静态方法 */
struct WINUX_DLL Schedule
{
#ifdef Yield
#undef Yield
#endif
    static int Yield(void);
    static int GetPriorityMin( SchedulePolicy policy );
    static int GetPriorityMax( SchedulePolicy policy );
    /** \exception ThreadSysError */
    static void SetScheduler( pid_t pid, SchedulePolicy policy );
    /** \exception ThreadSysError */
    static SchedulePolicy GetScheduler( pid_t pid );
};

/** \brief 线程属性 */
class WINUX_DLL ThreadAttr
{
public:
    /** \brief 构造函数，isCreate=false时不创建线程属性，用户手动调用create()创建。 */
    explicit ThreadAttr( bool isCreate = true );
    ~ThreadAttr();

    ThreadAttr( ThreadAttr && other );
    ThreadAttr & operator = ( ThreadAttr && other );

    /** \brief 创建并初始化一个线程属性 */
    int create();

    /** \brief 销毁一个线程属性 */
    int destroy();

    /** \brief 判断是否创建了attr */
    operator bool() const;

public:
    //attributes:

    /** \brief 分离状态类型 */
    enum DetachStateType
    {
        threadCreateJoinable = 0,  //!< 可连接
        threadCreateDetached = 1,  //!< 分离的
    };
    /** \brief 设置分离状态 */
    void setDetachState( DetachStateType detachState = threadCreateJoinable );
    /** \brief 获取分离状态 */
    DetachStateType getDetachState() const;

    /** \brief 设置线程栈大小 */
    void setStackSize( size_t stackSize );
    /** \brief 获取线程栈大小 */
    size_t getStackSize() const;

    /** \brief 设置调度参数（优先级）
     *
     *  \exception ThreadSysError */
    void setSchedParam( ScheduleParam const & param );
    /** \brief 获取调度参数（优先级）
     *
     *  \exception ThreadSysError */
    ScheduleParam getSchedParam() const;

    /** \brief 设置调度策略
     *
     *  \exception ThreadSysError */
    void setSchedPolicy( SchedulePolicy policy );
    /** \brief 获取调度策略
     *
     *  \exception ThreadSysError */
    SchedulePolicy getSchedPolicy() const;

private:
    MembersWrapper<struct ThreadAttr_Data> _self;
    friend class Thread;

    DISABLE_OBJECT_COPY(ThreadAttr)
};

/** \brief 线程ID */
class WINUX_DLL ThreadId
{
public:
    ThreadId();
    ~ThreadId();
    ThreadId( ThreadId const & other );
    ThreadId & operator = ( ThreadId const & other );
    ThreadId( ThreadId && other );
    ThreadId & operator = ( ThreadId && other );

    bool operator == ( ThreadId const & other ) const;
    bool operator != ( ThreadId const & other ) const { return !this->operator == (other); }

private:
    MembersWrapper<struct ThreadId_Data> _self;

    friend class Thread;
};

class ThreadGroup;
/** \brief 线程 */
class WINUX_DLL Thread
{
public:

    /** \brief 线程处理函数指针类型 */
    typedef void * (* ThreadFuncPtr)( void * param );

public:

    /** \brief 得到调用者线程的`ThreadId` */
    static ThreadId Self();

    /** \brief 调用者线程退出 */
    static void Exit( void * exitVal = NULL );

    /** \brief 调用者线程连接指定其他线程
     *
     *  退出状态由返回值或者`otherThread.getExitVal()`取得 */
    static void * Join( Thread & otherThread );

    // attributes
    /** \brief 设置建议并发数
     *
     *  \exception ThreadSysError  */
    static void SetConcurrency( int newLevel );
    /** \brief 获取并发数 */
    static int GetConcurrency( void );

private:
    // 默认线程处理函数, param被传入Thread对象, 调用thObj->run()
    static void * _ThreadDefaultFunc( void * param );

public:
    /** \brief 构造函数1
     *
     *  \param isStartup 是否立即启动线程。\n
     *  当`isStartup`=false时不立即创建线程，用户手动调用`startup()`函数创建线程。\n
     *  当`isStartup`=true时立即创建一个线程，以`run()`作为线程处理函数，用户需要派生`Thread`类重写`run()`函数以定义自己的处理。 */
    explicit Thread( bool isStartup = false ) : _param(nullptr), _attr(false), _exitVal(nullptr), _deleter(nullptr), _group(nullptr), _isRunning(false)
    {
        if ( isStartup ) this->startup();
    }

    /** \brief 构造函数2
     *
     *  立即创建一个线程，以routine作为线程处理例程 */
    template < typename _Fx, typename... _ArgType >
    explicit Thread( bool isStartup, _Fx routine, _ArgType&&... arg ) : _param(nullptr), _attr(false), _exitVal(nullptr), _deleter(nullptr), _group(nullptr), _isRunning(false)
    {
        this->setRunable( routine, std::forward<_ArgType>(arg)... );
        if ( isStartup ) this->startup();
    }

    /** \brief 析构函数 */
    virtual ~Thread();

    /** \brief 移动构造 */
    Thread( Thread && other );

    /** \brief 移动赋值 */
    Thread & operator = ( Thread && other );

    /** \brief 实际创建一个线程
     *
     *  以`run()`作为线程处理，用户可继承`Thread`类重写`run()`方法以定义自己的处理；也可以`setRunable()`设置一个可运行对象作为线程处理例程\n
     *  `startup()`会设置`Thread#_isRunning`为true，在默认的线程处理函数里，调用完`run()`方法后设置`Thread#_isRunning`为false */
    int startup();

    /** \brief 实际创建一个线程，提供你自己的处理例程 */
    template < typename _Fx, typename... _ArgType >
    int startup( _Fx routine, _ArgType&&... arg )
    {
        this->setRunable( routine, std::forward<_ArgType>(arg)... );
        return this->startup();
    }

    /** \brief 实际创建一个线程，提供你自己的处理函数 */
    int startupEx( ThreadFuncPtr startRoutine, void * param );

    /** \brief 被调用者线程连接 */
    void * joined();

    /** \brief 使线程分离
     *
     *  一个可连接线程启动后必须调用`joined()`来等待运行完毕，内部资源才能正确释放。调用此`detach()`方法后不能再调用`joined()`方法。\n
     *  此时，线程函数若有访问`Thread`对象的数据，必须保证线程运行结束前`Thread`对象的存在，但是因为没有了`joined()`方法的同步，你需要自己设计代码来保证对象不会提前释放。\n
     *  设置删除器可以一定程度上简化代码，如果`Thread`对象设置了删除器，那默认的线程函数处理时会在运行结束前调用删除器释放`Thread`对象。\n
     *  若你调用了`startupEx()`提供自己的线程处理函数，那就需要自己设计代码来保证`Thread`对象不会提前释放。 */
    int detach();

public:
    //attributes:

    /** \brief 设置调度策略、调度参数
     *
     *  \exception ThreadSysError */
    void setSchedParam( SchedulePolicy policy, int priority );
    /** \brief 获取调度策略、调度参数
     *
     *  \exception ThreadSysError */
    void getSchedParam( SchedulePolicy * policy, int * priority ) const;

    /** \brief 设置线程属性对象，下次`startup()`时生效 */
    ThreadAttr & attr();

    /** \brief 设置`Runable`，`run()`默认会调用它 */
    template < typename _Fx, typename... _ArgType >
    Thread & setRunable( _Fx routine, _ArgType&&... arg )
    {
        this->_runable.attachNew( NewRunable( routine, std::forward<_ArgType>(arg)... ) );
        return *this;
    }

    /** \brief 设置删除器场景以便默认线程函数删除`Thread`对象自己 */
    Thread & setDeleter( SimpleDeleterContext * deleter = NULL )
    {
        _deleter = deleter;
        return *this;
    }

    /** \brief 设置默认删除器场景以便默认线程函数删除`Thread`对象自己 */
    Thread & setDefaultDeleter() { return this->setDeleter( new SimpleDefaultDeleterContext<Thread*>(this) ); }

    /** \brief 设置自定义删除器场景以便默认线程函数删除`Thread`对象自己 */
    template < typename _Dt >
    Thread & setCustomDeleter( _Dt fn ) { return this->setDeleter( new SimpleCustomDeleterContext< Thread*, _Dt >( this, fn ) ); }

    /** \brief 取得退出值 */
    void * getExitVal() const { return _exitVal; }
    /** \brief 设置退出值，可在`run()`中调用 */
    Thread & setExitVal( void * exitVal )
    {
        _exitVal = exitVal;
        return *this;
    }

    /** \brief 线程Id */
    ThreadId get() const
    {
        return _threadId;
    }

protected:
    /** \brief 线程处理函数，默认调用内部的`Runable`，派生线程类可重写此函数 */
    virtual void run();
    struct _ThreadFuncParam * _param;   //!< 线程函数的参数

private:
    ThreadAttr _attr;                   //!< 线程属性
    void * _exitVal;                    //!< 线程退出值
    SimpleDeleterContext * _deleter;    //!< 简易删除器场景，用于自动删除`Thread`对象自身
    ThreadGroup * _group;               //!< 如果线程由线程组管理，这个指针指向线程组对象
    bool _isRunning;                    //!< 在默认线程处理函数和线程组处理函数情况下用于判断线程是否运行中。
    ThreadId _threadId;                 //!< 线程标识，不可显示输出其内容
    SimplePointer<Runable> _runable;    //!< 可运行接口如果不空，则`run()`会调用它

    friend class ThreadGroup;

    DISABLE_OBJECT_COPY(Thread)
};

//=========================================================================================
/** \brief 互斥量属性 */
class WINUX_DLL MutexAttr
{
public:
    explicit MutexAttr( bool isCreate = true );
    ~MutexAttr();

    MutexAttr( MutexAttr && other );
    MutexAttr & operator = ( MutexAttr && other );

    /** \brief 创建并初始化 */
    int create();

    /** \brief 销毁 */
    int destroy();

    /** \brief 判断是否创建了attr */
    operator bool() const;

public:
    /** \brief 互斥锁类型 */
    enum MutexType
    {
        mtxNormal, //!< 普通互斥锁
        mtxRecursive, //!< 递归互斥锁
        mtxErrorCheck,
        mtxDefault
    };
    int setType( MutexType type );

    MutexType getType() const;

private:
    MembersWrapper<struct MutexAttr_Data> _self;
    friend class Mutex;

    DISABLE_OBJECT_COPY(MutexAttr)
};

/** \brief 互斥量 */
class WINUX_DLL Mutex : public winux::ILockObj
{
public:
    explicit Mutex( bool isCreate = false );
    ~Mutex();

    Mutex( Mutex && other );
    Mutex & operator = ( Mutex && other );

    /** \brief 创建并初始化 */
    virtual int create();

    /** \brief 销毁 */
    int destroy();

    bool lock();
    bool tryLock();
    bool unlock();

    /** \brief 设置互斥量属性对象，下次create()时使用 */
    MutexAttr & attr();
private:
    MutexAttr _attr; //!< 互斥量属性
    MembersWrapper<struct Mutex_Data> _self;
    friend class Condition;

    DISABLE_OBJECT_COPY(Mutex)
};

/** \brief 递归锁 */
class WINUX_DLL RecursiveMutex : public Mutex
{
public:
    explicit RecursiveMutex( bool isCreate = false );
    int create() override;
};

//=========================================================================================
/** \brief 条件变量属性 */
class WINUX_DLL ConditionAttr
{
public:
    explicit ConditionAttr( bool isCreate = true );
    ~ConditionAttr();

    ConditionAttr( ConditionAttr && other );
    ConditionAttr & operator = ( ConditionAttr && other );

    /** \brief 创建并初始化 */
    int create();

    /** \brief 销毁 */
    int destroy();

    /** \brief 判断是否创建了attr */
    operator bool() const;

private:
    MembersWrapper<struct ConditionAttr_Data> _self;
    friend class Condition;

    DISABLE_OBJECT_COPY(ConditionAttr)
};

/** \brief 条件变量 */
class WINUX_DLL Condition
{
public:
    explicit Condition( bool isCreate = false );
    ~Condition();

    Condition( Condition && other );
    Condition & operator = ( Condition && other );

    /** \brief 创建并初始化 */
    int create();

    /** \brief 销毁 */
    int destroy();

    /** \brief 等待被通知
     *
     *  sec<1表示小于1秒的时间，sec<0表示无限等待。eg: sec=1.5表示等待1500ms
     *  在mutex锁定之间调用，超时返回false。不满足条件请循环wait()。eg: while ( !条件 ) wait(); */
    bool wait( Mutex & mutex, double sec = -1 );

    /** \brief 等待谓词条件达成
     *
     *  sec<1表示小于1秒的时间，sec<0表示无限等待。eg: sec=1.5表示等待1500ms
     *  在mutex锁定之间调用，给定一个谓词描述条件达成，条件不达成就循环wait()，直到达成或超时，超时返回pred()。 */
    template < typename _Predicate >
    bool waitUntil( _Predicate pred, Mutex & mutex, double sec = -1 )
    {
        while ( !pred() )
            if ( !this->wait( mutex, sec ) )
                return pred();
        return true;
    }

    /** \brief 通知一个正在wait()中的线程醒来 */
    int notify();

    /** \brief 通知所有正在wait()中的线程醒来 */
    int notifyAll();

    ConditionAttr & attr();

private:
    ConditionAttr _attr;
    MembersWrapper<struct Condition_Data> _self;
    DISABLE_OBJECT_COPY(Condition)
};

//===========================================================================================
/** \brief TLS Key */
class WINUX_DLL TlsKey
{
public:
    explicit TlsKey( void (*destructor)( void *pv ) = NULL );
    ~TlsKey();

    TlsKey( TlsKey && other );
    TlsKey & operator = ( TlsKey && other );

    /** \brief 创建并初始化 */
    int create( void (*destructor)( void *pv ) = NULL );

    /** \brief 销毁 */
    int destroy();

    /** \brief 获取Key值 */
    void * get() const;

private:
    MembersWrapper<struct TlsKey_Data> _self;

    friend class TlsVar;
    DISABLE_OBJECT_COPY(TlsKey)
};

/** \brief TLS Var */
class WINUX_DLL TlsVar
{
public:
    explicit TlsVar( TlsKey & tlsKey );
    ~TlsVar();

    TlsVar( TlsVar && other );
    TlsVar & operator = ( TlsVar && other );

    void * get();
    void * get() const;
    void set( void * v );

    template < typename _Ty >
    _Ty get() { return reinterpret_cast<_Ty>( this->get() ); }
    template < typename _Ty >
    _Ty get() const { return reinterpret_cast<_Ty>( this->get() ); }

    template < typename _Ty >
    void set( _Ty v ) { this->set( reinterpret_cast<void*>(v) ); }

    template < typename _Ty >
    _Ty & ref() { return *reinterpret_cast<_Ty*>( this->get() ); }
    template < typename _Ty >
    _Ty const & ref() const { return *reinterpret_cast<_Ty*>( this->get() ); }

private:
    TlsKey * _tlsKey;
    DISABLE_OBJECT_COPY(TlsVar)
};

//===========================================================================================

/** \brief 线程组
 *
 *  管理一组线程，它们有相同的线程处理例程 */
class WINUX_DLL ThreadGroup
{
public:

    /** \brief 构造函数1 默认 */
    ThreadGroup() : _mtxGroup(true), _cdtGroup(true)
    {
    }

    /** \brief 构造函数2 提供一个线程处理例程，并指定创建的线程数量 */
    template < typename _Fx, typename... _ArgType >
    ThreadGroup( size_t count, _Fx fn, _ArgType&&... arg ) : _mtxGroup(true), _cdtGroup(true)
    {
        for ( size_t i = 0; i < count; i++ )
        {
            Thread * p = new Thread( false, fn, std::forward<_ArgType>(arg)... );
            p->_group = this;
            _threads.emplace_back(p);
        }
    }

    virtual ~ThreadGroup()
    {
    }

    ThreadGroup( ThreadGroup && other ) : _mtxGroup( std::move(other._mtxGroup) ), _cdtGroup( std::move(other._cdtGroup) ), _threads( std::move(other._threads) )
    {
    }

    ThreadGroup & operator = ( ThreadGroup && other )
    {
        if ( this != &other )
        {
            // 先释放自身
            this->wait();

            _mtxGroup = std::move(other._mtxGroup);
            _cdtGroup = std::move(other._cdtGroup);
            _threads = std::move(other._threads);
        }
        return *this;
    }

    /** \brief 销毁所有线程 */
    ThreadGroup & destroy();

    /** \brief 按指定的线程处理例程，创建一定数量的线程 */
    template < typename _Fx, typename... _ArgType >
    ThreadGroup & create( size_t count, _Fx fn, _ArgType&&... arg )
    {
        this->destroy();

        for ( size_t i = 0; i < count; i++ )
        {
            Thread * p = new Thread( false, fn, std::forward<_ArgType>(arg)... );
            p->_group = this;
            _threads.emplace_back(p);
        }
        return *this;
    }

    /** \brief 创建一定数量指定的派生类线程 */
    template < class _ThreadCls >
    ThreadGroup & create( size_t count )
    {
        this->destroy();

        for ( size_t i = 0; i < count; i++ )
        {
            Thread * p = new _ThreadCls();
            p->_group = this;
            _threads.emplace_back(p);
        }
        return *this;
    }

    /** \brief 启动已经创建的线程 */
    ThreadGroup & startup();

    /** \brief 等待所有线程运行完毕
     *
     *  返回false表示超时，返回true表示运行线程全部正常退出 */
    bool wait( double sec = -1 );

private:
    static void * _ThreadGroupDefaultFunc( void * param );

    Mutex _mtxGroup; // 互斥量保护数据
    Condition _cdtGroup; // 用于判断组中线程是否全部运行完毕
    std::vector< SimplePointer<Thread> > _threads;

    DISABLE_OBJECT_COPY(ThreadGroup)
};


} // namespace winux

#endif // __THREADS_HPP__
