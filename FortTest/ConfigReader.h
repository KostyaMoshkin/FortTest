#pragma once

#include <map>
#include <QString>

class ConfigReader
{
	std::map<QString, QString> m_mValue;

public:
	ConfigReader();
	~ConfigReader();

public:
	int getInt(const char* key_);
};

