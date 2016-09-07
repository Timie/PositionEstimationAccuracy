#ifndef RCE_CORE_RSETTINGNESTER_H
#define RCE_CORE_RSETTINGNESTER_H

class QSettings;
class QString;

namespace rce {
    namespace core {

        class RSettingNester
        {
        public:
            RSettingNester(QSettings &settings,
                           const QString &sectionName);

            ~RSettingNester();

        protected:
            QSettings &settings_;
            const QString &sectionName_;
        };

    } // namespace core
} // namespace rce

#endif // RCE_CORE_RSETTINGNESTER_H
