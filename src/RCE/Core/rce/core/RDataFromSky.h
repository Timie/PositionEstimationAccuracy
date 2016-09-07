#ifndef RCE_CORE_RDATAFROMSKY_H
#define RCE_CORE_RDATAFROMSKY_H

class QString;
class QDateTime;
//class QVariant;

#include <QSharedPointer>

// FIXME: Make access to communication and configuration thread safe

namespace rce {

    namespace licence
    {
        struct RLicence;
        class RLicenceClientInterface;
        class RLicenceConfiguration;
    }

    namespace core {
        class RDataFromSky
        {
        public:
            enum REdition
            {
                RDevelopmentEdition,
                RUltimateEdition,
                RFullEdition, // fast vehicle trajectory definition is disabled
                REditEdition,
                RPreviewEdition,
                RPublicEdition,
                RCustomEdition, // disabled create trajectory functionality
                RLicensedEdition
            };

            static REdition getSuiteEdition();
            static void setSuiteEdition(REdition edition);
            static QString getSuiteEditionString();

            static bool isActionVisible(const QString &actionID);
            static bool isActionChecked(const QString &actionID);

            static QString getLicenceParameter(const QString &actionID,
                                                int idx);

            static int getLicenceParametersCount(const QString &actionID);

            static void setEventHappened(const QString &eventID,
                                         const QString &eventDescription);


            static const licence::RLicence &getLicence();

            static void setLicenceComm(rce::licence::RLicenceClientInterface *comm);
            static void setLicence(const QSharedPointer<const rce::licence::RLicence> &lic);

            static QString getReleaseID();


            static void setLastOnline(const QDateTime &lastOnline);

        protected:
            static REdition &editEdition();
#ifdef RCE_USING_LICENCE
            static rce::licence::RLicence &editLicence();            
            static rce::licence::RLicenceClientInterface* &editLicenceComm();
            static rce::licence::RLicenceConfiguration &editLicenceConfig();
#endif
        };

    } // namespace core
} // namespace rce

#endif // RCE_CORE_RDATAFROMSKY_H
