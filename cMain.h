#pragma once

#include "wx/wx.h"
#include <curl/curl.h>
#include "nlohmann/json.hpp"
#include <string>
#include <iostream>
#include <thread>
#include <fstream>
#include <streambuf>
#include "icon.xpm"
#include <wx/aboutdlg.h>
#include <wx/listctrl.h>
#include <wx/custombgwin.h>
#include <wx/checklst.h>

using json = nlohmann::json;

#define TIME_IN_US 1 /* microseconds */
#define TIMETYPE curl_off_t
#define TIMEOPT CURLINFO_TOTAL_TIME_T
#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL 1000000

const std::string USER_AGENT = "smotret-anime-client";

struct transl;

class cMain : public wxFrame
{
public:
	/*
	 * Creates main window
	*/
	cMain();

	/*
	 * Delete main window
	*/
	~cMain();
public:
	bool do_logout;
	CURL* curl_handle;
	CURL* curl_handle_cookies;
	CURLM* curl_multi_handle;
	std::vector <long long> epIDs;  // id эпизодов
	std::vector <long long> animeIDs;  // id аниме
	std::vector <long long> selTranslIDs;  // id выбранных переводов
	std::vector <std::string> animeNames;
	std::string choosedTitleName;
	std::vector <std::vector<transl>> epTransl;  // данные о переводах каждой серии
	size_t cur_ep_index;  // выбранный сейчас эпизод (его индекс)
	std::vector <std::string> epNames;  // названия серий
	wxPanel* main_panel;
	wxButton* button_search_anime;
	wxButton* button_download_anime;
	wxTextCtrl* entry_anime_name;
	wxListBox* choose_episode;
	wxListBox* choose_anime;
	wxCheckListBox* choose_episodes_to_download;
	wxTextEntryDialog* saLoginDialog;
	wxPasswordEntryDialog* saPasswordDialog;
	wxMenuBar* top_menu_bar;
	wxMenu* top_menu;
	wxChoice* choice_default_transl;
	wxListCtrl* translates_list;
	wxListCtrl* downloads_list;
	wxCheckBox* download_only_subs_checkbox;
	/*
	 * Handles button "Search" press event
	*/
	void onButtonSearchAnime(wxCommandEvent& evt);
	/*
	 * Handles button "Download" press event
	*/
	void onButtonDownloadAnime(wxCommandEvent& evt);
	/*
	 * Handles button "Select" press event
	*/
	void onButtonSelectTitle(wxCommandEvent& evt);
	/*
	 * Execs auth thread
	*/
	void execSaAuth();
	/*
	 * Ctartup exec of libcurl
	*/
	void execCURL();
	/*
	 * Handles anime entry change
	*/
	void onAnimeEntryChange(wxCommandEvent& evt);
	/*
	 * Handles check list events
	*/
	void onCheckListChecked(wxCommandEvent& evt);
	/*
	 * Handles top menu info
	*/
	void onTopMenuInfo(wxCommandEvent& evt);
	/*
	 * Handles choose episode event
	*/
	void onEpisodeChoosen(wxCommandEvent& evt);
	/*
	 * Handles choose translation event
	*/
	void onTranslationChoosen(wxListEvent& evt);
	/*
	 * Logout from SA
	*/
	void onlogoutSA(wxCommandEvent& evt);
	/*
	 * Handle downloads list right click
	*/
	void OnDownloadsListRightClick(wxListEvent& evt);
	/*
	 * Handle popup menu right click
	*/
	void OnPopupClick(wxCommandEvent& evt);
	/*
	 * Handle enable main_panel event
	*/
	void OnEnableMainPanel(wxCommandEvent& evt);
	/*
	 * Handle clear gui for search event
	*/
	void OnClearGuiForSearch(wxCommandEvent& evt);
	/*
	 * Handle append to animes list event
	*/
	void OnAppendToAnimesList(wxCommandEvent& evt);
	/*
	 * Handle clear gui for choosed anime event
	*/
	void OnClearGuiForChoosedAnime(wxCommandEvent& evt);
	/*
	 * Handle append to choose episodes to download event
	*/
	void OnChooseEpisodesToDownloadAppend(wxCommandEvent& evt);
	/*
	 * Handle clear translates list event
	*/
	void OnClearTranslatesList(wxCommandEvent& evt);
	/*
	 * Handle insert to translates event
	*/
	void OnInsertToTranslatesList(wxCommandEvent& evt);
	/*
	 * Handle set download status event
	*/
	void onSetDownloadStasus(wxCommandEvent& evt);
	/*
	 * Handle set download params event
	*/
	void onSetDownloadParams(wxCommandEvent& evt);
	/*
	 * Handle set download params progress event
	*/
	void onSetDownloadParamsProgress(wxCommandEvent& evt);
	/*
	 * Handle set downloads list insert event
	*/
	void onDownloadsListInsert(wxCommandEvent& evt);
	static const int ID_WXPANEL_MAIN = 10001;
	static const int ID_ENTRY_ANIME_NAME = 10002;
	static const int ID_BUTTON_SEARCH_ANIME = 10003;
	static const int ID_CHOOSE_EPISODE = 10004;
	static const int ID_BUTTON_DOWNLOAD_ANIME = 10005;
	static const int ID_CHOOSE_TRANSLATION = 10006;
	static const int ID_BUTTON_DOWNLOAD_TRANSLATIONS = 10007;
	static const int ID_CHOOSE_ANIME = 10008;
	static const int ID_CHOOSE_EPISODES_TO_DOWNLOAD = 10009;
	static const int ID_TOP_MENU_INFO = 10010;
	static const int ID_CHOICE_DEFAULT_TRANSL = 10011;
	static const int ID_TRANSLATES_LIST = 10012;
	static const int ID_DOWNLOADS_LIST = 10013;
	static const int ID_TOP_MENU_LOGOUT = 10014;
	static const int ID_CLEAR_GUI_FOR_SEARCH = 10015;
	static const int ID_CLEAR_GUI_FOR_CHOOSED_ANIME = 10016;
	static const int ID_TRANSLATES_LIST_INSERT = 10017;
	static const int ID_TRANSLATES_LIST_STATUS = 10018;
	static const int ID_TRANSLATES_LIST_PARAMS = 10019;
	static const int ID_TRANSLATES_LIST_PARAMS_PROGRESS = 10020;
	static const int ID_DOWNLOADS_LIST_INSERT = 10021;
	static const int ID_DOWNLOAD_ONLY_SUBS_CHECKBOX = 10022;
	wxDECLARE_EVENT_TABLE();
};

class MyPanel : public wxCustomBackgroundWindow<wxPanel>
{
public:
	MyPanel(wxWindow* parent)
	{
		Create(parent, wxID_ANY);
		wxInitAllImageHandlers();
		wxBitmap* bitmap = new wxBitmap;
		bitmap->LoadFile("background.jpg", wxBITMAP_TYPE_JPEG);
		SetBackgroundBitmap(*bitmap);
		delete bitmap;
	}
};

struct myprogress {
	TIMETYPE lastruntime;
	CURL* curl;
	cMain* cMain;
	curl_off_t lastdownloaded;
	long long index;
};

struct links {
	std::string videoLink;
	std::string subsLink;
	std::string quality;
};

struct episode {
	long long id;
	std::string name;
};

struct transl {
	long long id;
	std::string translName;
	double duration;
	long long size;
	std::string quality;
	links tr_links;
};

/*
 * Writes contents to std::string
 * Used in curl requests
*/
static size_t WriteCallbackToString(void* contents, size_t size, size_t nmemb, void* userp);

/*
 * Don't writes contents
*/
static size_t DontWriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

/*
 * Make request using curl and url
 */
bool get(CURL* curl, const char url[]);

/*
 * Make request to SA api and saves results to readBuffer
*/
bool requestSAapi(CURL* curl_handle, std::string& request_url, std::string& readBuffer);

/*
 * Encodes str to url format
*/
void encodeStringToURL(CURL* curl_handle, std::string& str);

/*
 * Parse episodes list
*/
void parseAnimes(cMain* cMain, std::string& readBuffer);

/*
 * Search for anime in SA database
*/
void findAnimeSA(cMain* cMain, CURL* curl_handle, std::string animename);

/*
 * Parse translations info from SA episode json
*/
void parseTranslations(cMain* cMain, CURL* curl_handle, std::string& readBuffer, size_t index);

/*
 * Parses direct link to video from Sa embeded player
*/
std::string parseLinkToVideo(const std::string& page);

/*
 * Parses direct link to subs from Sa embeded player
*/
std::string parseLinkToSubs(const std::string& page);

/*
 * Parse direct download link to video and subs from SA embeded player
*/
links parseDirectDownloadLink(std::string& readBuffer);

/*
 * Request to SA using cookies to get links
*/
bool requestSAWithCookies(CURL* curl_handle_cookies, std::string& request_url, std::string& readBuffer);

/*
 * Get direct download links to given trID
*/
links getDirectDownloadLinks(cMain* cMain, CURL* curl_handle, long long epID);

/*
 * Download choosen episodes from SA
*/
void dowloadAnimeSA(cMain* cMain, CURL* curl_handle, size_t n, wxArrayInt checkedEps, size_t index, bool is_download_only_subs);

/*
 * Adds easy_handle to multi_handle
*/
void addHandle(cMain* cMain, const std::string& url, FILE* f, long long index);

/*
 * Works on downloading files in multi_handle
*/
void multiDownload(cMain* cMain);

/*
 * Fast check if file exists
*/
inline bool fileExistsTest(const std::string& name);

/*
 * Checks if cookie file exists
*/
bool checkSaAuth();

/*
 * Gets cookies from SA and writes to file "cookies"
*/
void saAuth(const std::string& email, const std::string& password);

/*
 * Reads all file contents to std::string
*/
std::string readAllFileContents(const std::string& filename);

/*
 * Downloads episodes for choosen title
*/
void findEpisodesSA(cMain* cMain, CURL* curl_handle, size_t ind);

/*
 * Parse episode names and ids
*/
void parseEpisodes(cMain* cMain, std::string& readBuffer);

/*
 * Find ep translations
*/
void findEpTranslations(cMain* cMain, CURL* curl_handle, size_t index, wxArrayInt checkedEps);

/*
 * Downloads ep info
*/
void getEpInfo(cMain* cMain, CURL* curl_handle, size_t index);

/*
 * Refresh transl info
*/
void refreshTranslatesList(cMain* cMain, std::vector<transl>& info);

/*
 * Gets transl size
*/
curl_off_t getTranslSize(std::string& url);

/*
 * Parse video quality poop
*/
std::string parseVideoQuality(std::string& page);

/*
 * Cmp for sorting transl
*/
bool trCmp(const transl& a, const transl& b);

/*
 * Progress curl func
*/
static int xferinfo(void* p,
	curl_off_t dltotal, curl_off_t dlnow,
	curl_off_t ultotal, curl_off_t ulnow);

/*
 * Enable main_panel for cMain by event
*/
void enableMainPanel(cMain* cMain);
