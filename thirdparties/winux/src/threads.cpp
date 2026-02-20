#include "winux.hpp"
#include "threads.hpp"
#include <pthread.h>

namespace winux
{
// struct Schedule ============================================================================
int Schedule::Yield( void )
{
    return sched_yield();
}

int Schedule::GetPriorityMin( SchedulePolicy policy )
{
    return sched_get_priority_min(policy);
}

int Schedule::GetPriorityMax( SchedulePolicy policy )
{
    return sched_get_priority_max(policy);
}

void Schedule::SetScheduler( pid_t pid, SchedulePolicy policy )
{
#if defined(OS_WIN) && defined(HAVE_PTHREAD)
    int rc = sched_setscheduler( pid, policy );
#elif defined(OS_WIN) && !defined(HAVE_PTHREAD)
    int rc = 0;
#else
    struct sched_param param = { 0 };
    int rc = sched_setscheduler( pid, policy, &param );
#endif
    if ( rc == -1 )
    {
        int err = errno;
        switch ( err )
        {
        case EINVAL:
            throw ThreadSysError( err, FormatA( "The value of the policy parameter is invalid, or one or more of the parameters contained in param is outside the valid range for the specified scheduling policy (pid=%d, policy=%d).", pid, policy ) );
            break;
        case ENOSYS:
            throw ThreadSysError( err, "The function sched_setscheduler() is not supported by this implementation." );
            break;
        case EPERM:
            throw ThreadSysError( err, FormatA( "The requesting process does not have permission to set either or both of the scheduling parameters or the scheduling policy of the specified process (pid=%d, policy=%d).", pid, policy ) );
            break;
        case ESRCH:
            throw ThreadSysError( err, FormatA( "No process can be found corresponding to that specified by pid (pid=%d, policy=%d).", pid, policy ) );
            break;
        default:
            throw ThreadSysError( err, FormatA( "Unknown errror (pid=%d, policy=%d).", pid, policy ) );
            break;
        }
    }
}

SchedulePolicy Schedule::GetScheduler( pid_t pid )
{
    int rc = sched_getscheduler(pid);
    if ( rc == -1 )
    {
        switch ( errno )
        {
        case ENOSYS:
            throw ThreadSysError( rc, "The function sched_getscheduler() is not supported by this implementation." );
            break;
        case EPERM:
            throw ThreadSysError( rc, FormatA( "The requesting process does not have permission to determine the scheduling policy of the specified process(%d).", pid ) );
            break;
        case ESRCH:
            throw ThreadSysError( rc, FormatA( "No process can be found corresponding to that specified by pid(%d).", pid ) );
            break;
        }
    }
    return (SchedulePolicy)rc;
}

// struct ThreadAttr_Data =====================================================================
struct ThreadAttr_Data
{
    pthread_attr_t * pThreadAttr;   //!< 是否实际创建了线程属性。当等于NULL时，表示仅仅创建了ThreadAttr马甲对象，并未创建线程属性
    pthread_attr_t threadAttr;  //!< Pthread线程属性不透明对象

    ThreadAttr_Data() : pThreadAttr(NULL)
    {
    }
};

// class ThreadAttr ---------------------------------------------------------------------------
ThreadAttr::ThreadAttr( bool isCreate )
{
    if ( isCreate )
    {
        this->create();
    }
}

ThreadAttr::~ThreadAttr()
{
    this->destroy();
}

ThreadAttr::ThreadAttr( ThreadAttr && other ) : _self( std::move(other._self) )
{
}

ThreadAttr & ThreadAttr::operator = ( ThreadAttr && other )
{
    if ( this != &other )
    {
        this->destroy(); // 销毁自身

        _self = std::move(other._self);
    }
    return *this;
}

int ThreadAttr::create()
{
    this->destroy();
    int rc = pthread_attr_init(&_self->threadAttr);
    if ( rc == 0 )
    {
        _self->pThreadAttr = &_self->threadAttr;
    }
    return rc;
}

int ThreadAttr::destroy()
{
    if ( _self && _self->pThreadAttr != NULL )
    {
        int rc = pthread_attr_destroy(_self->pThreadAttr);
        if ( rc == 0 )
        {
            _self->pThreadAttr = NULL;
        }
        return rc;
    }
    return -1;
}

ThreadAttr::operator bool() const
{
    return _self->pThreadAttr != NULL;
}

void ThreadAttr::setDetachState( DetachStateType detachState )
{
    int rc = 0;
    switch ( detachState )
    {
    case threadCreateJoinable:
        rc = pthread_attr_setdetachstate( _self->pThreadAttr, PTHREAD_CREATE_JOINABLE );
        break;
    case threadCreateDetached:
        rc = pthread_attr_setdetachstate( _self->pThreadAttr, PTHREAD_CREATE_DETACHED );
        break;
    }
}

ThreadAttr::DetachStateType ThreadAttr::getDetachState() const
{
    int detachState = 0;
    int rc = pthread_attr_getdetachstate( _self->pThreadAttr, &detachState );

    DetachStateType myDetachState;
    switch ( detachState )
    {
    case PTHREAD_CREATE_JOINABLE:
        myDetachState = threadCreateJoinable;
        break;
    case PTHREAD_CREATE_DETACHED:
        myDetachState = threadCreateDetached;
        break;
    }
    return myDetachState;
}

void ThreadAttr::setStackSize( size_t stackSize )
{
    int rc = pthread_attr_setstacksize( _self->pThreadAttr, stackSize );
}

size_t ThreadAttr::getStackSize() const
{
    size_t stackSize = 0;
    int rc = pthread_attr_getstacksize( _self->pThreadAttr, &stackSize );
    return stackSize;
}

void ThreadAttr::setSchedParam( ScheduleParam const & param )
{
    sched_param pa;
    pa.sched_priority = param.schedPriority;
    int rc = pthread_attr_setschedparam( _self->pThreadAttr, &pa );
    switch ( rc )
    {
    case EINVAL:
        throw ThreadSysError( rc, "The value of the attribute being set is not valid." );
        break;
    case ENOTSUP:
        throw ThreadSysError( rc, "An attempt was made to set the attribute to an unsupported value." );
        break;
    }
}

ScheduleParam ThreadAttr::getSchedParam() const
{
    sched_param pa = { 0 };
    int rc = pthread_attr_getschedparam( _self->pThreadAttr, &pa );
    ScheduleParam param;
    param.schedPriority = pa.sched_priority;
    switch ( rc )
    {
    case EINVAL:
        throw ThreadSysError( rc, "The value of the attribute being set is not valid." );
        break;
    case ENOTSUP:
        throw ThreadSysError( rc, "An attempt was made to set the attribute to an unsupported value." );
        break;
    }
    return param;
}

void ThreadAttr::setSchedPolicy( SchedulePolicy policy )
{
    int rc = pthread_attr_setschedpolicy( _self->pThreadAttr, (int)policy );
    switch ( rc )
    {
    case EINVAL:
        throw ThreadSysError( rc, FormatA( "The value of the attribute being set is not valid (policy=%d).", policy ) );
        break;
    case ENOTSUP:
        throw ThreadSysError( rc, FormatA( "An attempt was made to set the attribute to an unsupported value (policy=%d).", policy ) );
        break;
    case ENOSYS:
        throw ThreadSysError( rc, "The function pthread_attr_setschedpolicy() is not supported by this implementation." );
        break;
    }
}

SchedulePolicy ThreadAttr::getSchedPolicy() const
{
    int policy = 0;
    int rc = pthread_attr_getschedpolicy( _self->pThreadAttr, &policy );
    switch ( rc )
    {
    case ENOSYS:
        throw ThreadSysError( rc, "The function pthread_attr_getschedpolicy() is not supported by this implementation." );
        break;
    }
    return (SchedulePolicy)policy;
}

// struct ThreadId_Data =======================================================================
struct ThreadId_Data
{
    pthread_t * pThread;    //!< 是否实际创建了线程。当等于NULL时，表示仅仅创建了Thread马甲对象，并未创建线程
    pthread_t thread;   //!< Pthread线程不透明对象

    ThreadId_Data() : pThread(NULL)
    {
    }

};

// class ThreadId -----------------------------------------------------------------------------
ThreadId::ThreadId()
{
}

ThreadId::~ThreadId()
{
}

ThreadId::ThreadId( ThreadId const & other )
{
    _self = other._self;
}

ThreadId & ThreadId::operator = ( ThreadId const & other )
{
    if ( this != &other )
    {
        _self = other._self;
    }
    return *this;
}

ThreadId::ThreadId( ThreadId && other ) : _self( std::move(other._self) )
{
}

ThreadId & ThreadId::operator = ( ThreadId && other )
{
    if ( this != &other )
    {
        //this->destroy(); 销毁自身

        _self = std::move(other._self);
    }
    return *this;
}

bool ThreadId::operator == ( ThreadId const & other ) const
{
    int r = pthread_equal( _self->thread, other._self->thread );
    return  r != 0;
}

// class Thread static methods ----------------------------------------------------------------
ThreadId Thread::Self()
{
    ThreadId tid;
    tid._self->thread = pthread_self();
    return tid;
}

void Thread::Exit( void * exitVal )
{
    pthread_exit(exitVal);
}

void * Thread::Join( Thread & otherThread )
{
    void * exitVal;
    int rc = pthread_join( otherThread._threadId._self->thread, &exitVal );
    return exitVal;
}

void Thread::SetConcurrency( int newLevel )
{
    int rc = pthread_setconcurrency(newLevel);
    switch ( rc )
    {
    case EINVAL:
        throw ThreadSysError( rc, FormatA( "The value specified by newLevel(%d) is negative.", newLevel ) );
        break;
    case EAGAIN:
        throw ThreadSysError( rc, FormatA( "The value specific by newLevel(%d) would cause a system resource to be exceeded.", newLevel ) );
        break;
    }
}

int Thread::GetConcurrency( void )
{
    return pthread_getconcurrency();
}

struct _ThreadFuncParam
{
    Thread * threadObj;
    _ThreadFuncParam( Thread * thObj ) : threadObj(thObj) { }
};

// 默认线程处理函数，把_ThreadFuncParam对象传入了线程函数
// 请查看 Thread::startup()
void * Thread::_ThreadDefaultFunc( void * param )
{
    auto funcParam = reinterpret_cast<struct _ThreadFuncParam *>(param);
    Thread * threadObj;
    threadObj = funcParam->threadObj;
    threadObj->run(); // 可能长时间
    threadObj = funcParam->threadObj;
    void * exitVal = threadObj->getExitVal();
    if ( funcParam )
    {
        funcParam->threadObj->_param = nullptr;
        delete funcParam;
    }
    threadObj->_isRunning = false; // 设置运行状态为假
    if ( threadObj->_deleter ) threadObj->_deleter->release();
    return exitVal;
}

// class Thread -------------------------------------------------------------------------------
Thread::~Thread()
{

}

Thread::Thread( Thread && other ) :
    _attr( std::move(other._attr) ),
    _exitVal( std::move(other._exitVal) ),
    _deleter( std::move(other._deleter) ),
    _group( std::move(other._group) ),
    _isRunning( std::move(other._isRunning) ),
    _threadId( std::move(other._threadId) ),
    _param( std::move(other._param) ),
    _runable( std::move(other._runable) )
{
    if ( _param ) _param->threadObj = this;
    other._param = nullptr;
}

Thread & Thread::operator = ( Thread && other )
{
    if ( this != &other )
    {
        // 先销毁自身，但是没有有效的销毁方法，只能自旋等待运行退出
        while ( _isRunning )
        {
        }

        _attr = std::move(other._attr);
        _exitVal = std::move(other._exitVal);
        _deleter = std::move(other._deleter);
        _group = std::move(other._group);
        _isRunning = std::move(other._isRunning);
        _threadId = std::move(other._threadId);
        _param = std::move(other._param);
        _runable = std::move(other._runable);

        if ( _param ) _param->threadObj = this;
        other._param = nullptr;
    }
    return *this;
}

// 请查看 Thread::_ThreadDefaultFunc()
int Thread::startup()
{
    _isRunning = true; // 设置运行状态为真
    if ( _param != nullptr ) delete _param;
    _param = new struct _ThreadFuncParam(this);
    return this->startupEx( Thread::_ThreadDefaultFunc, _param );
}

int Thread::startupEx( ThreadFuncPtr startRoutine, void * param )
{
    int rc = pthread_create( &_threadId._self->thread, _attr._self->pThreadAttr, startRoutine, param );
    if ( rc == 0 )
    {
        _threadId._self->pThread = &_threadId._self->thread;
    }
    return rc;
}

void * Thread::joined()
{
    void * exitVal = NULL;
    int rc = pthread_join( _threadId._self->thread, &exitVal );
    return exitVal;
}

int Thread::detach()
{
    int rc = pthread_detach(_threadId._self->thread);
    return rc;
}

void Thread::setSchedParam( SchedulePolicy policy, int priority )
{
    sched_param param;
    param.sched_priority = priority;
    int rc = pthread_setschedparam( _threadId._self->thread, policy, &param );
    switch ( rc )
    {
    case ENOSYS:
        throw ThreadSysError( rc, "The function pthread_setschedparam() is not supported by this implementation." );
        break;
    case EINVAL:
        throw ThreadSysError( rc, FormatA( "The value specified by policy or one of the scheduling parameters associated with the scheduling policy policy is invalid (policy=%d, priority=%d).", policy, priority ) );
        break;
    case ENOTSUP:
        throw ThreadSysError( rc, FormatA( "An attempt was made to set the policy or scheduling parameters to an unsupported value (policy=%d, priority=%d).", policy, priority ) );
        break;
    case EPERM:
        throw ThreadSysError( rc, FormatA( "The caller does not have the appropriate permission to set either the scheduling parameters or the scheduling policy of the specified thread (policy=%d, priority=%d).", policy, priority ) );
        break;
    //case :
        //The implementation does not allow the application to modify one of the parameters to the value specified.
    //    break;
    case ESRCH:
        throw ThreadSysError( rc, FormatA( "The value specified by thread does not refer to a existing thread (policy=%d, priority=%d).", policy, priority ) );
        break;
    }
}

void Thread::getSchedParam( SchedulePolicy * policy, int * priority ) const
{
    sched_param param;
    int pol;
    int rc = pthread_getschedparam( _threadId._self->thread, &pol, &param );
    *policy = (SchedulePolicy)pol;
    *priority = param.sched_priority;
    switch ( rc )
    {
    case ENOSYS:
        throw ThreadSysError( rc, "The function pthread_getschedparam() is not supported by this implementation." );
        break;
    case ESRCH:
        throw ThreadSysError( rc, "The value specified by thread does not refer to a existing thread." );
        break;
    }
}

ThreadAttr & Thread::attr()
{
    if ( !_attr ) _attr.create();
    return _attr;
}

void Thread::run()
{
    // 从Runable接口运行线程处理
    if ( this->_runable )
    {
        this->_runable->run();
    }
}

// struct MutexAttr_Data ======================================================================
struct MutexAttr_Data
{
    pthread_mutexattr_t * pMutexAttr;
    pthread_mutexattr_t mutexAttr;

    MutexAttr_Data() : pMutexAttr(NULL)
    {
    }
};

// class MutexAttr ----------------------------------------------------------------------------
MutexAttr::MutexAttr( bool isCreate )
{
    if ( isCreate )
    {
        this->create();
    }
}

MutexAttr::~MutexAttr()
{
    this->destroy();
}

MutexAttr::MutexAttr( MutexAttr && other ) : _self( std::move(other._self) )
{
}

MutexAttr & MutexAttr::operator = ( MutexAttr && other )
{
    if ( this != &other )
    {
        this->destroy(); // 销毁自身

        _self = std::move(other._self);
    }
    return *this;
}

int MutexAttr::create()
{
    this->destroy();
    int rc = pthread_mutexattr_init(&_self->mutexAttr);
    if ( rc == 0 )
    {
        _self->pMutexAttr = &_self->mutexAttr;
    }
    return rc;
}

int MutexAttr::destroy()
{
    if ( _self && _self->pMutexAttr != NULL )
    {
        int rc = pthread_mutexattr_destroy(_self->pMutexAttr);
        if ( rc == 0 )
        {
            _self->pMutexAttr = NULL;
        }
        return rc;
    }
    return -1;
}

MutexAttr::operator bool() const
{
    return _self->pMutexAttr != NULL;
}

int MutexAttr::setType( MutexType type )
{
    int rc = 0;
    switch ( type )
    {
    case mtxNormal:
        rc = pthread_mutexattr_settype( _self->pMutexAttr, PTHREAD_MUTEX_NORMAL );
        break;
    case mtxRecursive:
        rc = pthread_mutexattr_settype( _self->pMutexAttr, PTHREAD_MUTEX_RECURSIVE );
        break;
    case mtxErrorCheck:
        rc = pthread_mutexattr_settype( _self->pMutexAttr, PTHREAD_MUTEX_ERRORCHECK );
        break;
    case mtxDefault:
        rc = pthread_mutexattr_settype( _self->pMutexAttr, PTHREAD_MUTEX_DEFAULT );
        break;
    default:
        rc = EINVAL;
        break;
    }
    return rc;
}

MutexAttr::MutexType MutexAttr::getType() const
{
    MutexType type = mtxDefault;
    int itype = 0;
    int rc = pthread_mutexattr_gettype( _self->pMutexAttr, &itype );
    if ( rc == 0 )
    {
        switch ( itype )
        {
        case PTHREAD_MUTEX_NORMAL:
            type = mtxNormal;
            break;
        case PTHREAD_MUTEX_RECURSIVE:
            type = mtxRecursive;
            break;
        case PTHREAD_MUTEX_ERRORCHECK:
            type = mtxErrorCheck;
            break;
        }
    }
    return type;
}

// struct Mutex_Data ==========================================================================
struct Mutex_Data
{
    pthread_mutex_t * pMutex;
    pthread_mutex_t mutex;

    Mutex_Data() : pMutex(NULL)
    {
    }
};

// class Mutex --------------------------------------------------------------------------------
Mutex::Mutex( bool isCreate ) : _attr(false)
{
    if ( isCreate )
    {
        this->create();
    }
}

Mutex::~Mutex()
{
    this->destroy();
}

Mutex::Mutex( Mutex && other ) : _attr( std::move(other._attr) ), _self( std::move(other._self) )
{
}

Mutex & Mutex::operator = ( Mutex && other )
{
    if ( this != &other )
    {
        this->destroy(); // 销毁自身

        _attr = std::move(other._attr);
        _self = std::move(other._self);
    }
    return *this;
}

int Mutex::create()
{
    this->destroy();
    int rc = pthread_mutex_init( &_self->mutex, _attr._self->pMutexAttr );
    if ( rc == 0 )
    {
        _self->pMutex = &_self->mutex;
    }
    return rc;
}

int Mutex::destroy()
{
    if ( _self && _self->pMutex != NULL )
    {
        int rc = pthread_mutex_destroy(_self->pMutex);
        if ( rc == 0 )
        {
            _self->pMutex = NULL;
        }
        return rc;
    }
    return -1;
}

bool Mutex::lock()
{
    int rc = pthread_mutex_lock(_self->pMutex);
    return rc == 0;
}

bool Mutex::tryLock()
{
    int rc = pthread_mutex_trylock(_self->pMutex);
    if ( rc == 0 )
    {
        return true;
    }
    switch ( rc )
    {
    case EBUSY:
        return false;
    default:
        return false;
    }
}

bool Mutex::unlock()
{
    int rc = pthread_mutex_unlock(_self->pMutex);
    return rc == 0;
}

MutexAttr & Mutex::attr()
{
    if ( !_attr ) _attr.create();
    return _attr;
}

// class RecursiveMutex -----------------------------------------------------------------------
RecursiveMutex::RecursiveMutex( bool isCreate ) : Mutex(false)
{
    if ( isCreate )
    {
        this->create();
    }
}

int RecursiveMutex::create()
{
    this->attr().setType(MutexAttr::mtxRecursive);
    int rc = Mutex::create();
    this->attr().destroy();
    return rc;
}

// ConditionAttr_Data =========================================================================
struct ConditionAttr_Data
{
    pthread_condattr_t * pCondAttr;
    pthread_condattr_t condAttr;

    ConditionAttr_Data() : pCondAttr(NULL)
    {
    }

};

// class Condition ----------------------------------------------------------------------------
ConditionAttr::ConditionAttr( bool isCreate )
{
    if ( isCreate )
    {
        this->create();
    }
}

ConditionAttr::~ConditionAttr()
{
    this->destroy();
}

ConditionAttr::ConditionAttr( ConditionAttr && other ) : _self( std::move(other._self) )
{
}

ConditionAttr & ConditionAttr::operator = ( ConditionAttr && other )
{
    if ( this != &other )
    {
        this->destroy(); // 销毁自身

        _self = std::move(other._self);
    }
    return *this;
}

int ConditionAttr::create()
{
    this->destroy();
    int rc = pthread_condattr_init(&_self->condAttr);
    if ( rc == 0 )
    {
        _self->pCondAttr = &_self->condAttr;
    }
    return rc;
}

int ConditionAttr::destroy()
{
    if ( _self && _self->pCondAttr != NULL )
    {
        int rc = pthread_condattr_destroy(_self->pCondAttr);
        if ( rc == 0 )
        {
            _self->pCondAttr = NULL;
        }
        return rc;
    }
    return -1;
}

ConditionAttr::operator bool() const
{
    return _self->pCondAttr != NULL;
}

// struct Condition_Data ======================================================================
struct Condition_Data
{
    pthread_cond_t * pCond;
    pthread_cond_t cond;

    Condition_Data() : pCond(NULL)
    {
    }
};

// class Condition ----------------------------------------------------------------------------
Condition::Condition( bool isCreate ) : _attr(false)
{
    if ( isCreate )
    {
        this->create();
    }
}

Condition::~Condition()
{
    this->destroy();
}

Condition::Condition( Condition && other ) : _attr( std::move(other._attr) ), _self( std::move(other._self) )
{
}

Condition & Condition::operator = ( Condition && other )
{
    if ( this != &other )
    {
        this->destroy(); // 销毁自身

        _attr = std::move(other._attr);
        _self = std::move(other._self);
    }
    return *this;
}

int Condition::create()
{
    this->destroy();
    int rc = pthread_cond_init( &_self->cond, _attr._self->pCondAttr );
    if ( rc == 0 )
    {
        _self->pCond = &_self->cond;
    }
    return rc;
}

int Condition::destroy()
{
    if ( _self && _self->pCond != NULL )
    {
        int rc = pthread_cond_destroy(&_self->cond);
        if ( rc == 0 )
        {
            _self->pCond = NULL;
        }
        return rc;
    }
    return -1;
}

bool Condition::wait( Mutex & mutex, double sec )
{
    int rc;
    if ( sec < 0 )
    {
        rc = pthread_cond_wait( _self->pCond, mutex._self->pMutex );
    }
    else
    {
        timespec ts;
        winux::uint64 us = GetUtcTimeUs();
        us += (winux::uint64)( ( sec * 1e6 ) + 0.5 );

        ts.tv_sec = us / 1000000;
        ts.tv_nsec = ( us % 1000000 ) * 1000;
        rc = pthread_cond_timedwait( _self->pCond, mutex._self->pMutex, &ts );
    }
    if ( rc )
    {
        return false;
        /*switch ( rc )
        {
        case ETIMEDOUT:
            return false;
            break;
        default:
            throw ThreadSysError( rc, "" );
        }//*/
    }

    return true;
}

int Condition::notify()
{
    int rc = pthread_cond_signal(_self->pCond);
    return rc;
}

int Condition::notifyAll()
{
    int rc = pthread_cond_broadcast(_self->pCond);
    return rc;
}

ConditionAttr & Condition::attr()
{
    if ( !_attr ) _attr.create();
    return _attr;
}

// struct TlsKey_Data =========================================================================
struct TlsKey_Data
{
    pthread_key_t _key;

    TlsKey_Data() : _key(0)
    {
    }
};

// class TlsKey -------------------------------------------------------------------------------
TlsKey::TlsKey( void (*destructor)( void *pv ) )
{
    this->create(destructor);
}

TlsKey::~TlsKey()
{
    this->destroy();
}

TlsKey::TlsKey( TlsKey && other ) : _self( std::move(other._self) )
{
}

TlsKey & TlsKey::operator = ( TlsKey && other )
{
    if ( this != &other )
    {
        this->destroy(); // 先销毁自己

        _self = std::move(other._self);
    }
    return *this;
}

int TlsKey::create( void (*destructor)( void *pv ) )
{
    this->destroy();

    int rc = pthread_key_create( &_self->_key, destructor );
    return rc;
}

int TlsKey::destroy()
{
    if ( _self && _self->_key )
    {
        int rc = pthread_key_delete(_self->_key);
        if ( rc == 0 )
        {
            _self->_key = 0;
        }
        return rc;
    }
    return -1;
}

void * TlsKey::get() const
{
    return reinterpret_cast<void*>(_self->_key);
}

// class TlsVar -------------------------------------------------------------------------------
TlsVar::TlsVar( TlsKey & tlsKey ) : _tlsKey(&tlsKey)
{
}

TlsVar::~TlsVar()
{
}

TlsVar::TlsVar( TlsVar && other ) : _tlsKey( std::move(other._tlsKey) )
{
}

TlsVar & TlsVar::operator = ( TlsVar && other )
{
    if ( this != &other )
    {
        _tlsKey = std::move(other._tlsKey);
    }
    return *this;
}

void * TlsVar::get()
{
    return pthread_getspecific(_tlsKey->_self->_key);
}

void * TlsVar::get() const
{
    return pthread_getspecific(_tlsKey->_self->_key);
}

void TlsVar::set( void * v )
{
    pthread_setspecific( _tlsKey->_self->_key, v );
}

// class ThreadGroup --------------------------------------------------------------------------
// 线程组重定义了线程处理函数
void * ThreadGroup::_ThreadGroupDefaultFunc( void * param )
{
    Thread * threadObj = reinterpret_cast<Thread*>(param);
    ThreadGroup * groupObj = threadObj->_group;

    threadObj->run();
    void * exitVal = threadObj->getExitVal();

    // 设置运行状态为false
    if ( groupObj )
    {
        ScopeGuard guard(groupObj->_mtxGroup);
        threadObj->_isRunning = false;
        // 通知等待的外部线程
        groupObj->_cdtGroup.notifyAll();
    }
    else
    {
        threadObj->_isRunning = false;
    }

    return exitVal;
}


ThreadGroup & ThreadGroup::destroy()
{
    _threads.clear();
    return *this;
}

ThreadGroup & ThreadGroup::startup()
{
    for ( size_t i = 0; i < _threads.size(); ++i )
    {
        ScopeGuard guard(_mtxGroup);
        // 设置运行状态为true
        _threads[i]->_isRunning = true;
        _threads[i]->startupEx( ThreadGroup::_ThreadGroupDefaultFunc, _threads[i].get() );
        _threads[i]->detach();
    }
    return *this;
}

bool ThreadGroup::wait( double sec )
{
    // 检测所有线程运行状态为false
    ScopeGuard guard(_mtxGroup);
    return _cdtGroup.waitUntil( [this] () -> bool {
        for ( auto & th : this->_threads )
            if ( th->_isRunning )
                return false;
        return true;
    }, _mtxGroup, sec );
}


} // namespace winux
