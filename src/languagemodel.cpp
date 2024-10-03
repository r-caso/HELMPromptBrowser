#include "languagemodel.hpp"

LanguageModel::LanguageModel(const int id, QString name, const double parameters)
    : m_Id(id), m_Name(std::move(name)), m_Parameters(parameters)
{}

const QString& LanguageModel::name() const
{
    return m_Name;
}

double LanguageModel::parameters() const
{
    return m_Parameters;
}

HPB::Vendor LanguageModel::vendor() const
{
    constexpr int base = 0x10;
    return static_cast<HPB::Vendor>((m_Id / base) % base);
}

int LanguageModel::id() const
{
    return m_Id;
}
