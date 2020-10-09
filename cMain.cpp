
#include "cMain.h"

wxDEFINE_EVENT(MY_EVENT_TYPE, wxCommandEvent);

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
	EVT_TEXT(ID_ENTRY_ANIME_NAME, onAnimeEntryChange)
	EVT_BUTTON(ID_BUTTON_SEARCH_ANIME, onButtonSearchAnime)
	EVT_TEXT_ENTER(ID_ENTRY_ANIME_NAME, onButtonSearchAnime)
	EVT_LISTBOX_DCLICK(ID_CHOOSE_ANIME, onButtonSelectTitle)
	EVT_CHECKLISTBOX(ID_CHOOSE_EPISODES_TO_DOWNLOAD, onCheckListChecked)
	EVT_MENU(ID_TOP_MENU_INFO, onTopMenuInfo)
	EVT_MENU(ID_TOP_MENU_LOGOUT, onlogoutSA)
	EVT_LISTBOX(ID_CHOOSE_EPISODE, onEpisodeChoosen)
	EVT_LIST_ITEM_SELECTED(ID_TRANSLATES_LIST, onTranslationChoosen)
	EVT_BUTTON(ID_BUTTON_DOWNLOAD_ANIME, onButtonDownloadAnime)
	EVT_LIST_ITEM_RIGHT_CLICK(ID_DOWNLOADS_LIST, OnDownloadsListRightClick)
	EVT_COMMAND(ID_WXPANEL_MAIN, MY_EVENT_TYPE, OnEnableMainPanel)
	EVT_COMMAND(ID_CLEAR_GUI_FOR_SEARCH, MY_EVENT_TYPE, OnClearGuiForSearch)
	EVT_COMMAND(ID_CHOOSE_ANIME, MY_EVENT_TYPE, OnAppendToAnimesList)
	EVT_COMMAND(ID_CLEAR_GUI_FOR_CHOOSED_ANIME, MY_EVENT_TYPE, OnClearGuiForChoosedAnime)
	EVT_COMMAND(ID_CHOOSE_EPISODES_TO_DOWNLOAD, MY_EVENT_TYPE, OnChooseEpisodesToDownloadAppend)
	EVT_COMMAND(ID_TRANSLATES_LIST, MY_EVENT_TYPE, OnClearTranslatesList)
	EVT_COMMAND(ID_TRANSLATES_LIST_INSERT, MY_EVENT_TYPE, OnInsertToTranslatesList)
	EVT_COMMAND(ID_TRANSLATES_LIST_STATUS, MY_EVENT_TYPE, onSetDownloadStasus)
	EVT_COMMAND(ID_TRANSLATES_LIST_PARAMS, MY_EVENT_TYPE, onSetDownloadParams)
	EVT_COMMAND(ID_TRANSLATES_LIST_PARAMS_PROGRESS, MY_EVENT_TYPE, onSetDownloadParamsProgress)
	EVT_COMMAND(ID_DOWNLOADS_LIST_INSERT, MY_EVENT_TYPE, onDownloadsListInsert)
wxEND_EVENT_TABLE()

cMain::cMain() : wxFrame(nullptr, wxID_ANY, "Anime", wxPoint(0, 0), wxSize(800, 900), (wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))) {
	execCURL();
	this->SetIcon(wxIcon(icon));
	Centre();
	do_logout = false;
	saLoginDialog = new wxTextEntryDialog(this, "Email", "smotret-anime.online auth", "", (wxOK | wxCENTRE | wxWS_EX_VALIDATE_RECURSIVELY));
	saPasswordDialog = new wxPasswordEntryDialog(this, "Password", "smotret-anime.online auth", "", (wxOK | wxCENTRE | wxWS_EX_VALIDATE_RECURSIVELY));
	main_panel = new MyPanel(this);
	main_panel->SetId(ID_WXPANEL_MAIN);
	button_search_anime = new wxButton(main_panel, ID_BUTTON_SEARCH_ANIME, "Search", wxPoint(325, 243), wxSize(90, 35));
#ifdef _DEBUG
	entry_anime_name = new wxTextCtrl(main_panel, ID_ENTRY_ANIME_NAME, "Yesterday wo Utatte", wxPoint(50, 250), wxSize(265, 20), wxTE_PROCESS_ENTER);
#else
	entry_anime_name = new wxTextCtrl(main_panel, ID_ENTRY_ANIME_NAME, "", wxPoint(50, 250), wxSize(265, 20), wxTE_PROCESS_ENTER);
#endif
	choose_anime = new wxListBox(main_panel, ID_CHOOSE_ANIME, wxPoint(25, 25), wxSize(400, 200));
	choose_episodes_to_download = new wxCheckListBox(main_panel, ID_CHOOSE_EPISODES_TO_DOWNLOAD, wxPoint(470, 25), wxSize(250, 200));
	top_menu = new wxMenu();
	top_menu->Append(ID_TOP_MENU_INFO, "Info");
	top_menu->Append(ID_TOP_MENU_LOGOUT, "Logout");
	top_menu_bar = new wxMenuBar();
	top_menu_bar->Append(top_menu, "Menu");
	SetMenuBar(top_menu_bar);
	CreateStatusBar(3);
	SetStatusText(wxT("Ready"), 0);
	// SetStatusText(wxT("stewkk"), 1);
	choose_episode = new wxListBox(main_panel, ID_CHOOSE_EPISODE, wxPoint(25, 300), wxSize(250, 200));
	wxString choice_settings_names[2] = { "By size" };
	choice_default_transl = new wxChoice(main_panel, ID_CHOICE_DEFAULT_TRANSL, wxPoint(470, 250), wxSize(100, 25), 1, choice_settings_names);
	choice_default_transl->SetSelection(0);
	choice_default_transl->Disable();
	translates_list = new wxListCtrl(main_panel, ID_TRANSLATES_LIST, wxPoint(300, 300), wxSize(450, 200), wxLC_REPORT | wxLC_SINGLE_SEL);
	translates_list->AppendColumn("Author", wxLIST_FORMAT_LEFT, 190);
	translates_list->AppendColumn("Size", wxLIST_FORMAT_LEFT, 80);
	translates_list->AppendColumn("Quality", wxLIST_FORMAT_LEFT, 80);
	translates_list->AppendColumn("Duration", wxLIST_FORMAT_LEFT, 80);
	button_download_anime = new wxButton(main_panel, ID_BUTTON_DOWNLOAD_ANIME, "Download", wxPoint(600, 243), wxSize(90, 35));
	downloads_list = new wxListCtrl(main_panel, ID_DOWNLOADS_LIST, wxPoint(25, 525), wxSize(725, 250), wxLC_REPORT | wxLC_SINGLE_SEL);
	downloads_list->AppendColumn("Name", wxLIST_FORMAT_LEFT, 190);
	downloads_list->AppendColumn("Size", wxLIST_FORMAT_LEFT, 80);
	downloads_list->AppendColumn("Progress", wxLIST_FORMAT_LEFT, 80);
	downloads_list->AppendColumn("Status", wxLIST_FORMAT_LEFT, 80);
	downloads_list->AppendColumn("Speed", wxLIST_FORMAT_LEFT, 80);
	downloads_list->AppendColumn("Time passed", wxLIST_FORMAT_LEFT, 80);
	downloads_list->AppendColumn("Time left", wxLIST_FORMAT_LEFT, 80);
	download_only_subs_checkbox = new wxCheckBox(main_panel, ID_DOWNLOAD_ONLY_SUBS_CHECKBOX, "Subs only", wxPoint(470, 275), wxSize(100, 25));

	if (!checkSaAuth()) {
		execSaAuth();
	}
}

cMain::~cMain() {
	saLoginDialog->Destroy();
	saPasswordDialog->Destroy();
	curl_easy_cleanup(curl_handle);
	curl_easy_cleanup(curl_handle_cookies);
	curl_multi_cleanup(curl_multi_handle);
	if (do_logout) {
		std::remove("cookies");
	}
}

static size_t WriteCallbackToString(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

static size_t DontWriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	return size * nmemb;
}

bool get(CURL* curl, const char url[]) {
	curl_easy_setopt(curl, CURLOPT_URL, url);
	CURLcode res = curl_easy_perform(curl);
	long response_code;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
	return res == CURLE_OK && response_code == 200;
}

bool requestSAapi(CURL* curl_handle, std::string& request_url, std::string& readBuffer) {
	if (curl_handle) {
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &readBuffer);
		return get(curl_handle, request_url.c_str());
	}
	return false;
}

void encodeStringToURL(CURL* curl_handle, std::string& str) {
	char* output = curl_easy_escape(curl_handle, str.c_str(), str.size());
	str = output;
	curl_free(output);
}

void parseAnimes(cMain* cMain, std::string& readBuffer) {
	json j = json::parse(readBuffer);
	j = j["data"];
	wxCommandEvent* event_clear = new wxCommandEvent(MY_EVENT_TYPE, cMain->ID_CLEAR_GUI_FOR_SEARCH);
	wxQueueEvent(cMain, event_clear);
	cMain->epIDs.clear();
	cMain->animeIDs.clear();
	cMain->animeNames.clear();
	cMain->epNames.clear();
	for (auto& anime : j) {
		json temp_json = anime["id"];
		if (temp_json.is_number()) {
			long long id = temp_json.get<long long>();
			std::string title;
			temp_json = anime["titles"]["romaji"];
			std::string romanji_title = temp_json.get<std::string>();
			temp_json = anime["titles"]["ru"];
			if (temp_json.is_string()) {
				title = temp_json.get<std::string>();
			} else {
				title = romanji_title;
			}
			cMain->animeNames.push_back(romanji_title);
			cMain->animeIDs.push_back(id);
			wxCommandEvent* event_append = new wxCommandEvent(MY_EVENT_TYPE, cMain->ID_CHOOSE_ANIME);
			event_append->SetString(title);
			std::string* temp = new std::string;
			*temp = title;
			event_append->SetClientData(temp);
			wxQueueEvent(cMain, event_append);
		}
	}
}

void findAnimeSA(cMain* cMain, CURL* curl_handle, std::string animename) {
	encodeStringToURL(curl_handle, animename);
	std::string readBuffer;
#ifdef _DEBUG
	std::string request_url = "https://smotret-anime.online/api/series/?pretty=1&query=" + animename;
#else
	std::string request_url = "https://smotret-anime.online/api/series/?query=" + animename;
#endif
	if (requestSAapi(curl_handle, request_url, readBuffer) && !readBuffer.empty()) {
		parseAnimes(cMain, readBuffer);
	}
	enableMainPanel(cMain);
}

void parseTranslations(cMain* cMain, CURL* curl_handle, std::string& readBuffer, size_t index) {
	json j = json::parse(readBuffer);
	j = j["data"]["translations"];
	for (auto& tr : j) {
		if (tr["type"].get<std::string>() == "subRu" && tr["isActive"].get<long long>()) {
			std::string authorsSummary = tr["authorsSummary"].get<std::string>();
			std::string qualityType = tr["qualityType"].get<std::string>();
			long long trID = tr["id"].get<long long>();
			long long duration = stod(tr["duration"].get<std::string>());
			if (authorsSummary.empty()) {
				authorsSummary = "unknown";
			}
			long long epID = cMain->epIDs[index];
			transl ep;
			ep.tr_links = getDirectDownloadLinks(cMain, curl_handle, trID);
			if (!ep.tr_links.videoLink.empty()) {
				ep.size = getTranslSize(ep.tr_links.videoLink);
				if (ep.size) {
					ep.translName = authorsSummary;
					ep.quality = ep.tr_links.quality;
					ep.duration = duration;
					ep.id = trID;
					cMain->epTransl[index].push_back(ep);
				}
			}
			sort(cMain->epTransl[index].rbegin(), cMain->epTransl[index].rend(), trCmp);
		}
	}
}

bool trCmp(const transl& a, const transl& b) {
	return a.size < b.size;
}

curl_off_t getTranslSize(std::string& url) {
	CURL* curl_handle = curl_easy_init();
	std::string readBuffer;
	curl_off_t size = 0;
	if (curl_handle) {
		curl_easy_setopt(curl_handle, CURLOPT_NOBODY, 1L);
		requestSAapi(curl_handle, url, readBuffer);
		CURLcode res = curl_easy_getinfo(curl_handle, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &size);
		long response_code;
		curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
		curl_easy_cleanup(curl_handle);
		if (response_code != 200) {
			return 0;
		}
	}
	return size;
}

std::string parseLinkToVideo(const std::string& page) {
	std::string ans = "";
	size_t i = page.find("&quot;urls&quot;:[&quot;");
	if (i != std::string::npos) {
		i += 24;
		size_t j = page.find("&quot;", i);
		ans = page.substr(i, j - i);
		ans.erase(ans.find("amp;"), 4);
		i = ans.find('\\');
		while (i != std::string::npos) {
			ans.erase(i, 1);
			i = ans.find('\\');
		}
	}
	return ans;
}

std::string parseLinkToSubs(const std::string& page) {
	size_t i = page.find("data-subtitles=\"");
	i += 16;
	size_t j = page.find('"', i);
	std::string ans = page.substr(i, j - i);
	return ans;
}

links parseDirectDownloadLink(std::string& readBuffer) {
	std::string linkToVideo = parseLinkToVideo(readBuffer);
	std::string linkToSubs = parseLinkToSubs(readBuffer);
	return { linkToVideo, linkToSubs };
}

bool requestSAWithCookies(CURL* curl_handle_cookies, std::string& request_url, std::string& readBuffer) {
	if (curl_handle_cookies) {
		curl_easy_setopt(curl_handle_cookies, CURLOPT_WRITEDATA, &readBuffer);
		return get(curl_handle_cookies, request_url.c_str());
	}
	return false;
}

links getDirectDownloadLinks(cMain* cMain, CURL* curl_handle, long long epID) {
	links directDownloadLinks;
	std::string request_url = "https://smotret-anime.online/translations/embed/" + std::to_string(epID);
	std::string readBuffer;
	if (requestSAWithCookies(cMain->curl_handle_cookies, request_url, readBuffer) && !readBuffer.empty()) {
		links res = parseDirectDownloadLink(readBuffer);
		std::string quality = parseVideoQuality(readBuffer);
		res.quality = quality;
		directDownloadLinks = res;
	}
	return directDownloadLinks;
}

std::string parseVideoQuality(std::string& page) {
	std::string ans = "";
	size_t i = page.find("&quot;height&quot;:");
	if (i != std::string::npos) {
		i += 19;
		size_t j = page.find(",&quot;", i);
		ans = page.substr(i, j - i);
	}
	return ans;
}

static int xferinfo(void* p,
	curl_off_t dltotal, curl_off_t dlnow,
	curl_off_t ultotal, curl_off_t ulnow) {
	struct myprogress* myp = (struct myprogress*)p;
	CURL* curl = myp->curl;
	TIMETYPE curtime = 0;
	curl_easy_getinfo(curl, TIMEOPT, &curtime);
	if ((curtime - myp->lastruntime) >= MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL) {
		std::string* params = new std::string[3];
		params[0] = std::to_string((long long)(((dlnow - myp->lastdownloaded) / 1024.0 / 1024.0) / (((double)curtime -
        myp->lastruntime) / 1000000.0))) + "MB/s";
		params[1] = std::to_string((long long)((dltotal - dlnow) / (dlnow / (curtime / 1000000.0))));
		params[2] = std::to_string(curtime / 1000000);
		wxCommandEvent* event_insert = new wxCommandEvent(MY_EVENT_TYPE, myp->cMain->ID_TRANSLATES_LIST_PARAMS);
		event_insert->SetInt(myp->index);
		event_insert->SetClientData(params);
		wxQueueEvent(myp->cMain, event_insert);
		//myp->cMain->downloads_list->SetItem(myp->index, 4, std::to_string(long long(((dlnow - myp->lastdownloaded) / 1024.0 / 1024.0)
		//	/ (((double)curtime - myp->lastruntime) / 1000000.0))) + "MB/s");  // current download speed
		//myp->cMain->downloads_list->SetItem(myp->index, 6, std::to_string(long long((dltotal - dlnow) / (dlnow / (curtime / 1000000.0)))));  // time left
		//myp->cMain->downloads_list->SetItem(myp->index, 5, std::to_string(curtime / 1000000));  // current time
		myp->lastruntime = curtime;
		myp->lastdownloaded = dlnow;
	}
	wxCommandEvent* event_insert = new wxCommandEvent(MY_EVENT_TYPE, myp->cMain->ID_TRANSLATES_LIST_PARAMS_PROGRESS);
	event_insert->SetInt(myp->index);
	event_insert->SetString(std::to_string((int)((float)dlnow / dltotal * 100)) + "%");
	//myp->cMain->downloads_list->SetItem(myp->index, 2, std::to_string((int)((float)dlnow / dltotal * 100)) + "%");  // progress
	wxQueueEvent(myp->cMain, event_insert);
	return 0;
}

void addHandle(cMain* cMain, const std::string& url, FILE* f, long long index) {
	CURL* easy_handle = curl_easy_init();
	myprogress* prog = new myprogress;
	prog->lastruntime = 0;
	prog->curl = easy_handle;
	prog->cMain = cMain;
	prog->index = index;
	if (index != -1) {
		curl_easy_setopt(easy_handle, CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(easy_handle, CURLOPT_XFERINFOFUNCTION, xferinfo);
		curl_easy_setopt(easy_handle, CURLOPT_XFERINFODATA, prog);
	}
	curl_easy_setopt(easy_handle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, f);
	curl_easy_setopt(easy_handle, CURLOPT_PRIVATE, prog);
	curl_multi_add_handle(cMain->curl_multi_handle, easy_handle);
}

void multiDownload(cMain* cMain) {
	int still_running = 0;
	do {
		CURLMcode mc;
		int numfds;

		mc = curl_multi_perform(cMain->curl_multi_handle, &still_running);

		CURLMsg* m;
		do {
			int msgq = 0;
			m = curl_multi_info_read(cMain->curl_multi_handle, &msgq);
			if (m && (m->msg == CURLMSG_DONE)) {
				CURL* e = m->easy_handle;
				myprogress* prog = nullptr;
				curl_easy_getinfo(e, CURLINFO_PRIVATE, &prog);
				if (prog->index != -1) {
					wxCommandEvent* event_status = new wxCommandEvent(MY_EVENT_TYPE, cMain->ID_TRANSLATES_LIST_STATUS);
					event_status->SetInt(prog->index);
					if (m->data.result == CURLE_OK) {
						event_status->SetString("Done");
					} else {
						event_status->SetString("Error");
					}
					wxQueueEvent(cMain, event_status);
				}
				delete prog;
				curl_multi_remove_handle(cMain->curl_multi_handle, e);
				curl_easy_cleanup(e);
			}
		} while (m);

		if (mc == CURLM_OK) {
			/* wait for activity or timeout */
			mc = curl_multi_poll(cMain->curl_multi_handle, NULL, 0, 1000, &numfds);
		}

		if (mc != CURLM_OK) {
			//fprintf(stderr, "curl_multi     , code %d.\n", mc);
			break;
		}

	} while (still_running);
}

void dowloadAnimeSA(cMain* cMain, CURL* curl_handle, size_t n, wxArrayInt checkedEps, size_t index, bool is_download_only_subs) {
	if (!checkedEps.IsEmpty()) {
		std::vector <FILE*> files;
#ifdef _LINUX
		mkdir(cMain->choosedTitleName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#else
        _mkdir(cMain->choosedTitleName.c_str());
#endif
        for (size_t i = 0; i < n; ++i) {
			size_t epInd = checkedEps.Item(i);
			if (epInd != 0) {
				epInd--;
				if (cMain->selTranslIDs[epInd] == -1) {
					getEpInfo(cMain, curl_handle, epInd);
				}
				if (cMain->selTranslIDs[epInd] != -1) {
					transl tr = cMain->epTransl[epInd][cMain->selTranslIDs[epInd]];
					std::string epString = std::to_string(epInd + 1);
					for (int it = epString.size(); it < 3; ++it) {
						epString = "0" + epString;
					}
					FILE* f;

					if (!is_download_only_subs) {
#ifdef _LINUX
						f = fopen((cMain->choosedTitleName + "/[Anime365] " + cMain->choosedTitleName + " - " + epString + ".mp4").c_str(), "wb");
#else
						f = fopen((cMain->choosedTitleName + "\\[Anime365] " + cMain->choosedTitleName + " - " + epString + ".mp4").c_str(), "wb");
#endif
						files.push_back(f);
						addHandle(cMain, tr.tr_links.videoLink, f, index);
						std::string* params = new std::string[2];
						params[0] = cMain->epNames[epInd] + ".mp4";
						params[1] = std::to_string(tr.size / 1024 / 1024);
						wxCommandEvent* event_insert = new wxCommandEvent(MY_EVENT_TYPE, cMain->ID_DOWNLOADS_LIST_INSERT);
						event_insert->SetInt(index);
						event_insert->SetClientData(params);
						wxQueueEvent(cMain, event_insert);
						index++;
					}
					/*cMain->downloads_list->InsertItem(index, cMain->epNames[epInd] + ".mp4");
					cMain->downloads_list->SetItem(index, 1, std::to_string(tr.size / 1024 / 1024));
					cMain->downloads_list->SetItem(index, 2, "0%");
					cMain->downloads_list->SetItem(index, 3, "downloading");
					cMain->downloads_list->SetItem(index, 4, "0");
					cMain->downloads_list->SetItem(index, 5, "0");
					cMain->downloads_list->SetItem(index, 6, "0");*/
					if (!tr.tr_links.subsLink.empty()) {
#ifdef _LINUX
                        f = fopen((cMain->choosedTitleName + "/[Anime365] " + cMain->choosedTitleName + " - " + epString + ".ass").c_str(), "wb");
#else
						f = fopen((cMain->choosedTitleName + "\\[Anime365] " + cMain->choosedTitleName + " - " + epString + ".ass").c_str(), "wb");
#endif
						files.push_back(f);
						addHandle(cMain, tr.tr_links.subsLink, f, -1);
					}
				}
			}
		}

		multiDownload(cMain);
			
		for (auto& file : files) {
			fclose(file);
		}
	}
	enableMainPanel(cMain);
}

inline bool fileExistsTest(const std::string& name) {
	if (FILE* file = fopen(name.c_str(), "r")) {
		fclose(file);
		return true;
	} else {
		return false;
	}
}

bool checkSaAuth() {
	return fileExistsTest("cookies");
}

std::string readAllFileContents(const std::string& filename) {
	std::ifstream t(filename);
	std::string page((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
	t.close();
	return page;
}

void saAuth(const std::string& email, const std::string& password) {
	CURL* curl = curl_easy_init();
	CURLcode res;
	std::string url = "https://smotret-anime.online/users/login";
	FILE* cookies_file = fopen("cookies", "w");
	if (curl && cookies_file) {
		fclose(cookies_file);
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DontWriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, nullptr);
		struct curl_slist* list = NULL;
		list = curl_slist_append(list, "authority: smotret-anime.online");
		list = curl_slist_append(list, "dnt: 1");
		list = curl_slist_append(list, "accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9");
		list = curl_slist_append(list, "upgrade-insecure-requests: 1");
		list = curl_slist_append(list, "user-agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/81.0.4044.138 Safari/537.36");
		list = curl_slist_append(list, "sec-fetch-site: none");
		list = curl_slist_append(list, "sec-fetch-mode: navigate");
		list = curl_slist_append(list, "sec-fetch-user: ?1");
		list = curl_slist_append(list, "sec-fetch-dest: document");
		list = curl_slist_append(list, "accept-language: ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7");
		curl_easy_setopt(curl, CURLOPT_COOKIESESSION, 1L);
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, "cookies");
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "cookies");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		res = curl_easy_perform(curl);
		curl_easy_setopt(curl, CURLOPT_URL, "https://smotret-anime-365.ru/users/syncCookies");
		res = curl_easy_perform(curl);
		curl_easy_setopt(curl, CURLOPT_URL, "https://smotret-anime.online/users/login");
		curl_easy_setopt(curl, CURLOPT_POSTREDIR, CURL_REDIR_POST_ALL);
		curl_slist_free_all(list);
		list = NULL;
		list = curl_slist_append(list, "authority: smotret-anime.online");
		list = curl_slist_append(list, "dnt: 1");
		list = curl_slist_append(list, "accept: text/html, */*; q=0.01");
		list = curl_slist_append(list, "x-requested-with: XMLHttpRequest");
		list = curl_slist_append(list, "content-type: application/x-www-form-urlencoded; charset=UTF-8");
		list = curl_slist_append(list, "user-agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/81.0.4044.138 Safari/537.36");
		list = curl_slist_append(list, "origin: https://smotret-anime.online");
		list = curl_slist_append(list, "sec-fetch-site: same-origin");
		list = curl_slist_append(list, "sec-fetch-mode: cors");
		list = curl_slist_append(list, "sec-fetch-dest: empty");
		list = curl_slist_append(list, "referer: https://smotret-anime.online/users/login");
		list = curl_slist_append(list, "accept-language: ru-RU,ru;q=0.9,en-US;q=0.8,en;q=0.7");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		curl_easy_setopt(curl, CURLOPT_COOKIELIST, "FLUSH");
		std::string cookf = readAllFileContents("cookies");
		size_t i = cookf.find("csrf");
		i += 5;
		std::string csrf = cookf.substr(i, 92);
		std::string postfield = "csrf=" + csrf + "&LoginForm%5Busername%5D=" + email + "&LoginForm%5Bpassword%5D=" + password + "&yt0=&dynpage=1";
		const char* data = postfield.c_str();
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postfield.size());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
		res = curl_easy_perform(curl);
		curl_slist_free_all(list);
	}
	curl_easy_cleanup(curl);
}

void parseEpisodes(cMain* cMain, std::string& readBuffer) {
	json j = json::parse(readBuffer);
	j = j["data"]["episodes"];
	cMain->epIDs.clear();
	cMain->epNames.clear();
	wxCommandEvent* event_clear = new wxCommandEvent(MY_EVENT_TYPE, cMain->ID_CLEAR_GUI_FOR_CHOOSED_ANIME);
	wxQueueEvent(cMain, event_clear);
	for (auto& ep : j) {
		long long id = ep["id"].get<long long>();
		std::string episodeFull = ep["episodeFull"].get<std::string>();
		cMain->epIDs.push_back(id);
		wxCommandEvent* event_append = new wxCommandEvent(MY_EVENT_TYPE, cMain->ID_CHOOSE_EPISODES_TO_DOWNLOAD);
        std::string* temp = new std::string;
        *temp = episodeFull;
		event_append->SetClientData(temp);
		wxQueueEvent(cMain, event_append);
		cMain->epNames.push_back(episodeFull);
	}
	cMain->epTransl.clear();
	cMain->epTransl.resize(cMain->epIDs.size());
	cMain->selTranslIDs.assign(cMain->epIDs.size(), -1);
}

void findEpisodesSA(cMain* cMain, CURL* curl_handle, size_t ind) {
	long long id = cMain->animeIDs[ind];
	std::string readBuffer;
#ifdef _DEBUG
	std::string request_url = "https://smotret-anime.online/api/series/" + std::to_string(id) + "?pretty=1";
#else
	std::string request_url = "https://smotret-anime.online/api/series/" + std::to_string(id);
#endif
	if (requestSAapi(curl_handle, request_url, readBuffer) && !readBuffer.empty()) {
		parseEpisodes(cMain, readBuffer);
	}
	enableMainPanel(cMain);
}

void cMain::execSaAuth() {
	saLoginDialog->ShowModal();
	std::string email = (std::string)saLoginDialog->GetValue();
	saPasswordDialog->ShowModal();
	std::string password = (std::string)saPasswordDialog->GetValue();
	if (!email.empty() && !password.empty()) {
		std::thread auther(saAuth, email, password);
		auther.detach();
	}
}

void cMain::execCURL() {
	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 0L);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, USER_AGENT);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteCallbackToString);

	curl_handle_cookies = curl_easy_init();
	curl_easy_setopt(curl_handle_cookies, CURLOPT_VERBOSE, 0L);
	curl_easy_setopt(curl_handle_cookies, CURLOPT_COOKIEJAR, "cookies");
	curl_easy_setopt(curl_handle_cookies, CURLOPT_COOKIEFILE, "cookies");
	curl_easy_setopt(curl_handle_cookies, CURLOPT_WRITEFUNCTION, WriteCallbackToString);

	curl_multi_handle = curl_multi_init();
}

void findEpTranslations(cMain* cMain, CURL* curl_handle, size_t index, wxArrayInt checkedEps) {
	if (checkedEps.Item(0) == 0) {
		index++;
	}
	size_t epInd = checkedEps.Item(index);
	epInd--;
	if (cMain->selTranslIDs[epInd] == -1) {
		getEpInfo(cMain, curl_handle, epInd);
	}
	cMain->cur_ep_index = epInd;
	refreshTranslatesList(cMain, cMain->epTransl[epInd]);
	enableMainPanel(cMain);
}

void getEpInfo(cMain* cMain, CURL* curl_handle, size_t index) {
	std::string epIDstr = std::to_string(cMain->epIDs[index]);
#ifdef _DEBUG
	std::string request_url = "https://smotret-anime.online/api/episodes/" + epIDstr + "?pretty=1";
#else
	std::string request_url = "https://smotret-anime.online/api/episodes/" + epIDstr;
#endif
	std::string readBuffer;
	if (requestSAapi(curl_handle, request_url, readBuffer) && !readBuffer.empty()) {
		parseTranslations(cMain, curl_handle, readBuffer, index);
	}
	if (cMain->epTransl[index].size()) {
		cMain->selTranslIDs[index] = 0;
	}
}

void refreshTranslatesList(cMain* cMain, std::vector<transl>& info) {
	wxCommandEvent* event_remove = new wxCommandEvent(MY_EVENT_TYPE, cMain->ID_TRANSLATES_LIST);
	wxQueueEvent(cMain, event_remove);
	for (size_t i = 0; i < info.size(); ++i) {
		wxCommandEvent* event_insert = new wxCommandEvent(MY_EVENT_TYPE, cMain->ID_TRANSLATES_LIST_INSERT);
		event_insert->SetInt(i);
		std::string* params = new std::string[4];
		params[0] = info[i].translName;
		params[1] = std::to_string(info[i].size / 1024 / 1024) + " MB";
		params[2] = info[i].quality + "p";
		std::string duration_fixed = std::to_string((long long)info[i].duration / 60) + "m " + std::to_string((long long)info[i].duration % 60) + "s";
		params[3] = duration_fixed;
		event_insert->SetClientData(params);
		wxQueueEvent(cMain, event_insert);
	}
}

void enableMainPanel(cMain* cMain) {
	wxCommandEvent* event = new wxCommandEvent(MY_EVENT_TYPE, cMain->ID_WXPANEL_MAIN);
	wxQueueEvent(cMain, event);
}

void disableMainPanel(cMain* cMain, wxString status = "Working") {
	cMain->main_panel->Disable();
	cMain->SetStatusText(status, 0);
}
 
void cMain::onButtonSearchAnime(wxCommandEvent& evt) {
	if (button_search_anime->IsEnabled()) {
		disableMainPanel(this, "Searching");
		std::string animename = (std::string)entry_anime_name->GetValue().ToUTF8();
		std::thread searcher(findAnimeSA, this, curl_handle, animename);
		searcher.detach();
	}
	evt.Skip();
}

void cMain::onButtonDownloadAnime(wxCommandEvent& evt) {
	disableMainPanel(this, "Downloading");
	wxArrayInt checkedEps;
	size_t n = choose_episodes_to_download->GetCheckedItems(checkedEps);
	size_t index = downloads_list->GetItemCount();
	bool is_download_only_subs = download_only_subs_checkbox->IsChecked();
	std::thread downloader(dowloadAnimeSA, this, curl_handle, n, checkedEps, index, is_download_only_subs);
	downloader.detach();
	evt.Skip();
}

void cMain::onButtonSelectTitle(wxCommandEvent& evt) {
	disableMainPanel(this);
	size_t ind = choose_anime->GetSelection();
	choosedTitleName = animeNames[ind];
	std::thread downloader(findEpisodesSA, this, curl_handle, ind);
	downloader.detach();
	evt.Skip();
}

void cMain::onAnimeEntryChange(wxCommandEvent& evt) {
	if (entry_anime_name->GetValue().empty()) {
		button_search_anime->Disable();
	} else {
		button_search_anime->Enable();
	}
	evt.Skip();
}

void cMain::onCheckListChecked(wxCommandEvent& evt) {
	if (evt.GetInt() == 0) {
		if (choose_episodes_to_download->IsChecked(0)) {
			for (size_t i = 1; i < choose_episodes_to_download->GetCount(); ++i) {
				choose_episodes_to_download->Check(i);
			}
		} else {
			for (size_t i = 1; i < choose_episodes_to_download->GetCount(); ++i) {
				choose_episodes_to_download->Check(i, false);
			}
		}
	}
	choose_episode->Clear();
	wxArrayInt checkedEps;
	size_t n = choose_episodes_to_download->GetCheckedItems(checkedEps);
	for (size_t i = 0; i < n; ++i) {
		if (checkedEps.Item(i) != 0) {
			choose_episode->Append(wxString::FromUTF8(epNames[checkedEps.Item(i) - 1].c_str()));
		}
	}
	
	evt.Skip();
}

void cMain::onTopMenuInfo(wxCommandEvent& evt) {
	wxAboutDialogInfo info;
	info.SetName("SA");
	info.SetVersion("1.0");
	info.SetDescription("Program to download anime from smotret-anime.online");
	info.SetCopyright(wxT("(C) 2020 Alexandr Starovoytov <stewk6@gmail.com>"));
	wxAboutBox(info);
	evt.Skip();
}

void cMain::onEpisodeChoosen(wxCommandEvent& evt) {
	disableMainPanel(this);
	size_t index = evt.GetInt();
	wxArrayInt checkedEps;
	choose_episodes_to_download->GetCheckedItems(checkedEps);
	std::thread downloader(findEpTranslations, this, curl_handle, index, checkedEps);
	downloader.detach();
	evt.Skip();
}

void cMain::onTranslationChoosen(wxListEvent& evt) {
	size_t trIndex = evt.GetIndex();
	selTranslIDs[cur_ep_index] = trIndex;
	evt.Skip();
}

void cMain::onlogoutSA(wxCommandEvent& evt) {
	do_logout = true;
	evt.Skip();
	Close();
}

void cMain::OnDownloadsListRightClick(wxListEvent& evt) {
	void* data = reinterpret_cast<void*>(evt.GetItem().GetData());
	wxMenu mnu;
#define ID_SOMETHING		2001
#define ID_SOMETHING_ELSE	2002
	mnu.SetClientData(data);
	mnu.Append(ID_SOMETHING, "Do something");
	mnu.Append(ID_SOMETHING_ELSE, "Do something else");
	mnu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(cMain::OnPopupClick), NULL, this);
	PopupMenu(&mnu);
}

void cMain::OnPopupClick(wxCommandEvent& evt) {
	void* data = static_cast<wxMenu*>(evt.GetEventObject())->GetClientData();
	switch (evt.GetId()) {
	case ID_SOMETHING:
		break;
	case ID_SOMETHING_ELSE:
		break;
	}
}

void cMain::OnEnableMainPanel(wxCommandEvent& evt) {
	main_panel->Enable();
	SetStatusText(wxT("Ready"), 0);
	evt.Skip();
}

void cMain::OnClearGuiForSearch(wxCommandEvent& evt) {
	choose_episode->Clear();
	choose_anime->Clear();
	choose_episodes_to_download->Clear();
	translates_list->DeleteAllItems();
	evt.Skip();
}

void cMain::OnAppendToAnimesList(wxCommandEvent& evt) {
    std::string* temp = (std::string *)evt.GetClientData();

    choose_anime->Append(wxString::FromUTF8(temp->c_str()));
    delete temp;
    evt.Skip();
}

void cMain::OnClearGuiForChoosedAnime(wxCommandEvent& evt) {
	choose_episodes_to_download->Clear();
	choose_episode->Clear();
	choose_episodes_to_download->Append(wxString::FromUTF8("Выделить все"));
	translates_list->DeleteAllItems();
	evt.Skip();
}

void cMain::OnChooseEpisodesToDownloadAppend(wxCommandEvent& evt) {
    std::string* temp = (std::string*)evt.GetClientData();
	choose_episodes_to_download->Append(wxString::FromUTF8(temp->c_str()));
    delete temp;
	evt.Skip();
}

void cMain::OnClearTranslatesList(wxCommandEvent& evt) {
	translates_list->DeleteAllItems();
	evt.Skip();
}

void cMain::OnInsertToTranslatesList(wxCommandEvent& evt) {
	size_t i = evt.GetInt();
	std::string* params = (std::string*)evt.GetClientData();
	translates_list->InsertItem(i, wxString::FromUTF8(params[0].c_str()));
	translates_list->SetItem(i, 1, params[1]);
	translates_list->SetItem(i, 2, params[2]);
	translates_list->SetItem(i, 3, params[3]);
	delete[] params;
	evt.Skip();
}

void cMain::onSetDownloadStasus(wxCommandEvent& evt) {
	long long index = evt.GetInt();
	wxString status = evt.GetString();
	downloads_list->SetItem(index, 3, status);
	downloads_list->SetItem(index, 6, "0");
	evt.Skip();
}

void cMain::onSetDownloadParams(wxCommandEvent& evt) {
	long long i = evt.GetInt();
	std::string* params = (std::string*)evt.GetClientData();
	downloads_list->SetItem(i, 4, params[0]);
	downloads_list->SetItem(i, 6, params[1]);
	downloads_list->SetItem(i, 5, params[2]);
	delete[] params;
	evt.Skip();
}

void cMain::onSetDownloadParamsProgress(wxCommandEvent& evt) {
	long long index = evt.GetInt();
	wxString str = evt.GetString();
	downloads_list->SetItem(index, 2, str);
	evt.Skip();
}

void cMain::onDownloadsListInsert(wxCommandEvent& evt) {
	long long i = evt.GetInt();
	std::string* params = (std::string*)evt.GetClientData();
	downloads_list->InsertItem(i, wxString::FromUTF8(params[0].c_str()));
	downloads_list->SetItem(i, 1, params[1]);
	downloads_list->SetItem(i, 2, "0%");
	downloads_list->SetItem(i, 3, "downloading");
	downloads_list->SetItem(i, 4, "0");
	downloads_list->SetItem(i, 5, "0");
	downloads_list->SetItem(i, 6, "0");
	delete[] params;
	evt.Skip();
}
