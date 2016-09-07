#include "RSettingNester.h"

#include <QSettings>
#include <QString>


rce::core::RSettingNester::
RSettingNester(QSettings &settings,
               const QString &sectionName):
    settings_(settings),
    sectionName_(sectionName)
{
    if(!sectionName.isEmpty())
        settings_.beginGroup(sectionName);
}

rce::core::RSettingNester::~RSettingNester()
{
    if(!sectionName_.isEmpty())
        settings_.endGroup();
}
