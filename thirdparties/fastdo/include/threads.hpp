#ifndef __THREADS_HPP__
#define __THREADS_HPP__

namespace winux
{
//=========================================================================================
/** \brief 线程相关错误
 *
 *  可通过getErrCode()取得错误代码 */
class ThreadSysError : public Error
{
public:
    enum
    {
        FailedThreadAttrCreate = 1,
        FailedThreadAttrDestroy,
        FailedThreadAttrSetVal,
        FailedThreadAttrGetVal,
        FailedThreadJoin,

    };
    ThreadSysError( int errCode, AnsiString const & errStr ) throw() : Error( errCode, errStr ) {  }
    /** \brief 获取错误代码 */
    int getErrCode() const throw() { return this->getErrType(); }
};
//=========================================================================================
/** \brief 线程属性 */
class WINUX_DLL ThreadAttr
{
public:
    /** \brief 构造函数，isCreate=false时不创建线程属性，用户手动调用create()创建。 */
    explicit ThreadAttr( bool isCreate = true );
    ~ThreadAttr();

    /** \brief 创建并初始化一个线程属性 */
    void create();

    /** \brief 销毁一个线程属性 */
    void destroy();

public:
    //attributes:

    /** \brief 分离状态类型 */
    enum DetachStateType
    {
        threadCreateJoinable = 0,  ///< 可连接
        threadCreateDetached = 1,  ///< 分离的
    };
    /** \brief 设置分离状态 */
    void setDetachState( DetachStateType detachState = threadCreateJoinable );
    /** \brief 获取分离状态 */
    DetachStateType getDetachState() const;

    void setStackSize( size_t stackSize );
    size_t getStackSize() const;

private:
    MembersWrapper<struct ThreadAttr_Data> _self;
    friend class Thread;

    DISABLE_OBJECT_COPY(ThreadAttr)
};

//=========================================================================================
/** \brief 线程 */
class WINUX_DLL Thread
{
public:
    /** \brief 线程ID */
    class WINUX_DLL ThreadId
    {
    public:
        ThreadId();
        ThreadId( ThreadId const & other );
        ~ThreadId();
        ThreadId & operator = ( ThreadId const & other );
        bool operator == ( ThreadId const & other ) const;
        bool operator != ( ThreadId const & other ) const { return !this->operator == (other); }
        operator bool() const;
    private:
        MembersWrapper<struct ThreadId_Data> _self;

        friend class Thread;
    };

public:
    /** \brief 得到调用者线程的ThreadId */
    static ThreadId Self();

    /** \brief 调用者线程退出 */
    static void Exit( void * exitVal = NULL );

    /** \brief 调用者线程连接指定其他线程
     *
     *  退出状态由返回值或者otherThread.getExitVal()取得 */
    static void * Join( Thread & otherThread );

    /** \brief 线程处理函数指针类型 */
    typedef void * (* PFN_ThreadFunc)( void * param );

public:
    /** \brief 构造函数1
     *
     *  isStartup=false时不立即创建线程，忽略attr参数，用户手动调用startup()函数创建线程
     *  isStartup=true时立即创建一个线程，以run()作为线程处理函数、attr为线程属性，用户需要派生Thread类重写run()函数以定义自己的处理 */
    Thread( bool isStartup = false, ThreadAttr const & attr = ThreadAttr(false) );

    /** \brief 构造函数2
     *
     * 立即创建一个线程，以startRoutine作为线程处理函数、attr为线程属性 */
    Thread( PFN_ThreadFunc startRoutine, void * param, ThreadAttr const & attr = ThreadAttr(false) );

    /** \brief 析构函数 */
    virtual ~Thread();

    /** \brief 实际创建一个线程 */
    void startup( PFN_ThreadFunc startRoutine, void * param, ThreadAttr const & attr = ThreadAttr(false) );

    /** \brief 被调用者线程连接 */
    void * joined();

    /** \brief 分离线程，自动管理资释放源 */
    void detach();

public:
    //attributes:

    /** \brief 取得退出值 */
    void * getExitVal() const;
    /** \brief 设置退出值 */
    void setExitVal( void * exitVal );

    /** \brief 线程Id */
    ThreadId get() const;

protected:
    /** \brief 线程处理函数，用户需要派生重写此函数 */
    virtual void run();

private:
    static void * _ThreadFunc( void * param );

    MembersWrapper<struct Thread_Data> _self;
    DISABLE_OBJECT_COPY(Thread)
};
//=========================================================================================
/** \brief 互斥量属性 */
class WINUX_DLL MutexAttr
{
public:
    explicit MutexAttr( bool isCreate = true );
    ~MutexAttr();

    /** \brief 创建并初始化 */
    void create();

    /** \brief 销毁 */
    void destroy();

private:
    MembersWrapper<struct MutexAttr_Data> _self;
    friend class Mutex;

    DISABLE_OBJECT_COPY(MutexAttr)
};
//=========================================================================================
/** \brief 互斥量 */
class WINUX_DLL Mutex : public winux::ILockObj
{
public:
    Mutex( bool isCreate = false, MutexAttr const & attr = MutexAttr(false) );
    ~Mutex();

    /** \brief 创建并初始化 */
    void create( MutexAttr const & attr = MutexAttr(false) );

    /** \brief 销毁 */
    void destroy();

    bool lock();
    bool tryLock();
    bool unlock();

private:
    MembersWrapper<struct Mutex_Data> _self;
    friend class Condition;

    DISABLE_OBJECT_COPY(Mutex)
};

class WINUX_DLL ConditionAttr
{
public:
    explicit ConditionAttr( bool isCreate = true );
    ~ConditionAttr();

    /** \brief 创建并初始化 */
    void create();

    /** \brief 销毁 */
    void destroy();

private:
    MembersWrapper<struct ConditionAttr_Data> _self;
    friend class Condition;

    DISABLE_OBJECT_COPY(ConditionAttr)
};

class WINUX_DLL Condition
{
public:
    Condition( bool isCreate = false, ConditionAttr const & attr = ConditionAttr(false) );
    ~Condition();

    /** \brief 创建并初始化 */
    void create( ConditionAttr const & attr = ConditionAttr(false) );

    /** \brief 销毁 */
    void destroy();

    /** \brief 等待事件被通知
     *
     *  在mutex锁定之间调用，超时返回false。不满足条件请循环wait()。eg:
     *  while ( !条件 ) wait(); */
    bool wait( Mutex & mutex, int timeout = -1 );

    /** \brief 等待谓词条件达成
     *
     *  在mutex锁定之间调用，给定一个谓词描述条件达成，条件不达成就循环wait()，直到达成或超时，超时返回false。 */
    template < typename _Predicate >
    bool waitUntil( _Predicate pred, Mutex & mutex, int timeout = -1 )
    {
        while ( !pred() )
            if ( !this->wait( mutex, timeout ) )
                return pred();
        return true;
    }

    /** \brief 通知一个正在wait()中的线程醒来 */
    void notify();

    /** \brief 通知所有正在wait()中的线程醒来 */
    void notifyAll();

private:

    MembersWrapper<struct Condition_Data> _self;
    DISABLE_OBJECT_COPY(Condition)
};

}

#endif // __THREADS_HPP__
