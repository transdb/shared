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
#ifndef LOG_H
#define LOG_H

#include "../Threading/Threading.h"
#include "../Packets/ByteBuffer.h"
#include "../IO/IO.h"

#ifdef WIN32
	#define TRED FOREGROUND_RED | FOREGROUND_INTENSITY
	#define TGREEN FOREGROUND_GREEN | FOREGROUND_INTENSITY
	#define TYELLOW FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY
	#define TNORMAL FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE
	#define TWHITE TNORMAL | FOREGROUND_INTENSITY
	#define TBLUE FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY
#else
	#define TRED 1
	#define TGREEN 2
	#define TYELLOW 3
	#define TNORMAL 4
	#define TWHITE 5
	#define TBLUE 6
#endif

class FileLog
{
private:
    DISALLOW_COPY_AND_ASSIGN(FileLog);
	HANDLE          m_hFile;
    std::string     m_filename;
    
public:
	explicit FileLog(const std::string &sFilePath);
	~FileLog();
    
	void write(const char* format, ...);
    void getLogFileContent(ByteBuffer &rContent);
    
 	INLINE bool IsOpen() const
    {
        return (m_hFile != INVALID_HANDLE_VALUE);
    }
    
    INLINE std::string GetFileName() const
    {
        return m_filename;
    }
};

class ScreenLog : public Singleton<ScreenLog>
{
public:
	explicit ScreenLog();
    ~ScreenLog();

	void Notice(const char * source, const char * format, ...);
	void Warning(const char * source, const char * format, ...);
	void Success(const char * source, const char * format, ...);
	void Error(const char * source, const char * format, ...);
	void Debug(const char * source, const char * format, ...);
    
    void CreateFileLog(const std::string &sFilePath);
    
    INLINE void SetLogLevel(int logLevel)
    {
        m_log_level = logLevel;
    }
    
    INLINE void GetFileLogContent(ByteBuffer &rContent)
    {
        std::lock_guard<std::mutex> rGuard(m_lock);
        if(m_pFileLog && m_pFileLog->IsOpen())
        {
            m_pFileLog->getLogFileContent(rContent);
        }
    }
    
private:
    DISALLOW_COPY_AND_ASSIGN(ScreenLog);
    
	void Color(unsigned int color);
	void Time();
	void Line();

    std::mutex                  m_lock;
#ifdef WIN32	
	HANDLE                      m_stdout_handle;
	HANDLE                      m_stderr_handle;
#endif
	int32                       m_log_level;
    std::unique_ptr<FileLog>    m_pFileLog;
};

#define Log ScreenLog::getSingleton()

#endif

