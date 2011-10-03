/*
    Copyright 2005-2008 Intel Corporation.  All Rights Reserved.

    This file is part of Threading Building Blocks.

    Threading Building Blocks is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    version 2 as published by the Free Software Foundation.

    Threading Building Blocks is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Threading Building Blocks; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/

#ifndef __TBB_spin_rw_mutex_H
#define __TBB_spin_rw_mutex_H

#include "tbb_stddef.h"
#include "tbb_machine.h"

namespace tbb {

class spin_rw_mutex_v3;
typedef spin_rw_mutex_v3 spin_rw_mutex;

//! Fast, unfair, spinning reader-writer lock with backoff and writer-preference
/** @ingroup synchronization */
class spin_rw_mutex_v3 {
    //! @cond INTERNAL

    //! Internal acquire write lock.
    bool internal_acquire_writer();

    //! Out of line code for releasing a write lock.  
    /** This code is has debug checking and instrumentation for Intel(R) Thread Checker and Intel(R) Thread Profiler. */
    void internal_release_writer();

    //! Internal acquire read lock.
    void internal_acquire_reader();

    //! Internal upgrade reader to become a writer.
    bool internal_upgrade();

    //! Out of line code for downgrading a writer to a reader.   
    /** This code is has debug checking and instrumentation for Intel(R) Thread Checker and Intel(R) Thread Profiler. */
    void internal_downgrade();

    //! Internal release read lock.
    void internal_release_reader();

    //! Internal try_acquire write lock.
    bool internal_try_acquire_writer();

    //! Internal try_acquire read lock.
    bool internal_try_acquire_reader();

    //! @endcond
public:
    //! Construct unacquired mutex.
    spin_rw_mutex_v3() : state(0) {}

#if TBB_DO_ASSERT
    //! Destructor asserts if the mutex is acquired, i.e. state is zero.
    ~spin_rw_mutex_v3() {
        __TBB_ASSERT( !state, "destruction of an acquired mutex");
    };
#endif /* TBB_DO_ASSERT */

    //! The scoped locking pattern
    /** It helps to avoid the common problem of forgetting to release lock.
        It also nicely provides the "node" for queuing locks. */
    class scoped_lock : private internal::no_copy {
    public:
        //! Construct lock that has not acquired a mutex.
        /** Equivalent to zero-initialization of *this. */
        scoped_lock() : mutex(NULL) {}

        //! Acquire lock on given mutex.
        /** Upon entry, *this should not be in the "have acquired a mutex" state. */
        scoped_lock( spin_rw_mutex& m, bool write = true ) : mutex(NULL) {
            acquire(m, write);
        }

        //! Release lock (if lock is held).
        ~scoped_lock() {
            if( mutex ) release();
        }

        //! Acquire lock on given mutex.
        void acquire( spin_rw_mutex& m, bool write = true ) {
            __TBB_ASSERT( !mutex, "holding mutex already" );
            is_writer = write; 
            mutex = &m;
            if( write ) mutex->internal_acquire_writer();
            else        mutex->internal_acquire_reader();
        }

        //! Upgrade reader to become a writer.
        /** Returns true if the upgrade happened without re-acquiring the lock and false if opposite */
        bool upgrade_to_writer() {
            __TBB_ASSERT( mutex, "lock is not acquired" );
            __TBB_ASSERT( !is_writer, "not a reader" );
            is_writer = true; 
            return mutex->internal_upgrade();
        }

        //! Release lock.
        void release() {
            __TBB_ASSERT( mutex, "lock is not acquired" );
            spin_rw_mutex *m = mutex; 
            mutex = NULL;
#if TBB_DO_THREADING_TOOLS||TBB_DO_ASSERT
            if( is_writer ) m->internal_release_writer();
            else            m->internal_release_reader();
#else
            if( is_writer ) __TBB_AtomicAND( &m->state, READERS ); 
            else            __TBB_FetchAndAddWrelease( &m->state, -(intptr_t)ONE_READER);
#endif /* TBB_DO_THREADING_TOOLS||TBB_DO_ASSERT */
        }

        //! Downgrade writer to become a reader.
        bool downgrade_to_reader() {
#if TBB_DO_THREADING_TOOLS||TBB_DO_ASSERT
            __TBB_ASSERT( mutex, "lock is not acquired" );
            __TBB_ASSERT( is_writer, "not a writer" );
            mutex->internal_downgrade();
#else
	     __TBB_FetchAndAddW( &mutex->state, ((intptr_t)ONE_READER-WRITER));
#endif /* TBB_DO_THREADING_TOOLS||TBB_DO_ASSERT */
            is_writer = false;

            return true;
        }

        //! Try acquire lock on given mutex.
        bool try_acquire( spin_rw_mutex& m, bool write = true ) {
            __TBB_ASSERT( !mutex, "holding mutex already" );
            bool result;
            is_writer = write; 
            result = write? m.internal_try_acquire_writer()
                          : m.internal_try_acquire_reader();
            if( result ) 
                mutex = &m;
            return result;
        }

    private:
        //! The pointer to the current mutex that is held, or NULL if no mutex is held.
        spin_rw_mutex* mutex;

        //! If mutex!=NULL, then is_writer is true if holding a writer lock, false if holding a reader lock.
        /** Not defined if not holding a lock. */
        bool is_writer;
    };

    // Mutex traits
    static const bool is_rw_mutex = true;
    static const bool is_recursive_mutex = false;
    static const bool is_fair_mutex = false;

private:
    typedef intptr_t state_t;
    static const state_t WRITER = 1;
    static const state_t WRITER_PENDING = 2;
    static const state_t READERS = ~(WRITER | WRITER_PENDING);
    static const state_t ONE_READER = 4;
    static const state_t BUSY = WRITER | READERS;
    //! State of lock
    /** Bit 0 = writer is holding lock
        Bit 1 = request by a writer to acquire lock (hint to readers to wait)
        Bit 2..N = number of readers holding lock */
    state_t state;
};

} // namespace ThreadingBuildingBlocks

#endif /* __TBB_spin_rw_mutex_H */
