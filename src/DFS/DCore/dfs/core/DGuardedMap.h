#ifndef DFS_CORE_RGUARDEDMAP_H
#define DFS_CORE_RGUARDEDMAP_H

#include <QMap>
#include <QReadWriteLock>

namespace dfs {
    namespace core {

        template <class Key, class T>
        class DGuardedMap
        {
        public:
            DGuardedMap(){}

            void addData(Key k,
                         T data);
            void removeData(Key k);

            bool retrieveData(Key k,
                              T &data) const;
            int size() const;

            void clear();


        protected:
            mutable QReadWriteLock lock_;
            QMap<Key, T> map_;
        };

        template <class Key, class T>
        void
        DGuardedMap<Key, T>::
        addData(Key k,
                T data)
        {
            QWriteLocker l(&lock_);
            map_.insert(k,data);
        }

        template <class Key, class T>
        void
        DGuardedMap<Key, T>::
        removeData(Key k)
        {
            QWriteLocker l(&lock_);
            map_.remove(k);
        }


        template <class Key, class T>
        bool
        DGuardedMap<Key, T>::
        retrieveData(Key k,
                     T &data) const
        {
            QReadLocker l(&lock_);
            auto it = map_.find(k);
            if(it != map_.end())
            {
                data = it.value();
                return true;
            }
            else
            {
                return false;
            };
        }

        template <class Key, class T>
        int
        DGuardedMap<Key, T>::
        size() const
        {
            QReadLocker l(&lock_);
            return map_.size();
        }

        template <class Key, class T>
        void
        DGuardedMap<Key, T>::
        clear()
        {
            QWriteLocker l(&lock_);
            map_.clear();
        }


    } // namespace core
} // namespace dfs

#endif // DFS_CORE_RGUARDEDMAP_H
