#ifndef __SAFE_CONTAINER_HPP__
#define __SAFE_CONTAINER_HPP__
//
// safecontainer 提供线程安全的容器
//

namespace winux
{
/** \brief 安全队列 */
template < typename _Ty >
class SafeQueue
{
public:
    /** \brief 构造函数 */
    SafeQueue() : _mtx(true), _cdt(true), _exitPop(false) { }

    /** \brief 弹出并返回元素 */
    _Ty popEx()
    {
        ScopeGuard guard(_mtx);
        while ( _q.empty() )
        {
            // 抛出一个异常用以结束等待
            if ( _exitPop ) throw Error( 0, "SafeQueue.pop() has been exited." );

            _cdt.wait(_mtx);
        }

        _Ty t = std::move( _q.front() );
        _q.pop();
        return t;
    }

    /** \brief 弹出 */
    void pop()
    {
        ScopeGuard guard(_mtx);
        while ( _q.empty() )
        {
            // 抛出一个异常用以结束等待
            if ( _exitPop ) throw Error( 0, "SafeQueue.pop() has been exited." );

            _cdt.wait(_mtx);
        }
        _q.pop();
    }

    /** \brief 引用头部元素 */
    _Ty & front()
    {
        ScopeGuard guard(_mtx);
        while ( _q.empty() )
        {
            // 抛出一个异常用以结束等待
            if ( _exitPop ) throw Error( 0, "SafeQueue.front() has been exited." );

            _cdt.wait(_mtx);
        }
        return _q.front();
    }

    /** \brief 引用头部元素 */
    _Ty const & front() const
    {
        ScopeGuard guard(_mtx);
        while ( _q.empty() )
        {
            // 抛出一个异常用以结束等待
            if ( _exitPop ) throw Error( 0, "SafeQueue.front() has been exited." );

            const_cast<SafeQueue *>(this)->_cdt.wait( const_cast<SafeQueue *>(this)->_mtx );
        }
        return _q.front();
    }

    /** \brief 压入元素 */
    template < typename _Ty1 >
    void push( _Ty1 && v )
    {
        ScopeGuard guard(_mtx);
        _q.push( std::forward<_Ty1>(v) );
        _cdt.notify();
    }

    /** \brief 队列大小 */
    size_t size() const
    {
        ScopeGuard guard(const_cast<Mutex &>(_mtx));
        return _q.size();
    }

    /** \brief 退出pop()的阻塞（通过抛异常的方式） */
    void exitPop()
    {
        ScopeGuard guard(_mtx);
        _exitPop = true;
        _cdt.notifyAll();
    }

    /** \brief 暴露安全容器自带的互斥量 */
    Mutex & mutex() { return _mtx; }

private:
    std::queue<_Ty> _q;
    Mutex _mtx;
    Condition _cdt;
    bool _exitPop;
};


} // namespace winux

#endif // __SAFE_CONTAINER_HPP__
