#ifndef _WX_TANGRAM_H_
#define _WX_TANGRAM_H_

#include "../../common/platform_gl.h"
#include <wx/wx.h>
#include <wx/glcanvas.h>
#include "../../../core/include/tangram/tangram.h"

// So that project using those map doesn't need to include own GLM
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/transform.hpp"

class wxTangram: public wxGLCanvas
{
public:
	wxTangram(wxWindow *parent,
	          wxWindowID id = wxID_ANY,
						const wxString& name = wxGLCanvasName,
						const wxString& api = "",
						const wxString& sceneFile = "scene.yaml",
						const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = 0);
	~wxTangram(void);
	Tangram::Map &GetMap();
	
protected:
	// Core stuff
	bool m_wasGlInit = false;
	bool m_wasMapInit = false;
	wxString m_api;
	wxString m_sceneFile;
	wxGLContext *m_ctx;
	Tangram::Map* m_map = nullptr;
	
	virtual void Render(void);
	
private:
	// Event handlers
	void OnPaint(wxPaintEvent &evt);
	void OnMouseDown(wxMouseEvent &evt);
	void OnMouseUp(wxMouseEvent &evt);
	void OnMouseMove(wxMouseEvent &evt);
	void OnMouseWheel(wxMouseEvent &evt);
	void OnRenderTimer(wxTimerEvent &evt);
	void OnResize(wxSizeEvent &evt);
	
	void Prerender(void);
	
	// Stuff for rendering
	wxMutex m_renderMutex;
	wxTimer m_renderTimer;
	double m_lastTime;
	
	// Stuff for mouse nav
	double m_density = 1.0;
	wxPoint m_lastPosDown;
	bool m_wasPanning = false;
	double m_lastTimeMoved;
	double m_lastXVelocity;
	double m_lastYVelocity;
		
	const double m_scrollSpanMultiplier = 0.1; // scaling for zoom and rotation
	const double m_scrollDistanceMultiplier = 0.1; // scaling for shove
};

#endif // _WX_TANGRAM_H_