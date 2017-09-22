#include "wxtangramplatform.h"
#include "gl/hardware.h"
#include "log.h"
#include <stdio.h>
#include <libgen.h>
#include <unistd.h>
#include <cstdarg>

#define DEFAULT "fonts/NotoSans-Regular.ttf"
#define FONT_AR "fonts/NotoNaskh-Regular.ttf"
#define FONT_HE "fonts/NotoSansHebrew-Regular.ttf"
#define FONT_JA "fonts/DroidSansJapanese.ttf"
#define FALLBACK "fonts/DroidSansFallback.ttf"

namespace Tangram {

void logMsg(const char* fmt, ...) {
	// Direct displaying of strings via wxLogMessage makes funny chars
	char bfr[2048];
	va_list args;
	va_start(args, fmt);
	vsprintf(bfr, fmt, args);
	
	// Trim whitespace
	char *p = &bfr[strlen(bfr)-1];
	while(p != bfr && *p < 32)
		--p;
	*(p+1) = 0;
	
	// Display
	wxLogMessage(bfr);
	va_end(args);
}

wxTangramPlatform::wxTangramPlatform(wxTangram *parent):
	wxTangramPlatform(parent, Tangram::UrlClient::Options{})
{
}

wxTangramPlatform::wxTangramPlatform(wxTangram *parent, Tangram::UrlClient::Options urlClientOptions):
	m_urlClient(urlClientOptions),
	m_parent(parent)
{
}

wxTangramPlatform::~wxTangramPlatform() {}

void wxTangramPlatform::requestRender() const
{
	m_parent->Refresh();
}

std::vector<Tangram::FontSourceHandle> wxTangramPlatform::systemFontFallbacksHandle() const
{
	std::vector<Tangram::FontSourceHandle> handles;

	handles.emplace_back(DEFAULT);
	handles.emplace_back(FONT_AR);
	handles.emplace_back(FONT_HE);
	handles.emplace_back(FONT_JA);
	handles.emplace_back(FALLBACK);

	return handles;
}

bool wxTangramPlatform::startUrlRequest(const std::string& _url, Tangram::UrlCallback _callback)
{

	return m_urlClient.addRequest(_url, _callback);
}

void wxTangramPlatform::cancelUrlRequest(const std::string& _url) {

	m_urlClient.cancelRequest(_url);
}


void setCurrentThreadPriority(int priority) {
	// TODO: wxThread::SetPriority() could be used
	// WXTHREAD_MIN_PRIORITY = 0, WXTHREAD_MAX_PRIORITY = 100,
	// WXTHREAD_DEFAULT_PRIORITY = 50
	// But not sure if setting priority of main thread is wise
}

void initGLExtensions() {
	Tangram::Hardware::supportsMapBuffer = true;
}

} // namespace Tangram