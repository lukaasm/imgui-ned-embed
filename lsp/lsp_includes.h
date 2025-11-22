#pragma once

// Platform-specific LSP framework includes
#include "lsp/messages.h"
#include "lsp/types.h"

// Common LSP framework includes
#include "lsp/connection.h"
#include "lsp/error.h"
#include "lsp/messagehandler.h"
#include "lsp/process.h"

// Common project includes used by LSP classes
#include "../editor/editor.h"
#include "../editor/editor_scroll.h"
#include "../files/files.h"
#include "lsp_client.h"

// Standard includes commonly used by LSP classes
#include <chrono>
#include <functional>
#include <future>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <thread>
#include <vector>