#pragma once

#include "imgui.h"
#include <atomic>
#include <filesystem>
#include <future>
#include <mutex>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class EditorHighlight
{
  public:
	EditorHighlight();
	~EditorHighlight() = default;

	void highlightContent(bool fullRehighlight = false, bool sync = false);

	void cancelHighlighting();

	void forceColorUpdate();

	bool validateHighlightContentParams();

	void loadTheme(const std::string &themeName);

	void setTheme(const std::string &themeName);

  private:
	std::unordered_map<std::string, ImVec4> themeColors;

	// Highlighting state management
	std::mutex highlight_mutex;
	std::future<void> highlightFuture;
	std::atomic<bool> highlightingInProgress{false};
	std::mutex colorsMutex;
	std::atomic<bool> cancelHighlightFlag{false};
};

// Global instance
extern EditorHighlight gEditorHighlight;