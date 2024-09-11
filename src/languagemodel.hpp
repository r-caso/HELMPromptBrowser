#pragma once

#include <unordered_map>
#include <QString>

namespace HELMPromptBrowser {

enum class Vendor : uint8_t {
    AlephAlpha = 0x0,
    ai21 = 0x1,
    anthropic = 0x2,
    cohere = 0x3,
    eleutherai = 0x4,
    lmsys = 0x5,
    meta = 0x6,
    microsoft = 0x7,
    mistralai = 0x8,
    mosaicml = 0x9,
    openai = 0xA,
    stanford = 0xB,
    tiiuae = 0xC,
    together = 0xD,
    writer_palmyra = 0xE,
};

extern const std::unordered_map<Vendor, QString> vendorName;

class LanguageModel {
public:
    LanguageModel(int id, const QString& name, double parameters);
    const QString& name() const;
    double parameters() const;
    Vendor vendor() const;
    int id() const;

private:
    int m_Id;
    QString m_Name;
    double m_Parameters;
};

}
