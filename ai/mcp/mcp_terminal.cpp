#include "mcp_terminal.h"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <memory>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#define getcwd _getcwd
#else
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace MCP {

TerminalServer::TerminalServer() {}

std::string TerminalServer::executeCommand(const std::string &command)
{
	try
	{
		// Use popen to execute the command and capture output
		std::array<char, 128> buffer;
		std::string result;

		return "ERROR: MCP terminal commands not supported on Windows";

	} catch (const std::exception &e)
	{
		return "ERROR: " + std::string(e.what());
	} catch (...)
	{
		return "ERROR: Unknown error occurred while executing command";
	}
}

} // namespace MCP