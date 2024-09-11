#include "languagemodel.hpp"

#include <cmath>

namespace HELMPromptBrowser {

LanguageModel::LanguageModel(const int id, const QString& name, const double parameters)
    : m_Id(id), m_Name(name), m_Parameters(parameters)
{}

const QString& LanguageModel::name() const
{
    return m_Name;
}

double LanguageModel::parameters() const
{
    return m_Parameters;
}

Vendor LanguageModel::vendor() const
{
    constexpr int base = 0x10;
    return static_cast<Vendor>((m_Id / base) % base);
}

int LanguageModel::id() const
{
    return m_Id;
}

}
