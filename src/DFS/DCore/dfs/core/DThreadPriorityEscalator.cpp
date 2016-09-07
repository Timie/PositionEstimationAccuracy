#include "DThreadPriorityEscalator.h"



dfs::core::DThreadPriorityEscalator::
DThreadPriorityEscalator(QThread *targetThread,
                         QThread::Priority targetPriority):
    targetThread_(targetThread),
    formerPriority_(targetThread->priority())
{
    if(targetPriority == QThread::InheritPriority)
    {
        targetPriority = QThread::currentThread()->priority();
    }

    if(targetPriority != QThread::InheritPriority)
    {
        targetThread_->setPriority(targetPriority);
    }
}

dfs::core::DThreadPriorityEscalator::
~DThreadPriorityEscalator()
{
    if(formerPriority_ != QThread::InheritPriority)
    {
        targetThread_->setPriority(formerPriority_);
    }
}
