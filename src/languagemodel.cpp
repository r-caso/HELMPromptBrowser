#include "languagemodel.hpp"

#include <cmath>

namespace HELMPromptBrowser {

const std::unordered_map<Vendor, QString> vendorName = {
    { Vendor::AlephAlpha, "AlephAlpha" },
    { Vendor::ai21, "ai21" },
    { Vendor::anthropic, "anthropic" },
    { Vendor::cohere, "cohere" },
    { Vendor::eleutherai, "eleutherai" },
    { Vendor::lmsys, "lmsys" },
    { Vendor::meta, "meta" },
    { Vendor::microsoft, "microsoft" },
    { Vendor::mistralai, "mistralai" },
    { Vendor::mosaicml, "mosaicml" },
    { Vendor::openai, "openai" },
    { Vendor::stanford, "stanford" },
    { Vendor::tiiuae, "tiiuae" },
    { Vendor::together, "together" },
    { Vendor::writer_palmyra, "writer_palmyra" }
};

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
