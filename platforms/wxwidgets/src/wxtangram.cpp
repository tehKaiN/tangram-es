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
										const wxPoint& pos,
										const wxSize& size,
										long style,
										const wxString& name,
										const wxString& api)
	:wxGLCanvas(parent, id, NULL, pos, size, 0, name),
	m_api(api),
	m_ctx(new wxGLContext(this)),
	m_renderTimer(this)
{
	// Mouse events
	Connect(id, wxEVT_PAINT, wxPaintEventHandler(OnPaint));
	Connect(id, wxEVT_LEFT_DOWN, wxMouseEventHandler(OnMouseDown));
	Connect(id, wxEVT_LEFT_UP, wxMouseEventHandler(OnMouseUp));
	Connect(id, wxEVT_MOTION, wxMouseEventHandler(OnMouseMove));
	Connect(id, wxEVT_MOUSEWHEEL, wxMouseEventHandler(OnMouseWheel));
	
	// Resize event
	Connect(id, wxEVT_SIZE, wxSizeEventHandler(OnResize));
	
	// Render timer
	Connect(id, wxEVT_TIMER, wxTimerEventHandler(OnRenderTimer));
	
	// Start render timer
	m_renderTimer.StartOnce(1000/60.0);
}

Tangram::Map &wxTangram::GetMap() {
	return *m_map;
}

void wxTangram::OnMouseWheel(wxMouseEvent &evt)
{
	if(evt.GetWheelAxis() != wxMOUSE_WHEEL_VERTICAL)
		return;

	double x = evt.GetX() * m_density;
	double y = evt.GetY() * m_density;
	bool rotating = wxGetKeyState(WXK_ALT);
	bool shoving = wxGetKeyState(WXK_SHIFT);
	int rotation = evt.GetWheelRotation() / evt.GetWheelDelta();

	if(shoving) {
		m_tilt += rotation * 0.05 * 2*M_PI;
		m_map->setTilt(m_tilt);
		// m_map->handleShoveGesture(m_scrollDistanceMultiplier * rotation);
	}
	else if(rotating) {
		m_map->handleRotateGesture(x, y, m_scrollSpanMultiplier * rotation);
	}
	else
		m_map->handlePinchGesture(x, y, 1.0 + m_scrollSpanMultiplier * rotation, 0.f);
}

void wxTangram::OnResize(wxSizeEvent &evt)
{
	if(!m_wasInit)
		return;
	m_map->resize(GetSize().x, GetSize().y);
	Update();
}

void wxTangram::OnRenderTimer(wxTimerEvent &evt)
{
	wxStopWatch sw;
	PaintNow();
	m_renderTimer.StartOnce(std::max(1.0, 1000/60.0 - sw.Time())); 
}

void wxTangram::OnMouseUp(wxMouseEvent &evt)
{
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
	if(evt.LeftDown()) {
		m_lastTimeMoved = wxGetUTCTimeMillis().ToDouble()/1000.0;
		m_lastPosDown.x = evt.GetX() * m_density;
		m_lastPosDown.y = evt.GetY() * m_density;
	}
}

void wxTangram::OnMouseMove(wxMouseEvent &evt)
{
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
		m_lastPosDown.x = x;
		m_lastPosDown.y = y;

	}

	m_lastTimeMoved = time;

}

wxTangram::~wxTangram(void)
{
	if(m_map != nullptr) {
		delete m_map;
	}
}

void wxTangram::PaintNow(void)
{
	wxClientDC dc(this);
	Render();
}

void wxTangram::Render(void)
{
	if(m_renderMutex.TryLock() != wxMUTEX_NO_ERROR)
		return;
	if(!SetCurrent(*m_ctx)) {
		m_renderMutex.Unlock();
		return;
	}
	// Get delta between frames
	double currentTime = wxGetUTCTimeMillis().ToDouble()/1000.0;
	double delta = currentTime - m_lastTime;
	m_lastTime = currentTime;
	
	// Render
	if(!m_wasInit) {
		if (!gladLoadGL()) {
				Tangram::logMsg("GLAD: Failed to initialize OpenGL context");
				m_renderMutex.Unlock();
				return;
		}
		else {
			Tangram::logMsg("GLAD: Loaded OpenGL");
		}
		
		// Setup Tangram
		std::string sceneFile = "scene.yaml";
		if (!m_map) {
				m_map = new Tangram::Map(std::make_shared<Tangram::wxTangramPlatform>(this));
				m_map->loadSceneAsync(sceneFile.c_str(), true, {}, nullptr,
								{Tangram::SceneUpdate("global.sdk_mapzen_api_key", m_api.ToStdString())});
		}

		m_map->setupGL();
		m_wasInit = true;
	}
	
	// This stuff must be here or else main project will have link errors about
	// undefined reference to Tangram::GL functions.
	// This is kinda hackish and should be fixed later.
	Tangram::GL::clearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// End of Tangram::GL hackfix
	
	m_map->update(delta);
	m_map->render();
	// Swap front and back buffers
	SwapBuffers();

	m_renderMutex.Unlock();
}

void wxTangram::OnPaint(wxPaintEvent &evt)
{
	wxPaintDC(this); // Required here
	Render();
}
