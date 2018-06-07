#ifndef _WX_TANGRAM_H_
#define _WX_TANGRAM_H_

#include <mutex>
#include <memory>
#include <wx/wx.h>
#include <wx/glcanvas.h>
#include "../../../core/include/tangram/tangram.h"

// So that project using those map doesn't need to include own GLM
#define GLM_ENABLE_EXPERIMENTAL
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

	Tangram::Map &GetMap();

protected:
	// Core stuff
	bool m_wasGlInit = false;
	bool m_wasMapInit = false;
	wxString m_api;
	wxString m_sceneFile;
	std::shared_ptr<wxGLContext> m_ctx;
	std::shared_ptr<Tangram::Map> m_map;

	virtual void Render(void);

private:
	// Event handlers
	void OnPaint(wxPaintEvent &evt);
	void OnMouseDown(wxMouseEvent &evt);
	void OnMouseUp(wxMouseEvent &evt);
	void OnMouseMove(wxMouseEvent &evt);
	void OnMouseWheel(wxMouseEvent &evt);
	void OnIdle(wxIdleEvent &evt);
	void OnResize(wxSizeEvent &evt);

	void Prerender(void);

	// Stuff for rendering
	std::mutex m_renderMutex;
	double m_lastTime;

	// Stuff for mouse nav
	double m_density = 1.0;
	wxPoint m_lastPosDown;
	bool m_wasPanning = false;
	bool m_isRenderEnabled = true;
	double m_lastTimeMoved;
	double m_lastXVelocity;
	double m_lastYVelocity;

	const double m_scrollSpanMultiplier = 0.1; // scaling for zoom and rotation
	const double m_scrollDistanceMultiplier = 0.1; // scaling for shove
};

#endif // _WX_TANGRAM_H_
