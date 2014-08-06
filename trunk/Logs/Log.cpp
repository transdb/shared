/*
 * Game server
 * Copyright (C) 2010 Miroslav 'Wayland' Kudrnac
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "Log.h"

#ifdef ANDROID
    #include <android/log.h>
#endif

createFileSingleton(ScreenLog);

ScreenLog::ScreenLog()
{
	m_log_level		= 3;
#if defined(WIN32) && !defined(WP8)
	m_stderr_handle = GetStdHandle(STD_ERROR_HANDLE);
	m_stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
}

ScreenLog::~ScreenLog()
{

}

void ScreenLog::CreateFileLog(const std::string &sFilePath)
{
    m_pFileLog = std::unique_ptr<FileLog>(new FileLog(sFilePath));
}

void ScreenLog::Color(unsigned int color)
{
#ifndef WIN32
#ifndef MAC
		static const char* colorstrings[TBLUE+1] = {
			"",
				"\033[22;31m",
				"\033[22;32m",
				"\033[01;33m",
				"\033[0m",
				"\033[01;37m",
				"\033[1;34m",
		};
		fputs(colorstrings[color], stdout);
#endif
#elif defined(WP8)
	//nothing
#else
	SetConsoleTextAttribute(m_stdout_handle, (WORD)color);
#endif
}

void ScreenLog::Time()
{
#ifdef WP8
	char szBuf[512] = { 0 };
	wchar_t sWBuff[512] = { 0 };
	snprintf(szBuf, sizeof(szBuf), "%02u:%02u:%02u ", (uint32)g_localTime.tm_hour, (uint32)g_localTime.tm_min, (uint32)g_localTime.tm_sec);
	MultiByteToWideChar(CP_UTF8, 0, szBuf, -1, sWBuff, sizeof(sWBuff));
	OutputDebugString(sWBuff);
#else
	printf("%02u:%02u:%02u ", (uint32)g_localTime.tm_hour, (uint32)g_localTime.tm_min, (uint32)g_localTime.tm_sec);
#endif
}

void ScreenLog::Line()
{
	putchar('\n');
}

void ScreenLog::Notice(const char * source, const char * format, ...)
{
    if(m_log_level < 0)
        return;
    
    //lock
    std::lock_guard<std::mutex> rGuard(m_lock);
    
    //check is logging is to file
    if(m_pFileLog && m_pFileLog->IsOpen())
    {
        //create message
        size_t len;
        char out[2048];
        
        va_list ap;
        va_start(ap, format);
        len = sprintf(out, "N %s: ", source);
        vsnprintf(&out[len], sizeof(out) - len, format, ap);
        va_end(ap);
        
        //write to disk
        m_pFileLog->write(out);
    }
    else
    {
		/* notice is old loglevel 0/string */
		va_list ap;
		va_start(ap, format);

#if defined(ANDROID)
		__android_log_vprint(ANDROID_LOG_INFO, source, format, ap);
#elif defined(WP8)
		char scBuff[16384] = { 0 };
		wchar_t szBuf[16384] = { 0 };

		Time();
		OutputDebugString(L"N ");
		if (*source)
		{
			//convert to wchar
			wchar_t sWBuff[512] = { 0 };
			MultiByteToWideChar(CP_UTF8, 0, source, -1, sWBuff, sizeof(sWBuff));
			OutputDebugString(sWBuff);
			OutputDebugString(L": ");
		}

		vsnprintf(scBuff, sizeof(scBuff), format, ap);
		//convert to wchar
		MultiByteToWideChar(CP_UTF8, 0, scBuff, -1, szBuf, sizeof(szBuf));
		OutputDebugString(szBuf);
		OutputDebugString(L"\n");
#else
		Time();
		fputs("N ", stdout);
		if (*source)
		{
			Color(TWHITE);
			fputs(source, stdout);
			putchar(':');
			putchar(' ');
			Color(TNORMAL);
		}

		vprintf(format, ap);
		putchar('\n');
		Color(TNORMAL);
#endif

		va_end(ap);
    }
}

void ScreenLog::Warning(const char * source, const char * format, ...)
{
    if(m_log_level < 2)
        return;
    
    //lock
    std::lock_guard<std::mutex> rGuard(m_lock);
    
    //check is logging is to file
    if(m_pFileLog && m_pFileLog->IsOpen())
    {
        //create message
        size_t len;
        char out[2048];
        
        va_list ap;
        va_start(ap, format);
        len = sprintf(out, "W %s: ", source);
        vsnprintf(&out[len], sizeof(out) - len, format, ap);
        va_end(ap);
        
        //write to disk
        m_pFileLog->write(out);
    }
    else
    {
		/* warning is old loglevel 2/detail */
		va_list ap;
		va_start(ap, format);

#if defined(ANDROID)
		__android_log_vprint(ANDROID_LOG_WARN, source, format, ap);
#elif defined(WP8)
		char scBuff[16384] = { 0 };
		wchar_t szBuf[16384] = { 0 };

		Time();
		OutputDebugString(L"W ");
		if (*source)
		{
			//convert to wchar
			wchar_t sWBuff[512] = { 0 };
			MultiByteToWideChar(CP_UTF8, 0, source, -1, sWBuff, sizeof(sWBuff));
			OutputDebugString(sWBuff);
			OutputDebugString(L": ");
		}

		vsnprintf(scBuff, sizeof(scBuff), format, ap);
		//convert to wchar
		MultiByteToWideChar(CP_UTF8, 0, scBuff, -1, szBuf, sizeof(szBuf));
		OutputDebugString(szBuf);
		OutputDebugString(L"\n");
#else

		Time();
		Color(TYELLOW);
		fputs("W ", stdout);
		if (*source)
		{
			Color(TWHITE);
			fputs(source, stdout);
			putchar(':');
			putchar(' ');
			Color(TYELLOW);
		}

		vprintf(format, ap);
		putchar('\n');
		Color(TNORMAL);
#endif

		va_end(ap);
    }
}

void ScreenLog::Success(const char * source, const char * format, ...)
{
    if(m_log_level < 2)
        return;
    
    //lock
    std::lock_guard<std::mutex> rGuard(m_lock);
    
    //check is logging is to file
    if(m_pFileLog && m_pFileLog->IsOpen())
    {
        //create message
        size_t len;
        char out[2048];
        
        va_list ap;
        va_start(ap, format);
        len = sprintf(out, "S %s: ", source);
        vsnprintf(&out[len], sizeof(out) - len, format, ap);
        va_end(ap);
        
        //write to disk
        m_pFileLog->write(out);
    }
    else
    {
		va_list ap;
		va_start(ap, format);

#if defined(ANDROID)
		__android_log_vprint(ANDROID_LOG_INFO, source, format, ap);
#elif defined(WP8)
		char scBuff[16384] = { 0 };
		wchar_t szBuf[16384] = { 0 };

		Time();
		OutputDebugString(L"S ");
		if (*source)
		{
			//convert to wchar
			wchar_t sWBuff[512] = { 0 };
			MultiByteToWideChar(CP_UTF8, 0, source, -1, sWBuff, sizeof(sWBuff));
			OutputDebugString(sWBuff);
			OutputDebugString(L": ");
		}

		vsnprintf(scBuff, sizeof(scBuff), format, ap);
		//convert to wchar
		MultiByteToWideChar(CP_UTF8, 0, scBuff, -1, szBuf, sizeof(szBuf));
		OutputDebugString(szBuf);
		OutputDebugString(L"\n");
#else
		Time();
		Color(TGREEN);
		fputs("S ", stdout);
		if (*source)
		{
			Color(TWHITE);
			fputs(source, stdout);
			putchar(':');
			putchar(' ');
			Color(TGREEN);
		}

		vprintf(format, ap);
		putchar('\n');
		Color(TNORMAL);
#endif

		va_end(ap);
    }
}

void ScreenLog::Error(const char * source, const char * format, ...)
{
    if(m_log_level < 1)
        return;
    
    //lock
    std::lock_guard<std::mutex> rGuard(m_lock);
    
    //check is logging is to file
    if(m_pFileLog && m_pFileLog->IsOpen())
    {
        //create message
        size_t len;
        char out[2048];
        
        va_list ap;
        va_start(ap, format);
        len = sprintf(out, "E %s: ", source);
        vsnprintf(&out[len], sizeof(out) - len, format, ap);
        va_end(ap);
        
        //write to disk
        m_pFileLog->write(out);
    }
    else
    {
		va_list ap;
		va_start(ap, format);

#if defined(ANDROID)
		__android_log_vprint(ANDROID_LOG_ERROR, source, format, ap);
#elif defined(WP8)
		char scBuff[16384] = { 0 };
		wchar_t szBuf[16384] = { 0 };

		Time();
		OutputDebugString(L"E ");
		if (*source)
		{
			//convert to wchar
			wchar_t sWBuff[512] = { 0 };
			MultiByteToWideChar(CP_UTF8, 0, source, -1, sWBuff, sizeof(sWBuff));
			OutputDebugString(sWBuff);
			OutputDebugString(L": ");
		}

		vsnprintf(scBuff, sizeof(scBuff), format, ap);
		//convert to wchar
		MultiByteToWideChar(CP_UTF8, 0, scBuff, -1, szBuf, sizeof(szBuf));
		OutputDebugString(szBuf);
		OutputDebugString(L"\n");
#else
		Time();
		Color(TRED);
		fputs("E ", stdout);
		if (*source)
		{
			Color(TWHITE);
			fputs(source, stdout);
			putchar(':');
			putchar(' ');
			Color(TRED);
		}

		vprintf(format, ap);
		putchar('\n');
		Color(TNORMAL);
#endif

		va_end(ap);
    }
}

void ScreenLog::Debug(const char * source, const char * format, ...)
{
    if(m_log_level < 3)
        return;
    
    //lock
    std::lock_guard<std::mutex> rGuard(m_lock);
    
    //check is logging is to file
    if(m_pFileLog && m_pFileLog->IsOpen())
    {
        //create message
        size_t len;
        char out[2048];
        
        va_list ap;
        va_start(ap, format);
        len = sprintf(out, "D %s: ", source);
        vsnprintf(&out[len], sizeof(out) - len, format, ap);
        va_end(ap);
        
        //write to disk
        m_pFileLog->write(out);
    }
    else
    {
		va_list ap;
		va_start(ap, format);

#if defined(ANDROID)
		__android_log_vprint(ANDROID_LOG_DEBUG, source, format, ap);
#elif defined(WP8)
		char scBuff[16384] = { 0 };
		wchar_t szBuf[16384] = { 0 };

		Time();
		OutputDebugString(L"D ");
		if (*source)
		{
			//convert to wchar
			wchar_t sWBuff[512] = { 0 };
			MultiByteToWideChar(CP_UTF8, 0, source, -1, sWBuff, sizeof(sWBuff));
			OutputDebugString(sWBuff);
			OutputDebugString(L": ");
		}

		vsnprintf(scBuff, sizeof(scBuff), format, ap);
		//convert to wchar
		MultiByteToWideChar(CP_UTF8, 0, scBuff, -1, szBuf, sizeof(szBuf));
		OutputDebugString(szBuf);
		OutputDebugString(L"\n");
#else
		Time();
		Color(TBLUE);
		fputs("D ", stdout);
		if (*source)
		{
			Color(TWHITE);
			fputs(source, stdout);
			putchar(':');
			putchar(' ');
			Color(TBLUE);
		}

		vprintf(format, ap);
		putchar('\n');
		Color(TNORMAL);
#endif

		va_end(ap);
    }
}

FileLog::FileLog(const std::string &sFilePath) : m_hFile(INVALID_HANDLE_VALUE), m_filename(sFilePath)
{
    CommonFunctions::CheckFileExists(m_filename.c_str(), true);
    m_hFile = IO::fopen(m_filename.c_str(), IO::IO_RDWR, IO::IO_NORMAL);
	if(m_hFile == INVALID_HANDLE_VALUE)
    {
        Log.Error(__FUNCTION__, "Cannot create log file on path: %s. Log to file is disabled.", m_filename.c_str());
    }
}

FileLog::~FileLog()
{
    IO::fclose(m_hFile);
    m_hFile = INVALID_HANDLE_VALUE;
}

void FileLog::write(const char* format, ...)
{
    time_t t;
    size_t l;
    tm aTm;
	va_list ap;
	va_start(ap, format);
	char out[4096];

	t = time(NULL);
	localtime(&t, &aTm);
	l = snprintf(out, sizeof(out), "[%-4d-%02d-%02d %02d:%02d:%02d] ", aTm.tm_year+1900, aTm.tm_mon+1, aTm.tm_mday, aTm.tm_hour, aTm.tm_min, aTm.tm_sec);
	l += vsnprintf(&out[l], sizeof(out) - l, format, ap);
    l += snprintf(&out[l], sizeof(out) - l, "\n");
    
    //write to end
    IO::fseek(m_hFile, 0, IO::IO_SEEK_END);
    IO::fwrite(&out, l, m_hFile);

	va_end(ap);
}

void FileLog::getLogFileContent(ByteBuffer &rContent)
{
    int64 fileSize;
    
    IO::fseek(m_hFile, 0, IO::IO_SEEK_END);
    fileSize = IO::ftell(m_hFile);
    IO::fseek(m_hFile, 0, IO::IO_SEEK_SET);
    
    rContent.resize((size_t)fileSize);
    IO::fread((void*)rContent.contents(), rContent.size(), m_hFile);
}







