#ifndef RCE_UTILITY_RXMLHANDLING_H
#define RCE_UTILITY_RXMLHANDLING_H

#include <QString>

class QDomElement;

namespace rce {
    namespace utility {

        class RXmlHandling
        {
        public:
            RXmlHandling();

            static QDomElement createChildElement(QDomElement &element,
                                                  const QString &name);
        };

    } // namespace utility
} // namespace rce

#endif // RCE_UTILITY_RXMLHANDLING_H
