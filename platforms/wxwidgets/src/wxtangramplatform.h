#pragma once

#include <memory>
#include "platform.h"
#include "../../common/urlClient.h"
#include "wxtangram.h"

namespace Tangram {

class wxTangramPlatform : public Tangram::Platform
{
public:
    wxTangramPlatform(wxTangram *parent);
    wxTangramPlatform(wxTangram *parent, Tangram::UrlClient::Options urlClientOptions);
    ~wxTangramPlatform() override;
    void requestRender() const override;
    std::vector<FontSourceHandle> systemFontFallbacksHandle() const override;
    UrlRequestHandle startUrlRequest(Url _url, UrlCallback _callback) override;
    void cancelUrlRequest(UrlRequestHandle _request) override;

protected:
    Tangram::UrlClient m_urlClient;

private:
    wxTangram *m_parent;
};

}