#include "RDataFromSky.h"

#include <QObject>
#include <QVariant>
#include <QString>
#include <QCoreApplication>

#ifdef RCE_USING_LICENCE

#include "rce/licence/RLicence.h"
#include "rce/licence/RLicenceClientInterface.h"
#include "rce/licence/RLicenceConfiguration.h"

#endif


//rce::licence::RLicence
//rce::core::RDataFromSky::licence_ = QSharedPointer<rce::licence::RLicence>::create();

rce::core::RDataFromSky::REdition
rce::core::RDataFromSky::
getSuiteEdition()
{
    return editEdition();
}

rce::core::RDataFromSky::REdition &
rce::core::RDataFromSky::
editEdition()
{
    static REdition edition = RDevelopmentEdition;
    return edition;
}
void
rce::core::RDataFromSky::
setSuiteEdition(rce::core::RDataFromSky::REdition edition)
{
    editEdition() = edition;
}

QString
rce::core::RDataFromSky::
getSuiteEditionString()
{
    switch(getSuiteEdition())
    {
        case rce::core::RDataFromSky::RDevelopmentEdition:
            return QObject::tr("Development Edition");
            break;
        case rce::core::RDataFromSky::RFullEdition:
            return QObject::tr("Full Edition");
            break;
        case rce::core::RDataFromSky::REditEdition:
            return QObject::tr("Edit Edition");
            break;
        case rce::core::RDataFromSky::RPreviewEdition:
            return QObject::tr("Preview Edition");
            break;
        case rce::core::RDataFromSky::RPublicEdition:
            return QObject::tr("Public Edition");
        case rce::core::RDataFromSky::RCustomEdition:
            return QObject::tr("Custom Edition");
        case rce::core::RDataFromSky::RUltimateEdition:
            return QObject::tr("Ultimate Edition");
        case rce::core::RDataFromSky::RLicensedEdition:
            return QObject::tr("Licenced Edition"); // QUESTION: Fetch edition from licence?
        default:
            return QObject::tr("Undefined Edition");


    }
}

bool
rce::core::RDataFromSky::
isActionVisible(const QString &actionID)
{

#ifdef RCE_USING_LICENCE
    rce::licence::RLicenceConfiguration::RActionConfiguration actionConfig = editLicenceConfig().getActionConfiguration(actionID);
    return actionConfig.isVisible_;
#else
    return true;
#endif
}



bool
rce::core::RDataFromSky::
isActionChecked(const QString &actionID)
{
#ifdef RCE_USING_LICENCE
    rce::licence::RLicenceConfiguration::RActionConfiguration actionConfig = editLicenceConfig().getActionConfiguration(actionID);
    return actionConfig.isChecked_;
#else
    return true;
#endif
}


QString
rce::core::RDataFromSky::
getLicenceParameter(const QString &actionID,
                    int idx)
{

#ifdef RCE_USING_LICENCE
    rce::licence::RLicenceConfiguration::RActionConfiguration actionConfig = editLicenceConfig().getActionConfiguration(actionID);
    if((idx >= 0) &&
       (idx < actionConfig.parameters_.size()))
    {
       return actionConfig.parameters_[idx];
    }
    else
    {
        return QString();
    }
#else
    return QString();
#endif
}




int rce::core::RDataFromSky::getLicenceParametersCount(const QString &actionID)
{

#ifdef RCE_USING_LICENCE
    return editLicenceConfig().getActionConfiguration(actionID).parameters_.size();
#else
    return 0;
#endif
}


#ifdef RCE_USING_LICENCE

void
rce::core::RDataFromSky::
setEventHappened(const QString &eventID,
                 const QString &eventDescription)
{
    if(editLicenceComm() != NULL)
    {
        editLicenceComm()->sendEvent(eventID,
                                     QDateTime::currentDateTimeUtc(),
                                     eventDescription.toUtf8());
    }
}

const rce::licence::RLicence  &
rce::core::RDataFromSky::
getLicence()
{
    return editLicence();
}

void
rce::core::RDataFromSky::
setLicenceComm(rce::licence::RLicenceClientInterface *comm)
{
    editLicenceComm() = comm;
}

void
rce::core::RDataFromSky::
setLicence(const QSharedPointer<const licence::RLicence> &lic)
{
    editLicence() = *lic;

    editLicenceConfig().parseFromString(editLicence().versionConfigurationString_);
}

QString
rce::core::RDataFromSky::
getReleaseID()
{
    return QCoreApplication::applicationVersion();
}

void
rce::core::RDataFromSky::
setLastOnline(const QDateTime &lastOnline)
{
    editLicence().lastOnline_ = lastOnline;
}


rce::licence::RLicence &
rce::core::RDataFromSky::
editLicence()
{
    static rce::licence::RLicence licence = rce::licence::RLicence();

    return licence;
}

rce::licence::RLicenceClientInterface *&
rce::core::RDataFromSky::
editLicenceComm()
{
    static rce::licence::RLicenceClientInterface *comm = NULL;
    return comm;
}

rce::licence::RLicenceConfiguration &
rce::core::RDataFromSky::
editLicenceConfig()
{
    static rce::licence::RLicenceConfiguration config = rce::licence::RLicenceConfiguration();
    return config;
}

#endif
