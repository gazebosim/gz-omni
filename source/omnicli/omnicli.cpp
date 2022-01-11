/*###############################################################################
#
# Copyright 2020 NVIDIA Corporation
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
# the Software, and to permit persons to whom the Software is furnished to do so,
# subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
# COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
# IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
# CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
###############################################################################*/

#define __STDC_FORMAT_MACROS 1
#define _CRT_NONSTDC_NO_WARNINGS
#include <OmniClient.h>
#include <OmniUsdLive.h>
#include <algorithm>
#include <condition_variable>
#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <time.h>
#include <vector>

#include <pxr/pxr.h>
#include <pxr/usd/usd/stage.h>

static const int MAX_URL_SIZE = 2048;

bool g_shutdown = false;
bool g_haveUpdates = false;
std::mutex g_mutex;
std::condition_variable g_cv;
PXR_NS::UsdStageRefPtr g_stage;

template<class Mutex>
auto make_lock(Mutex& m)
{
	return std::unique_lock<Mutex>(m);
}

using ArgVec = std::vector<std::string>;

bool iequal(std::string const& a, std::string const& b)
{
	return (a.size() == b.size()) &&
		std::equal(a.begin(), a.end(), b.begin(), [](auto a, auto b) { return tolower(a) == tolower(b); });
}

/*
Tokenize a line using the Windows command line rules:
* Arguments are delimited by white space, which is either a space or a tab.
* A string surrounded by double quotation marks is interpreted as a single argument,
	regardless of white space contained within. A quoted string can be embedded in an argument.
* A double quotation mark preceded by a backslash, \", is interpreted as a literal double quotation mark (").
* Backslashes are interpreted literally, unless they immediately precede a double quotation mark.
* If an even number of backslashes is followed by a double quotation mark, then one backslash (\) is placed in
	the argv array for every pair of backslashes (\\), and the double quotation mark (") is interpreted as a string
delimiter.
* If an odd number of backslashes is followed by a double quotation mark, then one backslash (\) is placed in the argv
array for every pair of backslashes (\\) and the double quotation mark is interpreted as an escape sequence by the
remaining backslash, causing a literal double quotation mark (") to be placed in argv.
*/
std::vector<std::string> tokenize(char const* line)
{
	std::vector<std::string> tokens;
	std::string token;
	bool inWhiteSpace = true;
	bool inQuote = false;
	uint32_t backslashCount = 0;
	for (char const* p = line; *p; p++)
	{
		if (*p == '\n')
		{
			break;
		}
		if (inWhiteSpace)
		{
			if (*p == ' ' || *p == '\t')
			{
				continue;
			}
			else
			{
				inWhiteSpace = false;
			}
		}
		if (*p == '\\')
		{
			backslashCount++;
			continue;
		}
		if (*p == '\"')
		{
			token.append(backslashCount / 2, '\\');
			if (backslashCount % 2 == 1)
			{
				token.push_back('\"');
			}
			else
			{
				inQuote = !inQuote;
			}
			backslashCount = 0;
			continue;
		}
		if (backslashCount > 0)
		{
			token.append(backslashCount, '\\');
			backslashCount = 0;
		}
		if (!inQuote && (*p == ' ' || *p == '\t'))
		{
			tokens.emplace_back(std::move(token));
			token.clear();
			inWhiteSpace = true;
			continue;
		}
		token.push_back(*p);
	}
	if (backslashCount > 0)
	{
		token.append(backslashCount, '\\');
	}
	if (!token.empty())
	{
		tokens.emplace_back(std::move(token));
	}
	return tokens;
}

int resultToRetcode(OmniClientResult result)
{
	switch (result)
	{
	case eOmniClientResult_Ok:
	case eOmniClientResult_OkLatest:
	case eOmniClientResult_OkNotYetFound:
		return EXIT_SUCCESS;
	default:
		return EXIT_FAILURE;
	}
}

int help(ArgVec const& args);

int noop(ArgVec const& args)
{
	return EXIT_SUCCESS;
}

void printResult(OmniClientResult result)
{
	printf("%s\n", omniClientGetResultString(result));
}

char const* formatTime(uint64_t tns)
{
	time_t time = (time_t)(tns / 1'000'000'000);
	static char timeStr[30];
	strftime(timeStr, sizeof(timeStr), "%F %T", localtime(&time));
	return timeStr;
}

std::string getAccessString(uint16_t access)
{
	std::string accessString;
	if (access & fOmniClientAccess_Read)
	{
		if (!accessString.empty())
		{
			accessString += ", ";
		}
		accessString += "Read";
	}
	if (access & fOmniClientAccess_Write)
	{
		if (!accessString.empty())
		{
			accessString += ", ";
		}
		accessString += "Write";
	}
	if (access & fOmniClientAccess_Admin)
	{
		if (!accessString.empty())
		{
			accessString += ", ";
		}
		accessString += "Admin";
	}
	if (accessString.empty())
	{
		accessString = "None";
	}
	return accessString;
}

void printListEntries(uint32_t numEntries, struct OmniClientListEntry const* entries)
{
	size_t longestOwner = 0;
	size_t longestType = 0;
	std::vector<std::string> types;
	for (uint32_t i = 0; i < numEntries; i++)
	{
		if (entries[i].createdBy != nullptr)
		{
			longestOwner = std::max(longestOwner, strlen(entries[i].createdBy));
		}
		std::string type;
		{
			auto flags = entries[i].flags;
			if (flags & fOmniClientItem_IsInsideMount)
			{
				type.append("mounted ");
			}
			if (flags & fOmniClientItem_ReadableFile)
			{
				auto size = entries[i].size;
				auto sizeSuffix = "B ";
				if (size > 1000 * 1000 * 1000)
				{
					size /= 1000 * 1000 * 1000;
					sizeSuffix = "GB ";
				}
				else if (size > 1000 * 1000)
				{
					size /= 1000 * 1000;
					sizeSuffix = "MB ";
				}
				else if (size > 1000)
				{
					size /= 1000;
					sizeSuffix = "KB ";
				}
				type.append(std::to_string(size));
				type.append(sizeSuffix);

				if ((flags & fOmniClientItem_WriteableFile) == 0)
				{
					type.append("read-only ");
				}
				type.append("file ");
			}
			if (flags & fOmniClientItem_IsMount)
			{
				type.append("mount-point ");
			}
			else if (flags & fOmniClientItem_CanHaveChildren)
			{
				if (flags & fOmniClientItem_DoesNotHaveChildren)
				{
					type.append("empty-folder ");
				}
				else
				{
					type.append("folder ");
				}
			}
			if (flags & fOmniClientItem_CanLiveUpdate)
			{
				type.append("live ");
			}
			if (flags & fOmniClientItem_IsOmniObject)
			{
				type.append("omni-object ");
			}
			if (flags & fOmniClientItem_IsChannel)
			{
				type.append("channel ");
			}
			if (flags & fOmniClientItem_IsCheckpointed)
			{
				type.append("checkpointed ");
			}
		}
		longestType = std::max(longestType, type.size());
		types.emplace_back(std::move(type));
	}
	for (uint32_t i = 0; i < numEntries; i++)
	{
		auto ownerStr = entries[i].createdBy;
		if (ownerStr == nullptr)
		{
			ownerStr = "";
		}
		//     Date, type/size, owner, relative name
		printf("%18s  %*s %*s %s %s\n", formatTime(entries[i].modifiedTimeNs), (int)longestType, types[i].c_str(),
			(int)longestOwner, ownerStr, entries[i].relativePath, entries[i].comment);
	}
}

int list(ArgVec const& args)
{
	const char* url = ".";
	if (args.size() > 1)
	{
		url = args[1].data();
	}
	int retCode = EXIT_FAILURE;
	omniClientReconnect(url);
	omniClientWait(omniClientList(
		url, &retCode,
		[](void* userData, OmniClientResult result, uint32_t numEntries, struct OmniClientListEntry const* entries) noexcept {
		*(int*)userData = resultToRetcode(result);
		if (result != eOmniClientResult_Ok)
		{
			printResult(result);
			return;
		}
		printListEntries(numEntries, entries);
	}));
	return retCode;
}

int stat(ArgVec const& args)
{
	const char* url = ".";
	if (args.size() > 1)
	{
		url = args[1].data();
	}
	int retCode = EXIT_FAILURE;
	omniClientReconnect(url);
	omniClientWait(omniClientStat(
		url, &retCode, [](void* userData, OmniClientResult result, struct OmniClientListEntry const* entry) noexcept {
		*(int*)userData = resultToRetcode(result);
		if (result != eOmniClientResult_Ok)
		{
			printResult(result);
			return;
		}
		printf("Access: %s\n", getAccessString(entry->access).c_str());
		{
			std::string type;
			auto flags = entry->flags;
			if (flags & fOmniClientItem_ReadableFile)
			{
				type.append("ReadableFile ");
			}
			if (flags & fOmniClientItem_WriteableFile)
			{
				type.append("WritableFile ");
			}
			if (flags & fOmniClientItem_IsInsideMount)
			{
				type.append("IsInsideMount ");
			}
			if (flags & fOmniClientItem_IsMount)
			{
				type.append("IsMount ");
			}
			if (flags & fOmniClientItem_CanHaveChildren)
			{
				type.append("CanHaveChildren ");
			}
			if (flags & fOmniClientItem_DoesNotHaveChildren)
			{
				type.append("DoesNotHaveChildren ");
			}
			if (flags & fOmniClientItem_CanLiveUpdate)
			{
				type.append("CanLiveUpdate ");
			}
			if (flags & fOmniClientItem_IsOmniObject)
			{
				type.append("IsOmniObject ");
			}
			if (flags & fOmniClientItem_IsChannel)
			{
				type.append("IsChannel ");
			}
			if (flags & fOmniClientItem_IsCheckpointed)
			{
				type.append("IsCheckpointed");
			}
			printf("Flags: %s\n", type.c_str());
		}
		printf("Size: %" PRIu64 "\n", entry->size);
		printf("Modified: %s by %s\n", formatTime(entry->modifiedTimeNs),
			entry->modifiedBy ? entry->modifiedBy : "Unknown");
		printf(
			"Created: %s by %s\n", formatTime(entry->createdTimeNs), entry->createdBy ? entry->createdBy : "Unknown");
		printf("Version: %s\n", entry->version ? entry->version : "Not Supported");
		}));
	return retCode;
}

std::string combineWithBaseUrl(const char* path)
{
	std::string combined;
	size_t bufferSize = 100;
	combined.resize(bufferSize);
	bool success;
	do
	{
		success = (omniClientCombineWithBaseUrl(path, &combined[0], &bufferSize) != nullptr);
		combined.resize(bufferSize - 1); // includes null terminator
	} while (!success);
	return combined;
}

int cd(ArgVec const& args, bool push)
{
	if (args.size() == 1)
	{
		auto combined = combineWithBaseUrl(".");
		printf("%s\n", combined.c_str());
		return EXIT_SUCCESS;
	}
	std::string arg{ args[1] };
	if (arg.empty() || arg.back() != '/')
	{
		arg.push_back('/');
	}
	auto combined = combineWithBaseUrl(arg.c_str());
	if (combined.empty())
	{
		// I'm actually not sure if this can ever fail
		printf("Invalid value %s\n", args[1].data());
		return EXIT_FAILURE;
	}
	if (!push)
	{
		omniClientPopBaseUrl(nullptr);
	}
	omniClientPushBaseUrl(combined.c_str());
	printf("%s\n", combined.c_str());
	return EXIT_SUCCESS;
}

int cd(ArgVec const& args)
{
	return cd(args, false);
}

int push(ArgVec const& args)
{
	return cd(args, true);
}

int pop(ArgVec const& args)
{
	omniClientPopBaseUrl(nullptr);
	return EXIT_SUCCESS;
}

int logLevel(ArgVec const& args)
{
	if (args.size() > 1)
	{
		if (iequal(args[1], "debug") || iequal(args[1], "d"))
		{
			omniClientSetLogLevel(eOmniClientLogLevel_Debug);
			return EXIT_SUCCESS;
		}
		if (iequal(args[1], "verbose") || iequal(args[1], "v"))
		{
			omniClientSetLogLevel(eOmniClientLogLevel_Verbose);
			return EXIT_SUCCESS;
		}
		if (iequal(args[1], "info") || iequal(args[1], "i"))
		{
			omniClientSetLogLevel(eOmniClientLogLevel_Info);
			return EXIT_SUCCESS;
		}
		if (iequal(args[1], "warning") || iequal(args[1], "w"))
		{
			omniClientSetLogLevel(eOmniClientLogLevel_Warning);
			return EXIT_SUCCESS;
		}
		if (iequal(args[1], "error") || iequal(args[1], "e"))
		{
			omniClientSetLogLevel(eOmniClientLogLevel_Error);
			return EXIT_SUCCESS;
		}
	}
	printf("Valid log levels are: debug, verbose, info, warning, error\n");
	return EXIT_FAILURE;
}

int copy(ArgVec const& args)
{
	if (args.size() <= 2)
	{
		printf("Not enough arguments\n");
		return EXIT_FAILURE;
	}
	int retCode = EXIT_FAILURE;
	omniClientReconnect(args[1].data());
	omniClientReconnect(args[2].data());
	omniClientWait(omniClientCopy(args[1].data(), args[2].data(), &retCode, [](void* userData, OmniClientResult result) noexcept {
		*(int*)userData = resultToRetcode(result);
		printResult(result);
	}));
	return retCode;
}

int move(ArgVec const& args)
{
	if (args.size() <= 2)
	{
		printf("Not enough arguments\n");
		return EXIT_FAILURE;
	}
	int retCode = EXIT_FAILURE;
	omniClientReconnect(args[1].data());
	omniClientReconnect(args[2].data());
	omniClientWait(omniClientCopy(args[1].data(), args[2].data(), &retCode, [](void* userData, OmniClientResult result) noexcept {
		*(int*)userData = resultToRetcode(result);
		printResult(result);
		}));
	if (retCode == EXIT_SUCCESS)
	{
		omniClientWait(omniClientDelete(args[1].data(), &retCode, [](void* userData, OmniClientResult result) noexcept {
			*(int*)userData = resultToRetcode(result);
			printResult(result);
			}));
	}
	return retCode;
}

int del(ArgVec const& args)
{
	if (args.size() <= 1)
	{
		printf("Not enough arguments\n");
		return EXIT_FAILURE;
	}
	int retCode = EXIT_FAILURE;
	omniClientReconnect(args[1].data());
	omniClientWait(omniClientDelete(args[1].data(), &retCode, [](void* userData, OmniClientResult result) noexcept {
		*(int*)userData = resultToRetcode(result);
		printResult(result);
	}));
	return retCode;
}

int mkdir(ArgVec const& args)
{
	if (args.size() <= 1)
	{
		printf("Not enough arguments\n");
		return EXIT_FAILURE;
	}
	int retCode = EXIT_FAILURE;
	omniClientReconnect(args[1].data());
	omniClientWait(omniClientCreateFolder(args[1].data(), &retCode, [](void* userData, OmniClientResult result) noexcept {
		*(int*)userData = resultToRetcode(result);
		printResult(result);
	}));
	return retCode;
}

int cat(ArgVec const& args)
{
	if (args.size() <= 1)
	{
		printf("Not enough arguments\n");
		return EXIT_FAILURE;
	}
	int retCode = EXIT_FAILURE;
	omniClientReconnect(args[1].data());
	omniClientWait(omniClientReadFile(
		args[1].data(), &retCode,
		[](void* userData, OmniClientResult result, const char* version, OmniClientContent* content) noexcept {
		*(int*)userData = resultToRetcode(result);
		if (result != eOmniClientResult_Ok)
		{
			printResult(result);
			return;
		}
		if (content->size > 1 * 1000 * 1000)
		{
			printf("File too large to cat: %zu bytes.\n", content->size);
			return;
		}
		bool isAscii = true;
		uint8_t* buffer = (uint8_t*)content->buffer;
		for (size_t i = 0; i < content->size; i++)
		{
			if (buffer[i] == '\n' || buffer[i] == '\t' || buffer[i] == '\r')
			{
			}
			else if (buffer[i] >= 127 || buffer[i] < 32)
			{
				isAscii = false;
				break;
			}
		}
		if (isAscii)
		{
			printf("%.*s\n", (int)content->size, buffer);
		}
		else
		{
			uint32_t* buffer2 = (uint32_t*)content->buffer;
			for (size_t i = 0; i < content->size; i += 4)
			{
				auto sep = (i % 32 == 0) ? '\n' : ' ';
				printf("%8x%c", buffer2[i / 4], sep);
			}
			printf("\n");
		}
	}));
	return retCode;
}

int serverVersion(ArgVec const& args)
{
	const char* url = ".";
	if (args.size() > 1)
	{
		url = args[1].data();
	}
	else
	{
		printf("Not enough arguments: must provide an omniverse server URL (omniverse://server-name)\n");
		return EXIT_FAILURE;
	}
	int retCode = EXIT_FAILURE;
	omniClientReconnect(url);
	omniClientWait(omniClientGetServerInfo(
		url, &retCode,
		[](void* userData, OmniClientResult result, OmniClientServerInfo const * info) noexcept {
		*(int*)userData = resultToRetcode(result);
		if (result != eOmniClientResult_Ok)
		{
			printResult(result);
			return;
		}
		printf("%s\n", info->version);
	}));
	return retCode;
}

int loadUsd(ArgVec const& args)
{
	if (args.size() <= 1)
	{
		printf("Not enough arguments\n");
		return EXIT_FAILURE;
	}
	auto lock = make_lock(g_mutex);
	g_stage = PXR_NS::UsdStage::Open(args[1].data());
	if (!g_stage)
	{
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int saveUsd(ArgVec const& args)
{
	if (!g_stage)
	{
		printf("No USD loaded\n");
		return EXIT_FAILURE;
	}
	auto lock = make_lock(g_mutex);
	PXR_NS::TfErrorMark errorMark;
	errorMark.SetMark();
	if (args.size() <= 1)
	{
		g_stage->Save();
	}
	else if (!g_stage->Export(args[1].data()))
	{
		return EXIT_FAILURE;
	}
	if (errorMark.IsClean())
	{
		return EXIT_SUCCESS;
	}
	return EXIT_FAILURE;
}

int closeUsd(ArgVec const& args)
{
	if (!g_stage)
	{
		printf("No USD loaded\n");
		return EXIT_FAILURE;
	}
	g_stage = nullptr;
	return EXIT_SUCCESS;
}

int getacls(ArgVec const& args)
{
	const char* url = ".";
	if (args.size() > 1)
	{
		url = args[1].data();
	}
	int retCode = EXIT_FAILURE;
	omniClientReconnect(url);
	omniClientWait(omniClientGetAcls(
		url, &retCode,
		[](void* userData, OmniClientResult result, uint32_t numEntries, struct OmniClientAclEntry* entries) noexcept {
		*(int*)userData = resultToRetcode(result);
		if (result != eOmniClientResult_Ok)
		{
			printResult(result);
			return;
		}
		for (uint32_t i = 0; i < numEntries; i++)
		{
			printf("%s: %s\n", entries[i].name, getAccessString(entries[i].access).c_str());
		}
	}));
	return retCode;
}

int setacls(ArgVec const& args)
{
	if (args.size() <= 3)
	{
		printf("Not enough arguments\n");
		return EXIT_FAILURE;
	}
	char const * url = args[1].data();
	char const * name = args[2].data();
	char const * accessStr = args[3].data();

	bool removeEntry = false;
	uint16_t access = 0;

	for (char const* p = accessStr; *p; p++)
	{
		switch (toupper(*p))
		{
		case 'A':
			access |= fOmniClientAccess_Admin;
			break;
		case 'W':
			access |= fOmniClientAccess_Write;
			break;
		case 'R':
			access |= fOmniClientAccess_Read;
			break;
		case '-':
			removeEntry = true;
			break;
		default:
			printf("Unknown access \"%s\"\n", accessStr);
			return EXIT_FAILURE;
		}
	}
	struct GetAclsResult
	{
		OmniClientResult result;
		std::vector<std::pair<std::string, uint16_t>> entries;
	};
	GetAclsResult getAclsResult;

	omniClientReconnect(url);
	omniClientWait(omniClientGetAcls(
		url, &getAclsResult,
		[](void* userData, OmniClientResult result, uint32_t numEntries, struct OmniClientAclEntry* entries) noexcept {
		GetAclsResult & getAclsResult = *(GetAclsResult*)userData;
		getAclsResult.result = result;
		getAclsResult.entries.resize(numEntries);
		for (uint32_t i = 0; i < numEntries; i++)
		{
			std::string name = entries[i].name;
			auto access = entries[i].access;
			getAclsResult.entries[i] = std::make_pair(name, access);
		}
	}));

	if (getAclsResult.result != eOmniClientResult_Ok)
	{
		printResult(getAclsResult.result);
		return EXIT_FAILURE;
	}

	std::vector<OmniClientAclEntry> entries;
	bool found = false;
	for (auto it = getAclsResult.entries.begin(); it != getAclsResult.entries.end(); ++it)
	{
		if (it->first == name)
		{
			// Is it possible for there to be 2 entries for the same user?
			// I have no idea, but I'll assume it is...
			found = true;
		}
		else
		{
			entries.push_back(OmniClientAclEntry{ it->first.c_str(), it->second });
		}
	}
	if (removeEntry)
	{
		if (!found)
		{
			printf("%s not in the ACLs list\n", name);
			return EXIT_FAILURE;
		}
	}
	else
	{
		entries.push_back(OmniClientAclEntry{ name, access });
	}

	int retCode = EXIT_FAILURE;
	omniClientWait(omniClientSetAcls(
		url, entries.size(), entries.data(), &retCode,
		[](void* userData, OmniClientResult result) noexcept {
		*(int*)userData = resultToRetcode(result);
		if (result != eOmniClientResult_Ok)
		{
			printResult(result);
			return;
		}
	}));
	return retCode;
}

int auth(ArgVec const& args)
{
	const char * user = "";
	const char * pass = "";

	if (args.size() > 1)
	{
		user = args[1].data();
		pass = user;
	}
	if (args.size() > 2)
	{
		pass = args[2].data();
	}

	{
		static char user_env[200];
		snprintf(user_env, sizeof(user_env), "OMNI_USER=%s", user);
		putenv(user_env);
	}
	{
		static char pass_env[200];
		snprintf(pass_env, sizeof(pass_env), "OMNI_PASS=%s", pass);
		putenv(pass_env);
	}

	return EXIT_SUCCESS;
}

int makeCheckpoint(ArgVec const& args)
{
	if (args.size() <= 1)
	{
		printf("Not enough arguments\n");
		return EXIT_FAILURE;
	}
	auto comment = (args.size() > 2) ? args[2].data() : "";
	int retCode = EXIT_FAILURE;
	omniClientReconnect(args[1].data());
	bool bForce = true;
	omniClientWait(omniClientCreateCheckpoint(args[1].data(), comment, bForce, &retCode,
		[](void* userData, OmniClientResult result, char const * checkpointQuery) noexcept {
			*(int*)userData = resultToRetcode(result);
			if (result != eOmniClientResult_Ok)
			{
				printResult(result);
				return;
			}
			printf("Checkpoint created as %s\n", checkpointQuery);
		}));
	return retCode;
}

int listCheckpoints(ArgVec const& args)
{
	if (args.size() <= 1)
	{
		printf("Not enough arguments\n");
		return EXIT_FAILURE;
	}
	int retCode = EXIT_FAILURE;
	omniClientReconnect(args[1].data());
	omniClientWait(omniClientListCheckpoints(args[1].data(), &retCode,
		[](void* userData, OmniClientResult result, uint32_t numEntries, struct OmniClientListEntry const* entries) noexcept {
			*(int*)userData = resultToRetcode(result);
			if (result != eOmniClientResult_Ok)
			{
				printResult(result);
				return;
			}
			printListEntries(numEntries, entries);
		}));
	return retCode;
}

int restoreCheckpoint(ArgVec const& args)
{
	if (args.size() <= 1)
	{
		printf("Not enough arguments\n");
		return EXIT_FAILURE;
	}
	char combined[MAX_URL_SIZE] = {};
	auto combinedSize = sizeof(combined);
	if (!omniClientCombineWithBaseUrl(args[1].data(), combined, &combinedSize))
	{
		printf("URL too long\n");
		return EXIT_FAILURE;
	}
	auto brokenUrl = omniClientBreakUrl(combined);
	auto branchCheckpoint = omniClientGetBranchAndCheckpointFromQuery(brokenUrl->query);
	if (branchCheckpoint == nullptr)
	{
		printf("No checkpoint provided\n");
		omniClientFreeUrl(brokenUrl);
		return EXIT_FAILURE;
	}
	// Re-build the query string with only the branch
	char queryBuffer[MAX_URL_SIZE];
	auto queryBufferSize = sizeof(queryBuffer);
	auto queryString = omniClientMakeQueryFromBranchAndCheckpoint(branchCheckpoint->branch, 0, queryBuffer, &queryBufferSize);
	if (queryString != queryBuffer)
	{
		printf("Query string buffer overflow\n");
		omniClientFreeBranchAndCheckpoint(branchCheckpoint);
		omniClientFreeUrl(brokenUrl);
		return EXIT_FAILURE;
	}
	brokenUrl->query = queryString;
	char dstUrlBuffer[MAX_URL_SIZE];
	auto dstUrlSize = sizeof(dstUrlBuffer);
	auto dstUrl = omniClientMakeUrl(brokenUrl, dstUrlBuffer, &dstUrlSize);
	if (dstUrl != dstUrlBuffer)
	{
		printf("URL buffer overflow\n");
		omniClientFreeBranchAndCheckpoint(branchCheckpoint);
		omniClientFreeUrl(brokenUrl);
		return EXIT_FAILURE;
	}
	int retCode = EXIT_FAILURE;
	omniClientReconnect(args[1].data());
	omniClientWait(omniClientCopy(args[1].data(), dstUrl, &retCode,
		[](void* userData, OmniClientResult result) noexcept {
			*(int*)userData = resultToRetcode(result);
			if (result != eOmniClientResult_Ok)
			{
				printResult(result);
				return;
			}
		}));
	return retCode;
}

int live(ArgVec const& args)
{
	if (args.size() <= 1)
	{
		if (omniUsdLiveGetDefaultEnabled())
		{
			printf("USD Live Mode Enabled\n");
		}
		else
		{
			printf("USD Live Mode Disabled\n");
		}
		return EXIT_SUCCESS;
	}
	if (args[1] == "on")
	{
		omniUsdLiveSetDefaultEnabled(true);
		return EXIT_SUCCESS;
	}
	if (args[1] == "off")
	{
		omniUsdLiveSetDefaultEnabled(false);
		return EXIT_SUCCESS;
	}
	printf("Unknown option %s\n", args[1].data());
	return EXIT_FAILURE;
}

int lock(ArgVec const& args)
{
	if (!omniUsdLiveGetDefaultEnabled())
	{
		printf("Live mode disabled\n");
		return EXIT_FAILURE;
	}

	std::string url;
	if (args.size() <= 1)
	{
		if (g_stage)
		{
			url = g_stage->GetRootLayer()->GetRepositoryPath();
		}
		else
		{
			printf("No URL specified and no USD loaded\n");
			return EXIT_FAILURE;
		}
	}
	else
	{
		url = combineWithBaseUrl(args[1].data());
	}
	int retCode = EXIT_FAILURE;
	omniClientReconnect(url.c_str());
	omniClientWait(omniClientLock(url.c_str(), &retCode, [](void* userData, OmniClientResult result) noexcept
		{
			* (int*)userData = resultToRetcode(result);
			if (result != eOmniClientResult_Ok)
			{
				printResult(result);
				return;
			}
		}));
	return retCode;
}

int unlock(ArgVec const& args)
{
	if (!omniUsdLiveGetDefaultEnabled())
	{
		printf("Live mode disabled\n");
		return EXIT_FAILURE;
	}

	std::string url;
	if (args.size() <= 1)
	{
		if (g_stage)
		{
			url = g_stage->GetRootLayer()->GetRepositoryPath();
		}
		else
		{
			printf("No URL specified and no USD loaded\n");
			return EXIT_FAILURE;
		}
	}
	else
	{
		url = combineWithBaseUrl(args[1].data());
	}
	int retCode = EXIT_FAILURE;
	omniClientReconnect(url.c_str());
	omniClientWait(omniClientUnlock(url.c_str(), &retCode, [](void* userData, OmniClientResult result) noexcept
		{
			* (int*)userData = resultToRetcode(result);
			if (result != eOmniClientResult_Ok)
			{
				printResult(result);
				return;
			}
		}));
	return retCode;
}

int disconnect(ArgVec const& args)
{
	if (args.size() <= 1)
	{
		auto combined = combineWithBaseUrl(".");
		omniClientSignOut(combined.c_str());
	}
	else
	{
		omniClientSignOut(args[1].data());
	}
	return EXIT_SUCCESS;
}

using CommandFn = int(*)(ArgVec const& args);
struct Command
{
	char const* name;
	char const* args;
	char const* helpMessage;
	CommandFn function;
};
Command commands[] = {
	{ "help", nullptr, "Print this help message", help },
	{ "-h", nullptr, nullptr, help },
	{ "-?", nullptr, nullptr, help },
	{ "quit", nullptr, "Quit the interactive terminal", noop },
	{ "log", "<level>", "Change the log level", logLevel },
	{ "list", "<url>", "List the contents of a folder", list },
	{ "ls", nullptr, nullptr, list },
	{ "dir", nullptr, nullptr, list },
	{ "stat", "<url>", "Print information about a specified file or folder", stat },
	{ "cd", "<url>", "Makes paths relative to the specified folder", cd },
	{ "push", "<url>", "Makes paths relative to the specified folder, you can restore the original folder with pop", push },
	{ "pushd", nullptr, nullptr, push },
	{ "pop", nullptr, "Restores a folder pushed with 'push'", pop },
	{ "popd", nullptr, nullptr, pop },
	{ "copy", "<src> <dst>", "Copies a file or folder from src to dst", copy },
	{ "cp", nullptr, nullptr, copy },
	{ "move", "<src> <dst>", "Moves a file or folder from src to dst", move },
	{ "mv", nullptr, nullptr, move },
	{ "del", "<url>", "Deletes the specified file or folder", del },
	{ "delete", nullptr, nullptr, del },
	{ "rm", nullptr, nullptr, del },
	{ "mkdir", "<url>", "Create a folder", mkdir },
	{ "cat", "<url>", "Print the contents of a file", cat },
	{ "sver", "<url>", "Print the server version", serverVersion},
	{ "load", "<url>", "Load a USD file", loadUsd },
	{ "save", "[url]", "Save a previously loaded USD file (optionally to a different URL)", saveUsd },
	{ "close", "", "Close a previously loaded USD file", closeUsd },
	{ "live", "[on|off]", "Turn live mode on/off", live },
	{ "lock", "[url]", "Lock a USD file (defaults to loaded stage root)", lock },
	{ "unlock", "[url]", "Unlock a USD file (defaults to loaded stage root)", unlock },
	{ "getacls", "<url>", "Print the ACLs for a URL", getacls },
	{ "setacls", "<url> <user|group> <r|w|a|->", "Change the ACLs for a user or group for a URL; specify '-' to remove that user|group from the ACLs", setacls },
	{ "auth", "[username] [password]", "Set username/password for authentication; password defaults to username; blank reverts to standard auth", auth },
	{ "checkpoint", "<url> [comment]", "Create a checkpoint of a URL", makeCheckpoint },
	{ "listCheckpoints", "<url>", "List all checkpoints of a URL", listCheckpoints },
	{ "restoreCheckpoint", "<url>", "Restore a checkpoint", restoreCheckpoint },
	{ "disconnect", "<url>", "Disconnect from a server", disconnect }
};

int help(ArgVec const& args)
{
	for (auto&& command : commands)
	{
		if (command.helpMessage == nullptr)
		{
			continue;
		}
		printf("%10s : %s\n", command.name, command.helpMessage);
	}
	return EXIT_SUCCESS;
}

int run(ArgVec const& args)
{
	if (args.size() == 0)
	{
		return EXIT_FAILURE;
	}
	for (auto&& command : commands)
	{
		if (iequal(args[0], command.name))
		{
			return command.function(args);
		}
	}
	printf("Unknown command \"%s\":  Type \"help\" to list available commands.\n", args[0].data());
	return EXIT_FAILURE;
}

int main(int argc, char const* const* argv)
{
	ArgVec args;
	if (argc > 1)
	{
		args.resize(argc - 1);
		for (size_t i = 1; i < argc; i++)
		{
			args[i - 1] = argv[i];
		}
		return run(args);
	}
	auto updateThread = std::thread([]() {
		auto lock = make_lock(g_mutex);
		while (!g_shutdown)
		{
			if (g_haveUpdates)
			{
				g_haveUpdates = false;
				printf("Processing live updates\n");
				omniUsdLiveProcess();
			}
			while (!g_haveUpdates && !g_shutdown)
			{
				g_cv.wait(lock);
			}
		}
	});
	omniUsdLiveSetQueuedCallback([]() noexcept {
		{
			auto lock = make_lock(g_mutex);
			printf("Live update queued\n");
			g_haveUpdates = true;
		}
		g_cv.notify_all();
	});

	omniClientSetLogCallback(
		[](char const* threadName, char const* component, OmniClientLogLevel level, char const* message) noexcept
		{
			bool isPing = strstr(message, "\"command\":\"ping\"") != nullptr;
			if (!isPing)
			{
				// Some versions of the server don't include "command:ping" in the reply
				// Try to detect these ping replies based on other fields being present or not
				bool hasVersion = strstr(message, "\"version\":") != nullptr;
				bool hasAuth = strstr(message, "\"auth\":") != nullptr;
				if (hasVersion && hasAuth)
				{
					isPing = true;
				}
				bool hasToken = strstr(message, "\"token\":") != nullptr;
				bool hasServerCaps = strstr(message, "\"server_capabilities\":") != nullptr;
				if (hasToken && !hasServerCaps)
				{
					isPing = true;
				}
			}
			if (!isPing)
			{
				printf("%s: %s\n", omniClientGetLogLevelString(level), message);
			}
		});
	if (!omniClientInitialize(kOmniClientVersion))
	{
		printf("Failed to initialize Omniverse Client Library\n");
		return EXIT_FAILURE;
	}
	omniClientSetLogLevel(eOmniClientLogLevel_Warning);

	omniClientRegisterFileStatusCallback(nullptr, [](void* userData, char const* url, OmniClientFileStatus status, int percentage) noexcept {
		printf("%s (%d%%): %s\n", omniClientGetFileStatusString(status), percentage, url);
	});

	for (;;)
	{
		printf("> ");
		char line[5000];
		if (fgets(line, sizeof(line), stdin) == nullptr)
		{
			return EXIT_FAILURE;
		}
		auto tokens = tokenize(line);
		if (tokens.size() == 0)
		{
			continue;
		}
		if (iequal(tokens[0], "quit") || iequal(tokens[0], "exit") || iequal(tokens[0], "q"))
		{
			break;
		}
		args.resize(tokens.size());
		for (size_t i = 0; i < tokens.size(); i++)
		{
			args[i] = tokens[i];
		}
		run(args);
	}

	{
		auto lock = make_lock(g_mutex);
		g_shutdown = true;
	}
	g_cv.notify_all();
	updateThread.join();

	omniClientShutdown();

	return EXIT_SUCCESS;
}
