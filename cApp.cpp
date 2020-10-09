
#include "cApp.h"

wxIMPLEMENT_APP(cApp);

cApp::cApp()
{

}

cApp::~cApp()
{
	curl_global_cleanup();
}

bool cApp::OnInit()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	m_frame1 = new cMain();
	m_frame1->Show();
	return true;
}
