#include "RXmlHandling.h"

#include <QDomElement>
#include <QDomDocument>

QDomElement
rce::utility::RXmlHandling::
createChildElement(QDomElement &element,
                const QString &name)
{
    QDomElement existingElement = element.firstChildElement(name);
    if(existingElement.isNull())
    {
        QDomDocument ownerDocument = element.ownerDocument();
        QDomElement newElement = ownerDocument.createElement(name);
        element.appendChild(newElement);
        return newElement;
    }
    else
    {
        return existingElement;
    }
}
