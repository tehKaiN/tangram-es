#ifndef _WX_TANGRAM_H_
#define _WX_TANGRAM_H_

#include <wx/wx.h>
#include <wx/glcanvas.h>

class wxTangram: public wxGLCanvas
{
public:
	wxTangram(wxWindow *parent,
	          wxWindowID id = wxID_ANY,
						const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = 0,
            const wxString& name = wxGLCanvasName);
private:
	wxGLContext *m_ctx;
	void OnPaint(wxPaintEvent &evt);
};

#endif // _WX_TANGRAM_H_