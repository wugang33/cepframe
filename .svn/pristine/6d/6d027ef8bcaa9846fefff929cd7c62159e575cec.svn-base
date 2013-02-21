/* 
 * A circular bounded queue
 * 
 * File:   circular_queue.hpp
 * Author: WangquN
 *
 * Created on 2012-01-05 AM 09:05
 */

#ifndef CIRCULAR_QUEUE_HPP
#define CIRCULAR_QUEUE_HPP

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/thread.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"

// #include <ext/mt_allocator.h>

#include "macro_log.hpp"
#include "cep_base.hpp"

using namespace std;

namespace cep {

#ifdef  __VER4TEST_TEMPLATE_SPECIALIZATION__
    template<typename T, typename Mt>
    class circular_bounded_queue;

    class circular_bounded_queue_impl {
    public:

        template<typename T, typename Mt>
        void template_impl_test(circular_bounded_queue<T, Mt>& queue) {
            cout << "hello queue:" << queue << endl;
        }
    };
#endif

    template<typename T, typename Mt = false_tag>
            class circular_bounded_queue {
        /// Ref:boost/interprocess/ipc/message_queue.hpp
        // Blocking modes
        //        enum block_t {
        //            blocking, timed, non_blocking
        //        };
    public:
        typedef size_t capacity_type;

        circular_bounded_queue(capacity_type n) : capacity_(n), highIndex_(n - 1) {
            MLOG_DEBUG << boost::this_thread::get_id() << ':' << this << "->circular_bounded_queue::ctor execute!" << endl;
            init();
            reset_counter();
        };

#ifdef __FLAG4INHERITABLE__

        virtual
#endif
        ~circular_bounded_queue() {
            MLOG_DEBUG << boost::this_thread::get_id() << ':' << this << "->circular_bounded_queue::dtor execute!" << endl;
            /* T t = Pop();
            while (t) {
                delete t;
                t = Pop();
            }*/ // Do not delete the context! The out resource should by delete by out.
            delete [] buffer_;

            // More Exceptional C++ 4:23
            // T * (T::*testClone) () const = &T::Clone;
            // testClone; // 禁止掉未使用的变量所造成的警告
            // 这种未使用的变量可能会被编译器的优化程序完全消除掉
        };

    private: // Forbidden copy behave

        circular_bounded_queue(const circular_bounded_queue & queue) {
            MLOG_DEBUG << this << "->circular_bounded_queue copy from[" << &queue << "] ctor execute!" << endl;
        }

        circular_bounded_queue& operator=(const circular_bounded_queue & queue) {
            if (this == &queue) // effective c++ 16
                return *this;
            MLOG_DEBUG << this << "->circular_bounded_queue assignment operator(" << &queue << ") execute!" << endl;
            return *this;
        }
#ifdef __FLAG4INHERITABLE__
    protected:
#endif
        void init();
        double speed(const boost::posix_time::ptime& t, const unsigned long counter) const;
    public:

        capacity_type capacity() const {
            return capacity_;
        }

        capacity_type size() const {
            return pushCount_ - popCount_;
        }

        /**
         * Begin or recalculate speed
         */
        void reset_counter() const;

        unsigned long push_counter() const {
            return pushCounter_;
        }

        unsigned long pop_counter() const {
            return popCounter_;
        }

        double push_speed() const {
            return speed(beginTime_, pushCounter_);
        }

        double pop_speed() const {
            return speed(beginTime_, popCounter_);
        }

    private:

        inline void __do_push(const T& t);
        inline void __do_pop(T& t);
#ifdef __VER4QUEUE_SAFE_MODE__
        inline void __do_lock_push(const T& t);
        inline void __do_lock_pop(T& t);
#endif
        // St version!!! ///////////////////////////////////////////////////////

        void push(false_tag, const T& t) {
            st_push(t);
        }

        bool try_push(false_tag, const T& t) {
            return st_try_push(t);
        }

        bool timed_push(false_tag, const T& t, const size_t& wait_until) {
            return st_timed_push(t, wait_until);
        }

        void pop(false_tag, T& t) {
            st_pop(t);
        }

        bool try_pop(false_tag, T& t) {
            return st_try_pop(t);
        }

        bool timed_pop(false_tag, T& t, const size_t& wait_until) {
            return st_timed_pop(t, wait_until);
        }

        // Mt version!!! ///////////////////////////////////////////////////////

        void push(true_tag, const T& t) {
            mt_push(t);
        }

        bool try_push(true_tag, const T& t) {
            return mt_try_push(t);
        }

        bool timed_push(true_tag, const T& t, const size_t& wait_until) {
            return mt_timed_push(t, wait_until);
        }

        void pop(true_tag, T& t) {
            mt_pop(t);
        }

        bool try_pop(true_tag, T& t) {
            return mt_try_pop(t);
        }

        bool timed_pop(true_tag, T& t, const size_t& wait_until) {
            return mt_timed_pop(t, wait_until);
        }
    public:

        // St version!!!
        void st_push(const T& t);
        bool st_try_push(const T& t);
        bool st_timed_push(const T& t, const size_t& wait_until);

        void st_pop(T& t);
        bool st_try_pop(T& t);
        bool st_timed_pop(T& t, const size_t& wait_until);

        // Mt version!!!
        void mt_push(const T& t);
        bool mt_try_push(const T& t);
        bool mt_timed_push(const T& t, const size_t& wait_until);

        void mt_pop(T& t);
        bool mt_try_pop(T& t);
        bool mt_timed_pop(T& t, const size_t& wait_until);

        void push(const T& t) {
            push(mt_flag_, t);
        }

        bool try_push(const T& t) {
            return try_push(mt_flag_, t);
        }

        bool timed_push(const T& t, const size_t& wait_until = DEFAULT_WAIT_UNTIL_MS) {
            return timed_push(mt_flag_, t, wait_until);
        }

        void pop(T& t) {
            pop(mt_flag_, t);
        }

        bool try_pop(T& t) {
            return try_pop(mt_flag_, t);
        }

        bool timed_pop(T& t, const size_t& wait_until = DEFAULT_WAIT_UNTIL_MS) {
            return timed_pop(mt_flag_, t, wait_until);
        }
        /** millisecond */
        static const unsigned short DEFAULT_WAIT_UNTIL_MS = 500;

#ifdef  __VER4TEST_TEMPLATE_SPECIALIZATION__
        template<typename T1>
        void template_test(const T1&);
        template<char>
        void template_test(const char&);
        template<int>
        void template_test(const int&);

        void template_impl_test() {
            impl_.template_impl_test(*this);
        }
#endif
    private:
        const capacity_type capacity_, highIndex_;
        T * buffer_;

        capacity_type pushCount_, pushIndex_, popCount_, popIndex_;

        mutable boost::posix_time::ptime beginTime_;
        mutable unsigned long pushCounter_, popCounter_;

        bool is_not_empty() const {
            return pushCount_ != popCount_;
        }

        bool is_not_full() const {
            return pushCount_ - popCount_ < capacity_;
        }

        boost::mutex mutex_;
        boost::condition condPop_;
        boost::condition condPush_;
        Mt mt_flag_;
#ifndef __VER4QUEUE_MT__ // __VER4QUEUE_MT_WRAP__
        boost::mutex mutexPop_, mutexPush_;
#endif

#ifdef  __VER4TEST_TEMPLATE_SPECIALIZATION__
        friend class circular_bounded_queue_impl;
        circular_bounded_queue_impl impl_;
#endif
        template<typename T1, typename Mt1>
        friend ostream& operator<<(ostream&, const circular_bounded_queue<T1, Mt1>&);
    };

#ifdef  __VER4TEST_TEMPLATE_SPECIALIZATION__

    template<typename T, typename Mt>
    template<typename T1>
    void circular_bounded_queue<T, Mt>::template_test(const T1&) {
        cout << "<T1>::template_test" << endl;
    }

    // g++ not support template<>
    // but template<int> and template<char*> ... is right
    // template<void>, template<float>, template<double> is not support

    template<typename T, typename Mt>
    template<char>
    void circular_bounded_queue<T, Mt>::template_test(const char&) {
        cout << "<char>::template_test" << endl;
    }

    template<typename T, typename Mt>
    template<int>
    void circular_bounded_queue<T, Mt>::template_test(const int&) {
        cout << "<int>::template_test" << endl;
    }
#endif
    ////////////////////////////////////////////////////////////////////////////
    // Member function implements!!! ///////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    template<typename T1, typename Mt1>
    ostream& operator<<(ostream& s, const circular_bounded_queue<T1, Mt1>& r) {
        s << boost::this_thread::get_id() << ':' << &r << "->" << r.size() << '/' << r.capacity_
                << '[' << r.pushCounter_ << "->" << r.popCounter_
                << ']' << r.pushCount_ << '/' << r.popCount_
                << ':' << r.push_speed() << "->" << r.pop_speed();
        return s;
    }
    ////////////////////////////////////////////////////////////////////////////

    template<typename T, typename Mt>
    void circular_bounded_queue<T, Mt>::init() {
        pushCount_ = 0;
        pushIndex_ = 0;
        popCount_ = 0;
        popIndex_ = 0;
        buffer_ = new T[capacity_];
        // memset(buffer_, 0, sizeof (T) * capacity_);
    }

    template<typename T, typename Mt>
    double circular_bounded_queue<T, Mt>::speed(const boost::posix_time::ptime& t, const unsigned long counter) const {
        boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
        // boost::posix_time::time_period tmp(begin, end);
        // const int s = boost::posix_time::time_duration(now - beginTime_).total_seconds();
        boost::posix_time::time_duration td = now - t;
        const boost::posix_time::time_duration::tick_type s = td.total_milliseconds();
        if (s <= 0) return 0;
        else
            return (double) (counter * 1000) / s;
    }

    template<typename T, typename Mt>
    void circular_bounded_queue<T, Mt>::reset_counter() const {
        beginTime_ = boost::posix_time::microsec_clock::local_time();
        pushCounter_ = 0;
        popCounter_ = 0;
    }
    ////////////////////////////////////////////////////////////////////////////

    template<typename T, typename Mt>
    inline void circular_bounded_queue<T, Mt>::__do_push(const T& t) {
        buffer_[pushIndex_] = t;
        ++pushCount_;
        // Just for Fun
        if (pushCount_ == 0) cout << "*** " << this << "::pushCount_ to zero! ***" << endl;
        ++pushCounter_; // count for speed
        if (pushIndex_ == highIndex_)
            pushIndex_ = 0;
        else
            ++pushIndex_;
    }

    template<typename T, typename Mt>
    inline void circular_bounded_queue<T, Mt>::__do_pop(T& t) {
        // T result = buffer_[popIndex_];
        t = buffer_[popIndex_];
        ++popCount_;
        // Just for Fun
        if (popCount_ == 0) cout << "*** " << this << "::popCount_ to zero! ***" << endl;
        ++popCounter_; // count for speed
        if (popIndex_ == highIndex_)
            popIndex_ = 0;
        else
            ++popIndex_;
        // return result;
    }

#ifdef __VER4QUEUE_SAFE_MODE__

    template<typename T, typename Mt>
    inline void circular_bounded_queue<T, Mt>::__do_lock_push(const T& t) {
        /*buffer_[pushIndex_] = t;
        ++pushCount_;
        // Just for Fun
        if (pushCount_ == 0) cout << "*** " << this << "::pushCount_ to zero! ***" << endl;
        ++pushCounter_; // count for speed
        if (pushIndex_ == highIndex_)
            pushIndex_ = 0;
        else
            ++pushIndex_;*/
        boost::mutex::scoped_lock lock(mutex_);
        condPush_.wait(lock, boost::bind(&circular_bounded_queue::is_not_full, this));
        __do_push(t);
        lock.unlock();
        condPop_.notify_one();
    }

    template<typename T, typename Mt>
    inline void circular_bounded_queue<T, Mt>::__do_lock_pop(T& t) {
        /*t = buffer_[popIndex_];
        ++popCount_;
        // Just for Fun
        if (popCount_ == 0) cout << "*** " << this << "::popCount_ to zero! ***" << endl;
        ++popCounter_; // count for speed
        if (popIndex_ == highIndex_)
            popIndex_ = 0;
        else
            ++popIndex_;*/
        boost::mutex::scoped_lock lock(mutex_);
        condPop_.wait(lock, boost::bind(&circular_bounded_queue::is_not_empty, this));
        __do_pop(t);
        lock.unlock();
        condPush_.notify_one();
    }
#endif // __VER4QUEUE_SAFE_MODE__
    ////////////////////////////////////////////////////////////////////////////

    template<typename T, typename Mt>
    void circular_bounded_queue<T, Mt>::st_push(const T& t) {
#ifdef __VER4QUEUE_SAFE_MODE__
        __do_lock_push(t);
        return;
#endif // __VER4QUEUE_SAFE_MODE__
        if (is_not_full()) {
            __do_push(t);
        } else {
            {
#ifdef  __VER4BUG_QUEUE_BLOCKING_DEADLOCK__
                boost::mutex::scoped_lock lock(mutex_);
                // if (is_not_full()) {
                //     __do_push(t);
                //     condPop_.notify_one();
                //     return;
                // } else
                condPush_.wait(lock, boost::bind(&circular_bounded_queue::is_not_full, this));
#elif __VER4QUEUE_BLOCKING_SLEEP__
                while (!is_not_full()) {
                    // sleep(1); // 1 second = 1000 milliseconds
                    usleep(DEFAULT_WAIT_UNTIL_MS * 1000); // 1 millisecond = 1000 microseconds
                }
#else // __VER4QUEUE_BLOCKING_TIMED_WAIT__
                bool isTimeout = true;
                const boost::posix_time::millisec waitUntil(DEFAULT_WAIT_UNTIL_MS);
                boost::system_time timeout = boost::get_system_time();
                boost::mutex::scoped_lock lock(mutex_);
                while (isTimeout) {
                    timeout += waitUntil;
                    isTimeout = !(condPush_.timed_wait(lock, timeout,
                            boost::bind(&circular_bounded_queue::is_not_full, this)));
                    // if (isTimeout) {
                    //     MLOG_DEBUG << boost::this_thread::get_id() << ':' << this
                    //             << "->push loop timeout is_not_full is " << is_not_full() << endl;
                    // }
                }
#endif
            }
            __do_push(t);
        }
        condPop_.notify_one();
    }

    template<typename T, typename Mt>
    bool circular_bounded_queue<T, Mt>::st_try_push(const T& t) {
#ifdef __VER4QUEUE_SAFE_MODE__
        __do_lock_push(t);
        return true;
#endif // __VER4QUEUE_SAFE_MODE__
        if (is_not_full()) {
            __do_push(t);
            return true;
        } else
            return false;
    }

    template<typename T, typename Mt>
    bool circular_bounded_queue<T, Mt>::st_timed_push(const T& t, const size_t& wait_until) {
#ifdef __VER4QUEUE_SAFE_MODE__
        __do_lock_push(t);
        return true;
#endif // __VER4QUEUE_SAFE_MODE__
        // bool isEmpty = !is_not_empty();
        if (is_not_full()) {
            __do_push(t);
            // if (isEmpty)
            // if (!is_not_empty())
            condPop_.notify_one();
            return true;
        } else {
            bool isNotTimeout = true;
            {
                boost::mutex::scoped_lock lock(mutex_);
                // notFull_.wait(lock, boost::bind(&CircularQueue4SISO::is_not_full, this));
                boost::system_time timeout = boost::get_system_time() +
                        boost::posix_time::milliseconds(wait_until);
                isNotTimeout = condPush_.timed_wait(lock, timeout, boost::bind(&circular_bounded_queue::is_not_full, this));
            }
            if (isNotTimeout) { // is_not_full()
                __do_push(t);
                // if (isEmpty)
                // if (!is_not_empty())
                condPop_.notify_one();
                return true;
            } else {
                MLOG_DEBUG << boost::this_thread::get_id() << ':' << this << "->timed_push timeout!" << endl;
                return false;
            }
        }
    }

    template<typename T, typename Mt>
    void circular_bounded_queue<T, Mt>::st_pop(T& t) {
#ifdef __VER4QUEUE_SAFE_MODE__
        __do_lock_pop(t);
        return;
#endif // __VER4QUEUE_SAFE_MODE__
        // T result = NULL;
        if (is_not_empty()) {
            // result = __do_pop();
            __do_pop(t);
        } else {
            {
#ifdef  __VER4BUG_QUEUE_BLOCKING_DEADLOCK__
                boost::mutex::scoped_lock lock(mutex_);
                condPop_.wait(lock, boost::bind(&circular_bounded_queue::is_not_empty, this));
#elif __VER4QUEUE_BLOCKING_SLEEP__
                while (!is_not_empty()) {
                    // sleep(1); // 1 second = 1000 milliseconds
                    usleep(DEFAULT_WAIT_UNTIL_MS * 1000); // 1 millisecond = 1000 microseconds
                }
#else // __VER4QUEUE_BLOCKING_TIMED_WAIT__
                bool isTimeout = true;
                const boost::posix_time::millisec waitUntil(DEFAULT_WAIT_UNTIL_MS);
                boost::system_time timeout = boost::get_system_time();
                boost::mutex::scoped_lock lock(mutex_);
                while (isTimeout) {
                    timeout += waitUntil;
                    isTimeout = !(condPop_.timed_wait(lock, timeout,
                            boost::bind(&circular_bounded_queue::is_not_empty, this)));
                    // if (isTimeout) {
                    //     MLOG_DEBUG << boost::this_thread::get_id() << ':' << this
                    //             << "->pop loop timeout is_not_empty is " << is_not_empty() << endl;
                    // }
                }
#endif
            }
            // result = __do_pop();
            __do_pop(t);
        }
        condPush_.notify_one();
        // return result;
    }

    template<typename T, typename Mt>
    bool circular_bounded_queue<T, Mt>::st_try_pop(T& t) {
#ifdef __VER4QUEUE_SAFE_MODE__
        __do_lock_pop(t);
        return true;
#endif // __VER4QUEUE_SAFE_MODE__
        if (is_not_empty()) {
            // return __do_pop();
            __do_pop(t);
            return true;
        } else
            return false; // NULL;
    }

    template<typename T, typename Mt>
    bool circular_bounded_queue<T, Mt>::st_timed_pop(T& t, const size_t& wait_until) {
#ifdef __VER4QUEUE_SAFE_MODE__
        __do_lock_pop(t);
        return true;
#endif // __VER4QUEUE_SAFE_MODE__
        // T result = NULL;
        // bool isFull = !is_not_full();
        if (is_not_empty()) { // pushCount_ != popCount_
            // result = __do_pop();
            __do_pop(t);
            // if (!is_not_full())
            condPush_.notify_one();
        } else {
            bool isNotTimeout = true;
            {
                boost::mutex::scoped_lock lock(mutex_);
                // notEmpty_.wait(lock, boost::bind(&CircularQueue4SISO::is_not_empty, this));
                boost::system_time timeout = boost::get_system_time() +
                        boost::posix_time::milliseconds(wait_until);
                isNotTimeout = condPop_.timed_wait(lock, timeout, boost::bind(&circular_bounded_queue::is_not_empty, this));
            }
            if (isNotTimeout) { // is_not_empty() pushCount_ != popCount_
                // result = __do_pop();
                __do_pop(t);
                // if (!is_not_full())
                condPush_.notify_one();
            } else {
                MLOG_DEBUG << boost::this_thread::get_id() << ':' << this << "->timed_pop timeout!" << endl;
                return false;
            }
        }
        // if (isFull)
        // notFull_.notify_one();
        return true; // result;
    }
    ////////////////////////////////////////////////////////////////////////////
    // !Template [Partial] Specialization: Mt version //////////////////////////
    ////////////////////////////////////////////////////////////////////////////

    template<typename T, typename Mt>
    void circular_bounded_queue<T, Mt>::mt_push(const T& t) {
#ifdef __VER4QUEUE_SAFE_MODE__
        __do_lock_push(t);
        return;
#endif // __VER4QUEUE_SAFE_MODE__
        // MLOG_TRACE << boost::this_thread::get_id() << ':' << this << "-> Mt push!" << endl;
#ifdef __VER4QUEUE_MT__
        {
            boost::mutex::scoped_lock lock(mutex_);
            condPush_.wait(lock, boost::bind(&circular_bounded_queue::is_not_full, this));
            __do_push(t);
        }
        condPop_.notify_one();
#else // __VER4QUEUE_MT_WRAP__
        boost::mutex::scoped_lock lock(mutexPush_);
        st_push(t);
#endif
    }

    template<typename T, typename Mt>
    bool circular_bounded_queue<T, Mt>::mt_try_push(const T& t) {
#ifdef __VER4QUEUE_SAFE_MODE__
        __do_lock_push(t);
        return true;
#endif // __VER4QUEUE_SAFE_MODE__
        // MLOG_TRACE << boost::this_thread::get_id() << ':' << this << "-> Mt try_push!" << endl;
#ifdef __VER4QUEUE_MT__
        boost::mutex::scoped_lock lock(mutex_);
        if (is_not_full()) {
            __do_push(t);
            return true;
        } else
            return false;
#else // __VER4QUEUE_MT_WRAP__
        boost::mutex::scoped_lock lock(mutexPush_);
        return st_try_push(t);
#endif
    }

    template<typename T, typename Mt>
    bool circular_bounded_queue<T, Mt>::mt_timed_push(const T& t, const size_t& wait_until) {
#ifdef __VER4QUEUE_SAFE_MODE__
        __do_lock_push(t);
        return true;
#endif // __VER4QUEUE_SAFE_MODE__
        // MLOG_TRACE << boost::this_thread::get_id() << ':' << this << "-> Mt timed_push!" << endl;
#ifdef __VER4QUEUE_MT__
        bool isNotTimeout = true;
        {
            boost::mutex::scoped_lock lock(mutex_);
            // notFull_.wait(lock, boost::bind(&CircularQueue4SISO::is_not_full, this));
            boost::system_time timeout = boost::get_system_time() +
                    boost::posix_time::milliseconds(wait_until);
            isNotTimeout = condPush_.timed_wait(lock, timeout, boost::bind(&circular_bounded_queue::is_not_full, this));
            if (isNotTimeout) __do_push(t);
        }
        if (isNotTimeout) {
            condPop_.notify_one();
            return true;
        } else {
            MLOG_DEBUG << boost::this_thread::get_id() << ':' << this << "->timed_push timeout for MT!" << endl;
            return false;
        }
#else // __VER4QUEUE_MT_WRAP__
        boost::mutex::scoped_lock lock(mutexPush_);
        return st_timed_push(t, wait_until);
#endif
    }

    template<typename T, typename Mt>
    void circular_bounded_queue<T, Mt>::mt_pop(T& t) {
#ifdef __VER4QUEUE_SAFE_MODE__
        __do_lock_pop(t);
        return;
#endif // __VER4QUEUE_SAFE_MODE__
        // MLOG_TRACE << boost::this_thread::get_id() << ':' << this << "-> Mt pop!" << endl;
#ifdef __VER4QUEUE_MT__
        {
            boost::mutex::scoped_lock lock(mutex_);
            condPop_.wait(lock, boost::bind(&circular_bounded_queue::is_not_empty, this));
            __do_pop(t);
        }
        condPush_.notify_one();
#else // __VER4QUEUE_MT_WRAP__
        boost::mutex::scoped_lock lock(mutexPop_);
        st_pop(t);
#endif
    }

    template<typename T, typename Mt>
    bool circular_bounded_queue<T, Mt>::mt_try_pop(T& t) {
#ifdef __VER4QUEUE_SAFE_MODE__
        __do_lock_pop(t);
        return true;
#endif // __VER4QUEUE_SAFE_MODE__
        // MLOG_TRACE << boost::this_thread::get_id() << ':' << this << "-> Mt try_pop!" << endl;
#ifdef __VER4QUEUE_MT__
        boost::mutex::scoped_lock lock(mutex_);
        if (is_not_empty()) {
            // return __do_pop();
            __do_pop(t);
            return true;
        } else
            return false; // NULL;
#else // __VER4QUEUE_MT_WRAP__
        boost::mutex::scoped_lock lock(mutexPop_);
        return st_try_pop(t);
#endif
    }

    template<typename T, typename Mt>
    bool circular_bounded_queue<T, Mt>::mt_timed_pop(T& t, const size_t& wait_until) {
#ifdef __VER4QUEUE_SAFE_MODE__
        __do_lock_pop(t);
        return true;
#endif // __VER4QUEUE_SAFE_MODE__
        //  MLOG_TRACE << boost::this_thread::get_id() << ':' << this << "-> Mt timed_pop!" << endl;
#ifdef __VER4QUEUE_MT__
        bool isNotTimeout = true;
        {
            boost::mutex::scoped_lock lock(mutex_);
            // notEmpty_.wait(lock, boost::bind(&CircularQueue4SISO::is_not_empty, this));
            boost::system_time timeout = boost::get_system_time() +
                    boost::posix_time::milliseconds(wait_until);
            isNotTimeout = condPop_.timed_wait(lock, timeout, boost::bind(&circular_bounded_queue::is_not_empty, this));
            if (isNotTimeout) __do_pop(t);
        }
        if (isNotTimeout) {
            condPush_.notify_one();
            return true;
        } else {
            MLOG_DEBUG << boost::this_thread::get_id() << ':' << this << "->timed_pop timeout for MT!" << endl;
            return false;
        }
#else // __VER4QUEUE_MT_WRAP__
        boost::mutex::scoped_lock lock(mutexPop_);
        return st_timed_pop(t, wait_until);
#endif
    }
}; // namespace cep {

// #include "../src/circular_queue.cpp"

#endif  /* CIRCULAR_QUEUE_HPP */
