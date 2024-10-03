#pragma once

#include <QString>

#include "hpb_globals.hpp"

class LanguageModel {
public:
    LanguageModel(int id, QString name, double parameters);
    const QString& name() const;
    double parameters() const;
    HPB::Vendor vendor() const;
    int id() const;

private:
    int m_Id;
    QString m_Name;
    double m_Parameters;
};
