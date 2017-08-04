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
			std::vector<Tangram::FontSourceHandle> systemFontFallbacksHandle() const override;
			bool startUrlRequest(const std::string& _url, Tangram::UrlCallback _callback) override;
			void cancelUrlRequest(const std::string& _url) override;

	protected:
			Tangram::UrlClient m_urlClient;
			
	private:
		wxTangram *m_parent;
};

}