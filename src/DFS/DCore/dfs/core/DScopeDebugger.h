#ifndef DFS_CORE_DSCOPEDEBUGGER_H
#define DFS_CORE_DSCOPEDEBUGGER_H

#include <QtGlobal>
#include <QString>

namespace dfs {
    namespace core {

        class DScopeDebugger
        {


        public:
            enum DStreamType
            {
                DInfoStreamType,
                DDebugStreamType,
                DWarningStreamType
            };


            DScopeDebugger(DStreamType type,
                           const char *message,
                           const char *file,
                           int line,
                           const char *function);
            ~DScopeDebugger();
        protected:
            DStreamType outType_;

            QString message_;
            QString file_;
            int line_;
            QString function_;

        };

    } // namespace core
} // namespace dfs

// NOTE: Output of debug scopes is visible only if given debug output is enabled (see DDebug)

#ifdef RCE_ENABLE_DEBUG_SCOPES
#define dDebugScopeInfo(identificator, message) dfs::core::DScopeDebugger scopeDebugger_ ## identificator(dfs::core::DScopeDebugger::DInfoStreamType, message, __FILE__, __LINE__, Q_FUNC_INFO);
#define dDebugScopeDebug(identificator, message) dfs::core::DScopeDebugger scopeDebugger_ ## identificator(dfs::core::DScopeDebugger::DDebugStreamType, message, __FILE__, __LINE__, Q_FUNC_INFO);
#define dDebugScopeWarning(identificator, message) dfs::core::DScopeDebugger scopeDebugger_ ## identificator(dfs::core::DScopeDebugger::DWarningStreamType, message, __FILE__, __LINE__, Q_FUNC_INFO);


#else
#define dDebugScopeInfo(identificator, message)
#define dDebugScopeDebug(identificator, message)
#define dDebugScopeWarning(identificator, message)
#endif


#define dDebugScope dDebugScopeInfo(d, NULL)

#endif // DFS_CORE_DSCOPEDEBUGGER_H
