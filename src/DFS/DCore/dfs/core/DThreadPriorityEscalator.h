#ifndef DFS_CORE_DTHREADPRIORIDYESCALATOR_H
#define DFS_CORE_DTHREADPRIORIDYESCALATOR_H

#include <QThread>

namespace dfs {
    namespace core {

        class DThreadPriorityEscalator
        {
        public:
            DThreadPriorityEscalator(QThread *targetThread,
                                     QThread::Priority targetPriority = QThread::InheritPriority);
            ~DThreadPriorityEscalator();

        protected:
            QThread *targetThread_;
            QThread::Priority formerPriority_;
        };

    } // namespace core
} // namespace dfs

#endif // DFS_CORE_DTHREADPRIORIDYESCALATOR_H
