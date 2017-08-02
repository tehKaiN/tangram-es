#include "wxtangram.h"

wxTangram::wxTangram(wxWindow *parent,
										wxWindowID id,
										const wxPoint& pos,
										const wxSize& size,
										long style,
										const wxString& name)
	:wxGLCanvas(parent, id, NULL, pos, size, 0, name)
{
	m_ctx = new wxGLContext(this);
	Connect(id, wxEVT_PAINT, wxPaintEventHandler(OnPaint));
}

void wxTangram::OnPaint(wxPaintEvent &evt)
{
	wxPaintDC(this); // Required here
	m_ctx->SetCurrent(*this);
	
	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
	SwapBuffers();
}
