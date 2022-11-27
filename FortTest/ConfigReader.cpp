#include "ConfigReader.h"

#include <QFile>

#include <qxmlstream.h>
#include <QtXml/qdom.h>

ConfigReader::ConfigReader()
{
    QFile file("D:\\VisualStudioProjects\\FortTest\\FortTest\\config.xml");
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QDomDocument document;
    document.setContent(&file);

    QDomElement root = document.firstChildElement();


    QDomElement element = document.documentElement();
    for (QDomNode node = element.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        QString sNodeName = node.toElement().tagName();
        QString sNodeValue = node.toElement().text();

        m_mValue[sNodeName] = sNodeValue;
    }

}

ConfigReader::~ConfigReader()
{
}

int ConfigReader::getInt(const char* key_)
{
    return m_mValue[key_].toInt();
}
