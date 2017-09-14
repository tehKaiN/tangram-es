#include "wxtangram.h"
#include <memory>
#include <algorithm>
#include "map.h"
#include "wxTangramPlatform.h"
#include "data/clientGeoJsonSource.h"

template <typename T> T clamp(T x, T min, T max)
{
	if(x < min)
		return min;
	if(x > max)
		return max;
	return x;
}

wxTangram::wxTangram(wxWindow *parent,
										wxWindowID id,
										const wxString& name,
										const wxString& api,
										const wxPoint& pos,
										const wxSize& size,
										long style):
	wxGLCanvas(parent, id, NULL, pos, size, style, name),
	m_api(api),
	m_ctx(new wxGLContext(this)),
	m_renderTimer(this)
{
	// Mouse events
	Bind(wxEVT_PAINT, OnPaint, this);
	Bind(wxEVT_LEFT_DOWN, OnMouseDown, this);
	Bind(wxEVT_LEFT_UP, OnMouseUp, this);
	Bind(wxEVT_MOTION, OnMouseMove, this);
	Bind(wxEVT_MOUSEWHEEL, OnMouseWheel, this);
	
	// Resize event
	Bind(wxEVT_SIZE, OnResize, this);
	
	// Render timer
	Bind(wxEVT_TIMER, OnRenderTimer, this);
	
	// Start render timer
	m_renderTimer.StartOnce(1000/60.0);
}

Tangram::Map &wxTangram::GetMap() {
	return *m_map;
}

void wxTangram::OnMouseWheel(wxMouseEvent &evt)
{
	if(!m_wasMapInit)
		return;
	if(evt.GetWheelAxis() != wxMOUSE_WHEEL_VERTICAL)
		return;

	double x = evt.GetX() * m_density;
	double y = evt.GetY() * m_density;
	int rotation = evt.GetWheelRotation() / evt.GetWheelDelta();
	m_map->handlePinchGesture(x, y, 1.0 + m_scrollSpanMultiplier * rotation, 0.f);
}

void wxTangram::OnResize(wxSizeEvent &evt)
{
	if(!m_wasMapInit)
		return;
	m_map->resize(GetSize().x, GetSize().y);
	Refresh();
}

void wxTangram::OnRenderTimer(wxTimerEvent &evt)
{
	wxStopWatch sw;
	Refresh();
	m_renderTimer.StartOnce(std::max(1.0, 1000/60.0 - sw.Time())); 
}

void wxTangram::OnMouseUp(wxMouseEvent &evt)
{
	if(!m_wasMapInit)
		return;
	if(evt.LeftUp()) {
		auto vx = clamp(m_lastXVelocity, -2000.0, 2000.0);
		auto vy = clamp(m_lastYVelocity, -2000.0, 2000.0);
		m_map->handleFlingGesture(
			evt.GetX() * m_density,
			evt.GetY() * m_density,
			vx, vy
		);
		m_wasPanning = false;
	}
}

void wxTangram::OnMouseDown(wxMouseEvent &evt)
{
	if(!m_wasMapInit)
		return;
	if(evt.LeftDown())
		m_lastTimeMoved = wxGetUTCTimeMillis().ToDouble()/1000.0;
}

void wxTangram::OnMouseMove(wxMouseEvent &evt)
{
	if(!m_wasMapInit)
		return;
	double x = evt.GetX() * m_density;
	double y = evt.GetY() * m_density;
	double time = wxGetUTCTimeMillis().ToDouble()/1000.0;

	if (evt.LeftIsDown()) {

		if (m_wasPanning) {
			m_map->handlePanGesture(m_lastPosDown.x, m_lastPosDown.y, x, y);
		}

		m_wasPanning = true;
		m_lastXVelocity = (x - m_lastPosDown.x) / (time - m_lastTimeMoved);
		m_lastYVelocity = (y - m_lastPosDown.y) / (time - m_lastTimeMoved);
	}
	else if(evt.RightIsDown()) {
		// Could be rotating around any point, e.g. one where RMB was pressed,
		// but it's kinda trippy when controlling using PC mouse.
		m_map->handleRotateGesture(
			m_map->getViewportWidth()/2, m_map->getViewportHeight()/2,
			-(x - m_lastPosDown.x) * 0.01
		);
		
		// handleShoveGesture() doesn't work, idk why.
		m_tilt += (m_lastPosDown.y - y) * 0.001 * 2*M_PI;
		m_tilt = clamp(m_tilt, 0.0, (90.0-12.0)/360.0*2*M_PI);
		m_map->setTilt(m_tilt);
	}

	m_lastPosDown.x = x;
	m_lastPosDown.y = y;
	m_lastTimeMoved = time;

}

wxTangram::~wxTangram(void)
{
	if(m_map != nullptr) {
		delete m_map;
	}
}

void wxTangram::Prerender(void)
{
	// Make GL context access exclusive
	if(m_renderMutex.TryLock() != wxMUTEX_NO_ERROR)
		return;
	
	if(IsShown()) {
		// Select GL context
		if(!SetCurrent(*m_ctx)) {
			m_renderMutex.Unlock();
			return;
		}
		
		// Load OpenGL
		if(!m_wasGlInit) {
			if (!gladLoadGL()) {
				Tangram::logMsg("GLAD: Failed to initialize OpenGL context");
				m_renderMutex.Unlock();
				return;
			}
			else {
				Tangram::logMsg("GLAD: Loaded OpenGL");
			}
			
			// Clear context with default background color.
			// TODO: something better
			Tangram::GL::clearColor(240/255.0f, 235/255.0f, 235/255.0f, 1.0f);
			SwapBuffers();
			m_wasGlInit = true;
		}
	
		// This may be not the only ctx in app, so set glViewport accordingly
		Tangram::GL::viewport(0, 0, GetSize().GetWidth(), GetSize().GetHeight());
		
		// Do the actual rendering
		Render();
		
		// Swap front and back buffers
		SwapBuffers();
	}

	m_renderMutex.Unlock();
}

void wxTangram::Render(void)
{
	// Get delta between frames
	double currentTime = wxGetUTCTimeMillis().ToDouble()/1000.0;
	double delta = currentTime - m_lastTime;
	m_lastTime = currentTime;
	
	// Render
	if(!m_wasMapInit) {
		// Setup Tangram
		std::string sceneFile = "scene.yaml";
		std::vector<Tangram::SceneUpdate> updates;
		updates.push_back(
			Tangram::SceneUpdate("global.sdk_mapzen_api_key", m_api.ToStdString())
		);
		
		// m_map construct must be here or else destruct will crash app by trying to
		// free GL buffers which weren't even allocated
		m_map = new Tangram::Map(std::make_shared<Tangram::wxTangramPlatform>(this));
		m_map->loadSceneAsync(sceneFile, true, updates);

		m_map->setupGL();
		m_map->resize(GetSize().x, GetSize().y);
		
		// This stuff must be here or else main project inheriting from
		// wxTangram will have link errors about undefined reference to
		// Tangram::GL functions. This is kinda hackish and could be fixed later.
		glClearColor(240/255.0f, 235/255.0f, 235/255.0f, 1.0f);
		m_wasMapInit = true;
	}
		
	try {
		m_map->update(delta);
		m_map->render();
	}
	catch(...) {
		Tangram::logMsg("TANGRAM: Unknown render error");
	}
}

void wxTangram::OnPaint(wxPaintEvent &evt)
{
	wxPaintDC(this); // Required here
	Prerender();
}
