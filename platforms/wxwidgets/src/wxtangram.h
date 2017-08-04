#ifndef _WX_TANGRAM_H_
#define _WX_TANGRAM_H_

#include "../../common/platform_gl.h"
#include <wx/wx.h>
#include <wx/glcanvas.h>
#include "../../../core/include/tangram/map.h"

class wxTangram: public wxGLCanvas
{
public:
	wxTangram(wxWindow *parent,
	          wxWindowID id = wxID_ANY,
						const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = 0,
            const wxString& name = wxGLCanvasName,
						const wxString& api = "");
	~wxTangram(void);
	void PaintNow(void);
	
protected:
	// Core stuff
	Tangram::Map* m_map = nullptr;
	bool m_wasInit = false;
	wxString m_api;
	
private:
	// Event handlers
	void OnPaint(wxPaintEvent &evt);
	void OnMouseDown(wxMouseEvent &evt);
	void OnMouseUp(wxMouseEvent &evt);
	void OnMouseMove(wxMouseEvent &evt);
	void OnRenderTimer(wxTimerEvent &evt);
	void OnResize(wxSizeEvent &evt);
	void Render(void);
	
	// Stuff for rendering
	wxGLContext *m_ctx;
	wxMutex m_renderMutex;
	wxTimer m_renderTimer;
	double m_lastTime;
	
	// Stuff for mouse nav
	double m_density = 1.0;
	wxPoint m_lastPosDown;
	bool m_wasPanning = false;
	double m_lastTimeMoved;
};

#endif // _WX_TANGRAM_H_