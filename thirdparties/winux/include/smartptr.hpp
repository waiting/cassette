#ifndef __SMARTPTR_HPP__
#define __SMARTPTR_HPP__
//
// smartptr 提供智能指针相关的功能
//

namespace winux
{
/** \brief 智能指针错误 */
class SmartPtrError : public Error
{
public:
    enum SmartPtrErrorType
    {
        errNone,
        errBadSimpleHandle,
        errBadSimplePointer,
        errBadSharedHandle,
        errBadSharedPointer,
        errBadWeakHandle,
        errBadWeakPointer
    };

    SmartPtrError( SmartPtrErrorType err, AnsiString const & errStr ) : Error( err, errStr ) { }
};

/** \brief Simple删除器场景基类 */
class WINUX_DLL SimpleDeleterContext
{
protected:
    SimpleDeleterContext() { }
    virtual ~SimpleDeleterContext() { }

public:
    /** \brief 销毁资源和`SimpleDeleterContext`自身 */
    void release()
    {
        this->_destroy();
        this->_deleteThis();
    }

    /** \brief 显式删除`SimpleHandle#detach()`出来的`SimpleDeleterContext`对象 */
    void delThis() { this->_deleteThis(); }

protected:
    /** \brief 销毁资源 */
    virtual void _destroy() = 0;
    /** \brief 销毁`SimpleDeleterContext`自身 */
    virtual void _deleteThis() = 0;
};

/** \brief Simple默认删除器场景 */
template < typename _HTy >
class SimpleDefaultDeleterContext : public SimpleDeleterContext
{
public:
    SimpleDefaultDeleterContext( _HTy h ) : _h(h) { }

protected:
    virtual void _destroy() override { delete _h; }
    virtual void _deleteThis() override { delete this; }

private:
    _HTy _h;
};

/** \brief Simple自定义删除器场景 */
template < typename _HTy, typename _Dt >
class SimpleCustomDeleterContext : public SimpleDeleterContext
{
public:
    SimpleCustomDeleterContext( _HTy h, _Dt dt ) : _h(h), _dt(dt) { }

protected:
    virtual void _destroy() override { _dt(_h); }
    virtual void _deleteThis() override { delete this; }

private:
    _HTy _h;
    _Dt _dt;
};

/** \brief 简单句柄类，管理各种资源的自动释放，赋值相当于传递管理权。
 *
 *  如 ptr2 = ptr1。ptr1将管理权传递到ptr2，ptr1失效。\n
 *  还能指定一个`failVal`指示资源分配失败后的句柄值，当不等于这个值时才会创建删除器场景释放资源。 */
template < typename _HTy >
class SimpleHandle
{
public:
    /** \brief 句柄类型 */
    typedef _HTy HType;

    /** \brief 句柄数据结构体 */
    struct SimpleHandleData
    {
        _HTy h; //!< 句柄
        SimpleDeleterContext * ctx; //!< 简单删除器场景对象
        SimpleHandleData() : h(0), ctx(0) { }
        SimpleHandleData( _HTy h, SimpleDeleterContext * ctx ) : h(h), ctx(ctx) { }
    };

    /** \brief 构造函数0 */
    SimpleHandle() { }

    /** \brief 构造函数1_1
     *
     *  \param[in] h 新资源的句柄
     *  \param[in] failVal 分配资源失败的句柄值 */
    SimpleHandle( _HTy h, _HTy failVal ) { attachNew( h, failVal ); }

    /** \brief 构造函数1_2
     *
     *  \param[in] h 新资源的句柄
     *  \param[in] failVal 分配资源失败的句柄值
     *  \param[in] dt 可调用删除器 */
    template < typename _Dt >
    SimpleHandle( _HTy h, _HTy failVal, _Dt dt ) { attachNew( h, failVal, dt ); }

    /** \brief 构造函数2_1。`_HTy2`为可直接转为`_HTy`的类型
     *
     *  \param[in] h 新资源的句柄
     *  \param[in] failVal 分配资源失败的句柄值 */
    template < typename _HTy2 >
    SimpleHandle( _HTy2 h, _HTy2 failVal ) { attachNew( h, failVal ); }

    /** \brief 构造函数2_2。`_HTy2`为可直接转为`_HTy`的类型
     *
     *  \param[in] h 新资源的句柄
     *  \param[in] failVal 分配资源失败的句柄值
     *  \param[in] dt 可调用删除器 */
    template < typename _HTy2, typename _Dt >
    SimpleHandle( _HTy2 h, _HTy2 failVal, _Dt dt ) { attachNew( h, failVal, dt ); }

    /** \brief 析构函数 */
    ~SimpleHandle()
    {
        reset();
    }

    /** \brief 拷贝构造函数1 */
    SimpleHandle( SimpleHandle const & other )
    {
        _reset(other);
    }

    /** \brief 拷贝赋值函数1 */
    SimpleHandle & operator = ( SimpleHandle const & other )
    {
        _reset(other);
        return *this;
    }

    /** \brief 拷贝构造函数2。`_HTy2`为可直接转为`_HTy`的类型 */
    template < typename _HTy2 >
    SimpleHandle( SimpleHandle<_HTy2> const & other )
    {
        _reset(other);
    }

    /** \brief 拷贝赋值函数2。`_HTy2`为可直接转为`_HTy`的类型 */
    template < typename _HTy2 >
    SimpleHandle & operator = ( SimpleHandle<_HTy2> const & other )
    {
        _reset(other);
        return *this;
    }

    /** \brief 附加新句柄，管理新资源
     *
     *  \param[in] h 新资源的句柄
     *  \param[in] failVal 分配资源失败的句柄值 */
    void attachNew( _HTy h, _HTy failVal )
    {
        this->_reset0( h, ( h != failVal ? new SimpleDefaultDeleterContext<_HTy>(h) : 0 ) );
    }

    /** \brief 附加新句柄，管理新资源
     *
     *  \param[in] h 新资源的句柄
     *  \param[in] failVal 分配资源失败的句柄值
     *  \param[in] dt 可调用删除器 */
    template < typename _Dt >
    void attachNew( _HTy h, _HTy failVal, _Dt dt )
    {
        this->_reset0( h, ( h != failVal ? new SimpleCustomDeleterContext< _HTy, _Dt >( h, dt ) : 0 ) );
    }

    /** \brief 附加新句柄，管理新资源。`_HTy2`为可直接转为`_HTy`的类型
     *
     *  \param[in] h 新资源的句柄
     *  \param[in] failVal 分配资源失败的句柄值 */
    template < typename _HTy2 >
    void attachNew( _HTy2 h, _HTy2 failVal )
    {
        this->_reset0( h, ( h != failVal ? new SimpleDefaultDeleterContext<_HTy2>(h) : 0 ) );
    }

    /** \brief 附加新句柄，管理新资源。`_HTy2`为可直接转为`_HTy`的类型
     *
     *  \param[in] h 新资源的句柄
     *  \param[in] failVal 分配资源失败的句柄值
     *  \param[in] dt 可调用删除器 */
    template < typename _HTy2, typename _Dt >
    void attachNew( _HTy2 h, _HTy2 failVal, _Dt dt )
    {
        this->_reset0( h, ( h != failVal ? new SimpleCustomDeleterContext< _HTy2, _Dt >( h, dt ) : 0 ) );
    }

    /** \brief 附加其他`SimpleHandle`对象`detach()`出的句柄数据 */
    void attach( SimpleHandleData const & data )
    {
        _reset0( data.h, data.ctx );
    }

    /** \brief 附加其他`SimpleHandle`对象`detach()`出的句柄数据。`_HTy2`为可直接转为`_HTy`的类型 */
    template < typename _HTy2 >
    void attach( typename SimpleHandle<_HTy2>::SimpleHandleData const & data )
    {
        _reset0( data.h, data.ctx );
    }

    /** \brief 附加其他`SimpleHandle`对象`detach()`出的句柄和删除器场景。`_HTy2`为可直接转为`_HTy`的类型 */
    template < typename _HTy2 >
    void attach( _HTy2 h, SimpleDeleterContext * ctx )
    {
        _reset0( h, ctx );
    }

    /** \brief 资源脱离管理，返回句柄数据 */
    SimpleHandleData detach()
    {
        SimpleHandleData r = _self;
        _self.h = static_cast<_HTy>(0);
        _self.ctx = 0;
        return r;
    }

    /** \brief 释放资源并重置 */
    void reset()
    {
        _reset0( static_cast<_HTy>(0), 0 );
    }

    /** \brief 获取句柄 */
    _HTy get() const { return _self.h; }
    SimpleDeleterContext * getContext() const { return _self.ctx; }

    /** \brief 判断是否在管理资源 */
    operator bool() const { return _self.ctx != 0; }

    /** \brief less，给MAP使用 */
    bool operator < ( SimpleHandle const & other ) const { return _self.h < other._self.h; }

    _HTy operator -> ()
    {
        return _self.h;
    }

    _HTy operator -> () const
    {
        return _self.h;
    }

protected:
    /** \brief 释放自身资源，管理新资源 */
    template < typename _HTy2 >
    void _reset0( _HTy2 newH, SimpleDeleterContext * newCtx )
    {
        if ( _self.ctx )
            _self.ctx->release();
        _self.h = newH;
        _self.ctx = newCtx;
    }

    /** \brief 释放自身资源，接管另一个`SimpleHandle`的资源，另一个`SimpleHandle`置零 */
    template < typename _HTy2 >
    void _reset( _HTy2 & otherH, SimpleDeleterContext * & otherCtx )
    {
        _reset0( otherH, otherCtx );
        otherH = static_cast<_HTy2>(0);
        otherCtx = 0;
    }

    /** \brief 释放自身资源，接管另一个`SimpleHandle`的资源，另一个`SimpleHandle`置零 */
    template < typename _HTy2 >
    void _reset( SimpleHandle<_HTy2> const & other )
    {
        SimpleHandle<_HTy2> & o = const_cast< SimpleHandle<_HTy2> & >(other);
        _reset( o._self.h, o._self.ctx );
    }

    SimpleHandleData _self;

private:
    template < typename _HTy0 >
    friend class SimpleHandle;
};

/** \brief 简单指针 */
template < typename _Ty >
class SimplePointer : public SimpleHandle<_Ty*>
{
public:
    /** \brief 基类`SimpleHandle<_Ty*>` */
    typedef SimpleHandle<_Ty*> MyBase;
    /** \brief 指向的类型 */
    typedef _Ty Type;

    /** \brief 构造函数0 */
    SimplePointer() { }

    /** \brief 构造函数1_1
     *
     *  \param[in] p 新的对象 */
    explicit SimplePointer( _Ty* p ) : MyBase( p, (_Ty*)0 ) { }

    /** \brief 构造函数1_2
     *
     *  \param[in] p 新的对象
     *  \param[in] dt 可调用删除器 */
    template < typename _Dt >
    SimplePointer( _Ty* p, _Dt dt ) : MyBase( p, (_Ty*)0, dt ) { }

    /** \brief 构造函数2_1
     *
     *  \param[in] p 新的对象 */
    template < typename _Ty2 >
    explicit SimplePointer( _Ty2* p ) : MyBase( p, (_Ty2*)0 ) { }

    /** \brief 构造函数2_2
     *
     *  \param[in] p 新的对象
     *  \param[in] dt 可调用删除器 */
    template < typename _Ty2, typename _Dt >
    SimplePointer( _Ty2* p, _Dt dt ) : MyBase( p, (_Ty2*)0, dt ) { }

    /** \brief 拷贝构造函数1 */
    SimplePointer( SimplePointer const & other )
    {
        this->_reset(other);
    }

    /** \brief 拷贝赋值函数1 */
    SimplePointer & operator = ( SimplePointer const & other )
    {
        this->_reset(other);
        return *this;
    }

    /** \brief 拷贝构造函数2 */
    template < typename _Ty2 >
    SimplePointer( SimplePointer<_Ty2> const & other )
    {
        this->_reset(other);
    }

    /** \brief 拷贝赋值函数2 */
    template < typename _Ty2 >
    SimplePointer & operator = ( SimplePointer<_Ty2> const & other )
    {
        this->_reset(other);
        return *this;
    }

    /** \brief 附加新指针，管理新对象
     *
     *  \param[in] p 新对象的指针 */
    void attachNew( _Ty * p )
    {
        MyBase::attachNew( p, (_Ty*)0 );
    }

    /** \brief 附加新指针，管理新对象
     *
     *  \param[in] p 新对象的指针
     *  \param[in] dt 可调用删除器 */
    template < typename _Dt >
    void attachNew( _Ty * p, _Dt dt )
    {
        MyBase::attachNew( p, (_Ty*)0, dt );
    }

    /** \brief 附加新指针，管理新对象
     *
     *  \param[in] p 新对象的指针 */
    template < typename _Ty2 >
    void attachNew( _Ty2 * p )
    {
        MyBase::attachNew( p, (_Ty2*)0 );
    }

    /** \brief 附加新指针，管理新对象
     *
     *  \param[in] p 新对象的指针
     *  \param[in] dt 可调用删除器 */
    template < typename _Ty2, typename _Dt >
    void attachNew( _Ty2 * p, _Dt dt )
    {
        MyBase::attachNew( p, (_Ty2*)0, dt );
    }

    /** \brief 把指针由_Ty转换(dynamic_cast)成_Ty2类型
     *
     *  通常_Ty为基类，_Ty2为派生类，并且_Ty指针所指的是_Ty2类型的对象。\n
     *  如果转换失败，返回空指针，自身改不变。如果转换成功，自身放弃所有权，置零。 */
    template < typename _Ty2 >
    SimplePointer<_Ty2> cast()
    {
        SimplePointer<_Ty2> r;
        typename SimplePointer<_Ty2>::HType p = dynamic_cast< typename SimplePointer<_Ty2>::HType >(this->_self.h);
        if ( p != 0 )
        {
            r._reset( p, this->_self.ctx );
            this->_self.h = static_cast<typename MyBase::HType>(0);
            this->_self.ctx = 0;
        }
        return r;
    }

    /** \brief 把指针由_Ty转换(static_cast)成_Ty2类型
     *
     *  通常_Ty为基类，_Ty2为派生类，并且_Ty指针所指的必须是_Ty2类型的对象，才能使用这个函数。\n
     *  自身放弃所有权，置零。\n
     *  你必须担保转换成功，否则将产生灾难。 */
    template < typename _Ty2 >
    SimplePointer<_Ty2> ensureCast()
    {
        SimplePointer<_Ty2> r;
        typename SimplePointer<_Ty2>::HType p = static_cast< typename SimplePointer<_Ty2>::HType >(this->_self.h);
        r._reset( p, this->_self.ctx );
        this->_self.h = static_cast<typename MyBase::HType>(0);
        this->_self.ctx = 0;
        return r;
    }

    template < typename _Ty0 >
    friend class SimplePointer;
};

///////////////////////////////////////////////////////////////////////////////////////////

/** \brief 原子化使一个Long型变量+1，返回值是+1后的*p值 */
WINUX_FUNC_DECL(long) LongAtomicIncrement( long volatile * p );
/** \brief 原子化使一个Long型变量-1，返回值是-1后的*p值 */
WINUX_FUNC_DECL(long) LongAtomicDecrement( long volatile * p );
/** \brief 原子化操作，*p若和comparand相等，就把*p赋成exchange，返回值是初始的*p值 */
WINUX_FUNC_DECL(long) LongAtomicCompareExchange( long volatile * p,  long exchange, long comparand );

class ILockObj;
/** \brief 原子化使一个Long型变量+1，返回值是+1后的*p值 */
WINUX_FUNC_DECL(long) LongAtomicIncrement( ILockObj & lockObj, long volatile * p );
/** \brief 原子化使一个Long型变量-1，返回值是-1后的*p值 */
WINUX_FUNC_DECL(long) LongAtomicDecrement( ILockObj & lockObj, long volatile * p );
/** \brief 原子化操作，*p若和comparand相等，就把*p赋成exchange，返回值是初始的*p值 */
WINUX_FUNC_DECL(long) LongAtomicCompareExchange( ILockObj & lockObj, long volatile * p,  long exchange, long comparand );

// 前向声明
template < typename _HTy >
class WeakHandle;
template < typename _Ty >
class WeakPointer;
template < typename _Ty >
class SharedPointer;

// For SFINAE
template < typename... _Ty >
struct MakeVoid { typedef void type; };
template < typename... _Ty >
using VoidT = typename MakeVoid<_Ty...>::type;

// Detect unambiguous and accessible inheritance from EnableSharedFromThis
template < typename _Yty, typename = VoidT<> >
struct _CanEnableShared : std::false_type
{
};

// is_convertible is necessary to verify unambiguous inheritance
template < typename _Yty >
struct _CanEnableShared<_Yty, VoidT<typename _Yty::_EsftType> > : std::is_convertible<typename std::remove_cv<_Yty>::type *, typename _Yty::_EsftType *>::type
{
};

// enable sharedFromThis
template < typename _Other, typename _Yty >
void _EnableSharedFromThis1( SharedPointer<_Other> const & sharedThis, _Yty * ptr, std::true_type )
{
    if ( ptr && ptr->_weakPtr.expired() )
    {
        ptr->_weakPtr = SharedPointer<typename std::remove_cv<_Yty>::type>( sharedThis, const_cast<typename std::remove_cv<_Yty>::type *>(ptr) );
    }
}

// don't enable sharedFromThis
template < typename _Other, typename _Yty >
void _EnableSharedFromThis1( SharedPointer<_Other> const &, _Yty *, std::false_type )
{
}

// possibly enable sharedFromThis
template < typename _Other, typename _Yty >
void _EnableSharedFromThis( SharedPointer<_Other> const & sharedThis, _Yty * ptr )
{
    _EnableSharedFromThis1( sharedThis, ptr, std::integral_constant< bool, _CanEnableShared<_Yty>::value >() );
}

/** \brief Shared删除器场景基类/Shared引用计数场景 */
class WINUX_DLL SharedDeleterContext
{
protected:
    SharedDeleterContext() : _uses(1), _weaks(1) { }
    virtual ~SharedDeleterContext() { }

    /** \brief 销毁资源 */
    virtual void _destroy() = 0;
    /** \brief 删除引用计数数据场景 */
    virtual void _deleteThis() = 0;

public:
    /** \brief 显式删除`SharedHandle#detach()`出来的`SharedDeleterContext`对象 */
    void delThis() { this->_deleteThis(); }

    /** \brief 如果引用计数不是0，则增加引用计数。成功则返回true。
     *
     *  用于`WeakHandle`创建`SharedHandle`时检测所持资源的计数是否不为0，即还有效。*/
    bool _incRefNz()
    {
        for ( ; ; )
        {
            // loop until state is known
            long count = _uses.load(std::memory_order_relaxed);
            if ( count == 0 ) return false;
            if ( _uses.compare_exchange_weak( count, count + 1, std::memory_order_acq_rel, std::memory_order_relaxed ) ) return true;
        }
    }
    /** \brief 增加引用计数 */
    void incRef() { _uses.fetch_add( 1, std::memory_order_acq_rel ); }
    /** \brief 减少引用计数。当引用计数为0时销毁资源，并且销毁资源时减少弱引用计数。 */
    void decRef()
    {
        if ( _uses.fetch_sub( 1, std::memory_order_acq_rel ) == 1 )
        {
            std::atomic_thread_fence(std::memory_order_acquire);
            this->_destroy();
            this->decWRef();
        }
    }

    /** \brief 增加弱引用计数 */
    void incWRef() { _weaks.fetch_add( 1, std::memory_order_acq_rel ); }
    /** \brief 减少弱引用计数，当弱引用计数为0时销毁删除器场景对象 */
    void decWRef()
    {
        if ( _weaks.fetch_sub( 1, std::memory_order_acq_rel ) == 1 )
        {
            std::atomic_thread_fence(std::memory_order_acquire);
            this->_deleteThis();
        }
    }

    /** \brief 资源引用计数 */
    long useCount() const { return _uses.load(std::memory_order_relaxed); }

    /** \brief 资源是否已过期 */
    bool expired() const { return _uses.load(std::memory_order_relaxed) == 0; }

    /** \brief 弱引用计数 */
    long weakCount() const { return _weaks.load(std::memory_order_relaxed); }

private:
    std::atomic<long> _uses;
    std::atomic<long> _weaks;

    DISABLE_OBJECT_COPY(SharedDeleterContext)
};

/** \brief Shared默认删除器场景 */
template < typename _HTy >
class SharedDefaultDeleterContext : public SharedDeleterContext
{
public:
    SharedDefaultDeleterContext( _HTy h ) : _h(h) { }

protected:
    virtual void _destroy() override { delete _h; }
    virtual void _deleteThis() override { delete this; }

private:
    _HTy _h;
};

/** \brief Shared自定义删除器场景 */
template < typename _HTy, typename _Dt >
class SharedCustomDeleterContext : public SharedDeleterContext
{
public:
    SharedCustomDeleterContext( _HTy h, _Dt dt ) : _h(h), _dt(dt) { }

protected:
    virtual void _destroy() override { _dt(_h); }
    virtual void _deleteThis() override { delete this; }

private:
    _HTy _h;
    _Dt _dt;
};

/** \brief 引用计数共享句柄，管理各种资源的自动释放，赋值则引用计数加一。
 *
 *  还能指定一个`failVal`指示资源分配失败后的句柄值，当不等于这个值时才会创建删除器场景释放资源 */
template < typename _HTy >
class SharedHandle
{
public:
    /** \brief 句柄类型 */
    typedef _HTy HType;

    /** \brief 句柄数据结构体 */
    struct SharedHandleData
    {
        _HTy h; //!< 句柄
        SharedDeleterContext * ctx; //!< 共享删除器场景对象
        SharedHandleData() : h(0), ctx(0) { }
        SharedHandleData( _HTy h, SharedDeleterContext * ctx ) : h(h), ctx(ctx) { }
    };

    /** \brief 构造函数0 */
    SharedHandle() { }

    /** \brief 构造函数1_1
     *
     *  \param[in] h 新资源的句柄
     *  \param[in] failVal 分配资源失败的句柄值 */
    SharedHandle( _HTy h, _HTy failVal ) { attachNew( h, failVal ); }

    /** \brief 构造函数1_2
     *
     *  \param[in] h 新资源的句柄
     *  \param[in] failVal 分配资源失败的句柄值
     *  \param[in] dt 可调用删除器 */
    template < typename _Dt >
    SharedHandle( _HTy h, _HTy failVal, _Dt dt ) { attachNew( h, failVal, dt ); }

    /** \brief 构造函数2_1。`_HTy2`为可直接转为`_HTy`的类型
     *
     *  \param[in] h 新资源的句柄
     *  \param[in] failVal 分配资源失败的句柄值 */
    template < typename _HTy2 >
    SharedHandle( _HTy2 h, _HTy2 failVal ) { attachNew( h, failVal ); }

    /** \brief 构造函数2_2。`_HTy2`为可直接转为`_HTy`的类型
     *
     *  \param[in] h 新资源的句柄
     *  \param[in] failVal 分配资源失败的句柄值
     *  \param[in] dt 可调用删除器 */
    template < typename _HTy2, typename _Dt >
    SharedHandle( _HTy2 h, _HTy2 failVal, _Dt dt ) { attachNew( h, failVal, dt ); }

    /** \brief 构造函数3。从弱句柄构建共享句柄 */
    template < typename _HTy2 >
    explicit SharedHandle( WeakHandle<_HTy2> const & other )
    {
        if ( !other._sharedReset(this) )
        {
            throw SmartPtrError( SmartPtrError::errBadWeakHandle, AnsiString("Bad WeakHandle<") + typeid(_HTy2).name() + ">" );
        }
    }

    /** \brief 析构函数 */
    ~SharedHandle()
    {
        reset();
    }

    /** \brief 拷贝构造函数1 */
    SharedHandle( SharedHandle const & other )
    {
        _reset(other);
    }

    /** \brief 拷贝赋值函数1 */
    SharedHandle & operator = ( SharedHandle const & other )
    {
        _reset(other);
        return *this;
    }

    /** \brief 拷贝构造函数2。`_HTy2`为可直接转为`_HTy`的类型 */
    template < typename _HTy2 >
    SharedHandle( SharedHandle<_HTy2> const & other )
    {
        _reset(other);
    }

    /** \brief 拷贝赋值函数2。`_HTy2`为可直接转为`_HTy`的类型 */
    template < typename _HTy2 >
    SharedHandle & operator = ( SharedHandle<_HTy2> const & other )
    {
        _reset(other);
        return *this;
    }

    /** \brief 附加新句柄，管理新资源
     *
     *  \param[in] h 新资源的句柄
     *  \param[in] failVal 分配资源失败的句柄值 */
    void attachNew( _HTy h, _HTy failVal )
    {
        this->_reset0( h, ( h != failVal ? new SharedDefaultDeleterContext<_HTy>(h) : 0 ) );
    }

    /** \brief 附加新句柄，管理新资源
     *
     *  \param[in] h 新资源的句柄
     *  \param[in] failVal 分配资源失败的句柄值
     *  \param[in] dt 可调用删除器 */
    template < typename _Dt >
    void attachNew( _HTy h, _HTy failVal, _Dt dt )
    {
        this->_reset0( h, ( h != failVal ? new SharedCustomDeleterContext< _HTy, _Dt >( h, dt ) : 0 ) );
    }

    /** \brief 附加新句柄，管理新资源。`_HTy2`为可直接转为`_HTy`的类型
     *
     *  \param[in] h 新资源的句柄
     *  \param[in] failVal 分配资源失败的句柄值 */
    template < typename _HTy2 >
    void attachNew( _HTy2 h, _HTy2 failVal )
    {
        this->_reset0( h, ( h != failVal ? new SharedDefaultDeleterContext<_HTy2>(h) : 0 ) );
    }

    /** \brief 附加新句柄，管理新资源。`_HTy2`为可直接转为`_HTy`的类型
     *
     *  \param[in] h 新资源的句柄
     *  \param[in] failVal 分配资源失败的句柄值
     *  \param[in] dt 可调用删除器 */
    template < typename _HTy2, typename _Dt >
    void attachNew( _HTy2 h, _HTy2 failVal, _Dt dt )
    {
        this->_reset0( h, ( h != failVal ? new SharedCustomDeleterContext< _HTy2, _Dt >( h, dt ) : 0 ) );
    }

    /** \brief 附加其他`SharedHandle`对象`detach()`出的句柄数据
     *
     *  若资源是新建而来，则`isIncRef`应设为false；\n
     *  若资源是从另一个`SharedHandle#detach()`而来，则`isIncRef`应设为false；\n
     *  若资源是从另一个`SharedHandle`窥视而来，则`isIncRef`应设为true。 */
    void attach( SharedHandleData const & data, bool isIncRef )
    {
        if ( isIncRef )
        {
            _reset( data.h, data.ctx );
        }
        else
        {
            _reset0( data.h, data.ctx );
        }
    }

    /** \brief 附加其他`SharedHandle`对象`detach()`出的句柄数据。`_HTy2`为可直接转为`_HTy`的类型
     *
     *  若资源是新建而来，则`isIncRef`应设为false；\n
     *  若资源是从另一个`SharedHandle#detach()`而来，则`isIncRef`应设为false；\n
     *  若资源是从另一个`SharedHandle`窥视而来，则`isIncRef`应设为true。 */
    template < typename _HTy2 >
    void attach( typename SharedHandle<_HTy2>::SharedHandleData const & data, bool isIncRef )
    {
        if ( isIncRef )
        {
            _reset( data.h, data.ctx );
        }
        else
        {
            _reset0( data.h, data.ctx );
        }
    }

    /** \brief 附加其他`SharedHandle`对象`detach()`出的句柄和删除器场景。`_HTy2`为可直接转为`_HTy`的类型
     *
     *  若资源是新建而来，则`isIncRef`应设为false；\n
     *  若资源是从另一个`SharedHandle#detach()`而来，则`isIncRef`应设为false；\n
     *  若资源是从另一个`SharedHandle`窥视而来，则`isIncRef`应设为true。 */
    template < typename _HTy2 >
    void attach( _HTy2 h, SharedDeleterContext * ctx, bool isIncRef )
    {
        if ( isIncRef )
        {
            _reset( h, ctx );
        }
        else
        {
            _reset0( h, ctx );
        }
    }

    /** \brief 资源脱离管理，返回句柄数据 */
    SharedHandleData detach()
    {
        SharedHandleData r = _self;
        _self.h = static_cast<_HTy>(0);
        _self.ctx = 0;
        return r;
    }

    /** \brief 窥视句柄数据 */
    SharedHandleData peek() const
    {
        return _self;
    }

    /** \brief 释放资源并重置 */
    void reset()
    {
        _reset0( static_cast<_HTy>(0), 0 );
    }

    /** \brief 获取句柄 */
    _HTy get() const { return _self.h; }
    SharedDeleterContext * getContext() const { return _self.ctx; }

    /** \brief 判断是否在管理资源 */
    operator bool() const { return _self.ctx != 0; }

    /** \brief less，给MAP使用 */
    bool operator < ( SharedHandle const & other ) const { return _self.h < other._self.h; }

    _HTy operator -> ()
    {
        return _self.h;
    }

    _HTy operator -> () const
    {
        return _self.h;
    }

protected:
    /** \brief 减少自身引用计数，管理新资源 */
    template < typename _HTy2 >
    void _reset0( _HTy2 newH, SharedDeleterContext * newCtx )
    {
        if ( _self.ctx )
            _self.ctx->decRef();
        _self.h = newH;
        _self.ctx = newCtx;
    }

    /** \brief 增加另一个`SharedHandle`的资源引用计数，减少自身计数。管理另一个`SharedHandle`的资源 */
    template < typename _HTy2 >
    void _reset( _HTy2 otherH, SharedDeleterContext * otherCtx )
    {
        if ( otherCtx )
            otherCtx->incRef();
        _reset0( otherH, otherCtx );
    }

    /** \brief 增加另一个`SharedHandle`的资源引用计数，减少自身计数。管理另一个`SharedHandle`的资源 */
    template < typename _HTy2 >
    void _reset( SharedHandle<_HTy2> const & other )
    {
        _reset( other._self.h, other._self.ctx );
    }

    SharedHandleData _self;

private:
    template < typename _HTy0 >
    friend class SharedHandle;
    template < typename _HTy0 >
    friend class WeakHandle;
};

/** \brief 引用计数共享指针 */
template < typename _Ty >
class SharedPointer : public SharedHandle<_Ty*>
{
public:
    /** \brief 基类`SharedHandle<_Ty*>` */
    typedef SharedHandle<_Ty*> MyBase;
    /** \brief 指向的类型 */
    typedef _Ty Type;

    /** \brief 构造函数0 */
    SharedPointer() { }

    /** \brief 构造函数1_1
     *
     *  \param[in] p 新的对象 */
    explicit SharedPointer( _Ty* p ) { attachNew(p); }

    /** \brief 构造函数1_2
     *
     *  \param[in] p 新的对象
     *  \param[in] dt 可调用删除器 */
    template < typename _Dt >
    SharedPointer( _Ty* p, _Dt dt ) { attachNew( p, dt ); }

    /** \brief 构造函数2_1
     *
     *  \param[in] p 新的对象 */
    template < typename _Ty2 >
    explicit SharedPointer( _Ty2* p ) { attachNew(p); }

    /** \brief 构造函数2_2
     *
     *  \param[in] p 新的对象
     *  \param[in] dt 可调用删除器 */
    template < typename _Ty2, typename _Dt >
    SharedPointer( _Ty2* p, _Dt dt ) { attachNew( p, dt ); }

    /** \brief 构造函数3。从弱指针构建共享指针 */
    template < typename _Ty2 >
    explicit SharedPointer( WeakPointer<_Ty2> const & other )
    {
        if ( !other._sharedReset(this) )
        {
            throw SmartPtrError( SmartPtrError::errBadWeakPointer, AnsiString("Bad WeakPointer<") + typeid(_Ty2).name() + ">" );
        }
    }

    /** \brief 构造函数4。别名构造 */
    template < typename _Ty2 >
    SharedPointer( SharedPointer<_Ty2> const & other, _Ty2 * ptr ) noexcept
    {
        if ( other._self.ctx )
            other._self.ctx->incRef();

        this->_self.h = ptr;
        this->_self.ctx = other._self.ctx;
    }

    /** \brief 拷贝构造函数1 */
    SharedPointer( SharedPointer const & other )
    {
        this->_reset(other);
    }

    /** \brief 拷贝赋值函数1 */
    SharedPointer & operator = ( SharedPointer const & other )
    {
        this->_reset(other);
        return *this;
    }

    /** \brief 拷贝构造函数2 */
    template < typename _Ty2 >
    SharedPointer( SharedPointer<_Ty2> const & other )
    {
        this->_reset(other);
    }

    /** \brief 拷贝赋值函数2 */
    template < typename _Ty2 >
    SharedPointer & operator = ( SharedPointer<_Ty2> const & other )
    {
        this->_reset(other);
        return *this;
    }

    /** \brief 附加新指针，管理新对象
     *
     *  \param[in] p 新对象的指针 */
    void attachNew( _Ty * p )
    {
        MyBase::attachNew( p, (_Ty*)0 );
        _EnableSharedFromThis( *this, p );
    }

    /** \brief 附加新指针，管理新对象
     *
     *  \param[in] p 新对象的指针
     *  \param[in] dt 可调用删除器 */
    template < typename _Dt >
    void attachNew( _Ty * p, _Dt dt )
    {
        MyBase::attachNew( p, (_Ty*)0, dt );
        _EnableSharedFromThis( *this, p );
    }

    /** \brief 附加新指针，管理新对象
     *
     *  \param[in] p 新对象的指针 */
    template < typename _Ty2 >
    void attachNew( _Ty2 * p )
    {
        MyBase::attachNew( p, (_Ty2*)0 );
        _EnableSharedFromThis( *this, p );
    }

    /** \brief 附加新指针，管理新对象
     *
     *  \param[in] p 新对象的指针
     *  \param[in] dt 可调用删除器 */
    template < typename _Ty2, typename _Dt >
    void attachNew( _Ty2 * p, _Dt dt )
    {
        MyBase::attachNew( p, (_Ty2*)0, dt );
        _EnableSharedFromThis( *this, p );
    }

    /** \brief 把指针由_Ty转换(dynamic_cast)成_Ty2类型
     *
     *  通常_Ty为基类，_Ty2为派生类，并且_Ty指针所指的是_Ty2类型的对象。\n
     *  如果转换失败，返回空指针。如果转换成功，引用计数增加。 */
    template < typename _Ty2 >
    SharedPointer<_Ty2> cast()
    {
        SharedPointer<_Ty2> r;
        typename SharedPointer<_Ty2>::HType p = dynamic_cast< typename SharedPointer<_Ty2>::HType >(this->_self.h);
        if ( p != 0 )
            r._reset( p, this->_self.ctx );
        return r;
    }

    /** \brief 把指针由_Ty转换(static_cast)成_Ty2类型
     *
     *  通常_Ty为基类，_Ty2为派生类，并且_Ty指针所指的必须是_Ty2类型的对象，才能使用这个函数。\n
     *  你必须担保转换成功，否则将产生灾难。 */
    template < typename _Ty2 >
    SharedPointer<_Ty2> ensureCast()
    {
        SharedPointer<_Ty2> r;
        r._reset( static_cast< typename SharedPointer<_Ty2>::HType >(this->_self.h), this->_self.ctx );
        return r;
    }

private:
    template < typename _Ty0 >
    friend class SharedPointer;
    template < typename _Ty0 >
    friend class WeakPointer;
};

/** \brief 弱句柄 */
template < typename _HTy >
class WeakHandle
{
public:
    /** \brief 句柄类型 */
    typedef _HTy HType;

    /** \brief 句柄数据结构体 */
    typedef typename SharedHandle<_HTy>::SharedHandleData WeakHandleData;

    /** \brief 构造函数0 */
    WeakHandle() { }

    /** \brief 构造函数1，从一个`SharedHandle`构建一个`WeakHandle` */
    template < typename _HTy2 >
    WeakHandle( SharedHandle<_HTy2> const & other )
    {
        _reset( other._self.h, other._self.ctx );
    }

    /** \brief 赋值函数1，从一个`SharedHandle`构建一个`WeakHandle` */
    template < typename _HTy2 >
    WeakHandle & operator = ( SharedHandle<_HTy2> const & other )
    {
        _reset( other._self.h, other._self.ctx );
        return *this;
    }

    /** \brief 析构函数 */
    ~WeakHandle()
    {
        reset();
    }

    /** \brief 拷贝构造函数1 */
    WeakHandle( WeakHandle const & other )
    {
        _reset(other);
    }

    /** \brief 拷贝赋值函数1 */
    WeakHandle & operator = ( WeakHandle const & other )
    {
        _reset(other);
        return *this;
    }

    /** \brief 拷贝构造函数2。`_HTy2`为可直接转为`_HTy`的类型 */
    template < typename _HTy2 >
    WeakHandle( WeakHandle<_HTy2> const & other )
    {
        _reset(other);
    }

    /** \brief 拷贝赋值函数2。`_HTy2`为可直接转为`_HTy`的类型 */
    template < typename _HTy2 >
    WeakHandle & operator = ( WeakHandle<_HTy2> const & other )
    {
        _reset(other);
        return *this;
    }

    /** \brief 释放并重置 */
    void reset()
    {
        _reset( static_cast<_HTy>(0), 0 );
    }

    /** \brief 锁定共享引用计数场景，创建一个`SharedHandle`持住资源 */
    SharedHandle<_HTy> lock() const
    {
        SharedHandle<_HTy> r;
        (void)this->_sharedReset(&r);
        return r;
    }

    /** \brief 是否过期
     *
     *  true表示已过期，false表示尚未过期 */
    bool expired() const { return ( !_self.ctx || _self.ctx->expired() ); }

    /** \brief 弱句柄是否有效 */
    operator bool() const { return _self.ctx != 0; }

protected:
    /** \brief 调用`SharedHandle#_reset0()`。用于从`WeakHandle`创建`SharedHandle` */
    template < typename _HTy2 >
    bool _sharedReset( SharedHandle<_HTy2> * shared ) const
    {
        if ( _self.ctx && _self.ctx->_incRefNz() )
        {
            // 由于之前_incRefNz()已经增加引用计数，因此这里当作新资源看待
            shared->_reset0( _self.h, _self.ctx );
            return true;
        }
        return false;
    }

    /** \brief 增加另一个弱引用计数，减少自身弱计数。管理另一个`WeakHandle` */
    template < typename _HTy2 >
    void _reset( _HTy2 otherH, SharedDeleterContext * otherCtx )
    {
        if ( otherCtx != 0 )
            otherCtx->incWRef();
        if ( _self.ctx != 0 )
            _self.ctx->decWRef();
        _self.h = otherH;
        _self.ctx = otherCtx;
    }

    /** \brief 增加另一个弱引用计数，减少自身弱计数。管理另一个`WeakHandle` */
    template < typename _HTy2 >
    void _reset( WeakHandle<_HTy2> const & other )
    {
        _reset( other._self.h, other._self.ctx );
    }

    WeakHandleData _self;

private:
    template < typename _HTy0 >
    friend class WeakHandle;
    template < typename _HTy0 >
    friend class SharedHandle;
};

/** \brief 弱指针 */
template < typename _Ty >
class WeakPointer : public WeakHandle<_Ty*>
{
public:
    /** \brief 基类`WeakHandle<_Ty*>` */
    typedef WeakHandle<_Ty*> MyBase;
    /** \brief 指向的类型 */
    typedef _Ty Type;

    /** \brief 构造函数0 */
    WeakPointer() { }

    /** \brief 构造函数1，从一个`SharedPointer`构建一个`WeakPointer` */
    template < typename _Ty2 >
    WeakPointer( SharedPointer<_Ty2> const & other )
    {
        this->_reset( other._self.h, other._self.ctx );
    }

    /** \brief 赋值函数1，从一个`SharedPointer`构建一个`WeakPointer` */
    template < typename _Ty2 >
    WeakPointer & operator = ( SharedPointer<_Ty2> const & other )
    {
        this->_reset( other._self.h, other._self.ctx );
        return *this;
    }

    /** \brief 拷贝构造函数1 */
    WeakPointer( WeakPointer const & other )
    {
        this->_reset(other);
    }

    /** \brief 拷贝赋值函数1 */
    WeakPointer & operator = ( WeakPointer const & other )
    {
        this->_reset(other);
        return *this;
    }

    /** \brief 拷贝构造函数2。`_HTy2`为可直接转为`_HTy`的类型 */
    template < typename _Ty2 >
    WeakPointer( WeakPointer<_Ty2> const & other )
    {
        this->_reset(other);
    }

    /** \brief 拷贝赋值函数2。`_HTy2`为可直接转为`_HTy`的类型 */
    template < typename _Ty2 >
    WeakPointer & operator = ( WeakPointer<_Ty2> const & other )
    {
        this->_reset(other);
        return *this;
    }

    /** \brief 锁定共享引用计数场景，创建一个`SharedPointer`持住资源 */
    SharedPointer<_Ty> lock() const
    {
        SharedPointer<_Ty> r;
        (void)this->_sharedReset(&r);
        return r;
    }

private:
    template < typename _Ty0 >
    friend class WeakPointer;
    template < typename _Ty0 >
    friend class SharedPointer;
};

/** \brief 能从this指针获得`SharedPointer` */
template < typename _Ty >
class EnableSharedFromThis
{
public:
    using _EsftType = EnableSharedFromThis;

    SharedPointer<_Ty> sharedFromThis()
    {   // return shared_ptr
        return SharedPointer<_Ty>(_weakPtr);
    }

    SharedPointer<const _Ty> sharedFromThis() const
    {   // return shared_ptr
        return SharedPointer<const _Ty>(_weakPtr);
    }

    WeakPointer<_Ty> weakFromThis() noexcept
    {   // return weak_ptr
        return _weakPtr;
    }

    WeakPointer<const _Ty> weakFromThis() const noexcept
    {   // return weak_ptr
        return _weakPtr;
    }

protected:
    constexpr EnableSharedFromThis() noexcept : _weakPtr()
    {   // construct
    }

    EnableSharedFromThis( EnableSharedFromThis const & ) noexcept : _weakPtr()
    {   // construct (must value-initialize _Wptr)
    }

    EnableSharedFromThis& operator = ( EnableSharedFromThis const & ) noexcept
    {   // assign (must not change _Wptr)
        return (*this);
    }

private:
    template < typename _Other, typename _Yty >
    friend void _EnableSharedFromThis1( SharedPointer<_Other> const & sharedThis, _Yty * ptr, std::true_type );

    mutable WeakPointer<_Ty> _weakPtr;
};

/** \brief 创建一个`SimplePointer`来管理新对象资源 */
template < typename _Ty >
inline SimplePointer<_Ty> MakeSimple( _Ty * newObj )
{
    return SimplePointer<_Ty>(newObj);
}

/** \brief 创建一个`SimplePointer`来管理新对象资源，自定义可调用删除器 */
template < typename _Ty, typename _Dt >
inline SimplePointer<_Ty> MakeSimple( _Ty * newObj, _Dt dt )
{
    return SimplePointer<_Ty>( newObj, dt );
}

/** \brief 创建一个`SimplePointer`来管理新对象资源 */
template < typename _Ty, typename... _ArgType >
inline SimplePointer<_Ty> MakeSimpleNew( _ArgType&& ... arg )
{
    return SimplePointer<_Ty>( new _Ty( std::forward<_ArgType>(arg)... ) );
}

/** \brief 创建一个`SharedPointer`来管理新对象资源 */
template < typename _Ty >
inline SharedPointer<_Ty> MakeShared( _Ty * newObj )
{
    return SharedPointer<_Ty>(newObj);
}

/** \brief 创建一个`SharedPointer`来管理新对象资源，自定义可调用删除器 */
template < typename _Ty, typename _Dt >
inline SharedPointer<_Ty> MakeShared( _Ty * newObj, _Dt dt )
{
    return SharedPointer<_Ty>( newObj, dt );
}

/** \brief 创建一个`SharedPointer`来管理新对象资源 */
template < typename _Ty, typename... _ArgType >
inline SharedPointer<_Ty> MakeSharedNew( _ArgType&& ... arg )
{
    return SharedPointer<_Ty>( new _Ty( std::forward<_ArgType>(arg)... ) );
}


} // namespace winux

#endif // __SMARTPTR_HPP__
