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

#include "CommonFunctions.h"

#define GZIP_ENCODING				16
#define ZLIB_CHUNK                  16384

int CommonFunctions::decompressGzip(const uint8 *pData, const size_t &dataLen, ByteBuffer &rBuffOut)
{
    int ret;
    size_t processed;
    uInt buffSize = ZLIB_CHUNK;
    uint8 outBuff[ZLIB_CHUNK];
    
    //we need clean buffer
    rBuffOut.clear();
    
    //set up stream
    z_stream stream;
    memset(&stream, 0, sizeof(z_stream));
    
    //init for gzip
    ret = inflateInit2(&stream, GZIP_ENCODING+MAX_WBITS);
    if(ret != Z_OK)
        return ret;
    
    /* decompress until deflate stream ends or end of file */
    stream.avail_in = static_cast<uInt>(dataLen);
    stream.next_in = (Bytef*)pData;
    
    /* run inflate() on input until output buffer not full */
    do
    {
        stream.avail_out = buffSize;
        stream.next_out = outBuff;
        ret = inflate(&stream, Z_FINISH);
        switch (ret)
        {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
            {
                inflateEnd(&stream);
                return ret;
            }
        }
        
        processed = buffSize - stream.avail_out;
        rBuffOut.append(outBuff, processed);
        
    }while(stream.avail_out == 0);
    
    /* clean up and return */
    inflateEnd(&stream);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

int CommonFunctions::compressGzip(const int &compressionLevel, const uint8 *pData, const size_t &dataLen, ByteBuffer &rBuffOut)
{
    int ret;
    uint32 processed;
    uInt buffSize = ZLIB_CHUNK;
    uint8 outBuff[ZLIB_CHUNK];
    
    //we need clean buffer
    rBuffOut.clear();
    
    //set up stream
    z_stream stream;
    memset(&stream, 0, sizeof(z_stream));
    
    //init for gzip
    ret = deflateInit2(&stream, compressionLevel, Z_DEFLATED, GZIP_ENCODING+MAX_WBITS, MAX_MEM_LEVEL, Z_DEFAULT_STRATEGY);
    if(ret != Z_OK)
        return ret;
    
    //setup input informations
    stream.next_in = (Bytef*)pData;
    stream.avail_in = static_cast<uInt>(dataLen);
    
    do
    {
        stream.avail_out = buffSize;
        stream.next_out = outBuff;
        ret = deflate(&stream, Z_FINISH);
        switch (ret)
        {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
            {
                deflateEnd(&stream);
                return ret;
            }
        }
        
        processed = buffSize - stream.avail_out;
        rBuffOut.append(outBuff, processed);
        
    }while(stream.avail_out == 0);
    
    /* clean up and return */
    deflateEnd(&stream);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

bool CommonFunctions::CheckFileExists(const char *pFileName, bool oCreate)
{
	bool oReturnVal = true;
	FILE * rFile;
	rFile = fopen(pFileName, "rb");
	if(rFile == NULL && oCreate)
	{
		//create file
		rFile = fopen(pFileName, "wb+");
		oReturnVal = false;
	}
    
	if(rFile)
	{
		fclose(rFile);
	}
	
	return oReturnVal;
}

time_t CommonFunctions::GetLastFileModificationTime(const char *pFilePath)
{
#ifdef WIN32
	struct _stat rStat;
	if(_stat(pFilePath, &rStat) != -1)
		return rStat.st_mtime;
	else
		return 0;
#else
	struct stat rStat;
    if(stat(pFilePath, &rStat) != -1)
        return rStat.st_mtime;
    else
		return 0;
#endif
}


std::vector<std::string> CommonFunctions::StrSplit(const std::string & src, const std::string & sep)
{
	std::vector<std::string>	tokens;
	std::string					item;
	std::stringstream			ss(src);
    
	while(std::getline(ss, item, sep[0]))
	{
		tokens.push_back(item);
	}
    
	return tokens;
}

void CommonFunctions::replace(std::string &str, const char* find, const char* rep, uint32 limit)
{
	uint32 i = 0;
	std::string::size_type pos = 0;
	while((pos = str.find(find, pos)) != std::string::npos)
	{
		str.erase(pos, strlen(find));
		str.insert(pos, rep);
		pos += strlen(rep);

		++i;
		if(limit != 0 && i == limit)
			break;
	}
}

std::string CommonFunctions::FormatOutputString(const char * Prefix, const char * Description, bool useTimeStamp)
{
	char p[MAX_PATH];
	memset(&p, 0, sizeof(p));
	tm *a;
	time_t t = time(NULL);
	a = gmtime(&t);
	strcat(p, Prefix);
	strcat(p, "/");
	strcat(p, Description);
	if(useTimeStamp)
	{
		char ftime[128];
		memset(&ftime, 0, sizeof(ftime));
		snprintf(ftime, sizeof(ftime), "-%-4d-%02d-%02d %02d-%02d-%02d", a->tm_year+1900, a->tm_mon+1, a->tm_mday, a->tm_hour, a->tm_min, a->tm_sec);
		strcat(p, ftime);
	}

	strcat(p, ".log");
	return std::string(p);
}

void CommonFunctions::SetThreadName(const char* format, ...)
{
	// This isn't supported on nix?
	va_list ap;
	va_start(ap, format);
    
	char thread_name[256];
	vsnprintf(thread_name, sizeof(thread_name), format, ap);
    
#ifdef WIN32
#pragma pack(push,8)
	typedef struct tagTHREADNAME_INFO
	{
		DWORD dwType;		// must be 0x1000
		LPCSTR szName;		// pointer to name (in user addr space)
		DWORD dwThreadID;	// thread ID (-1=caller thread)
		DWORD dwFlags;		// reserved for future use, must be zero
	} THREADNAME_INFO;
#pragma pack(pop)

	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.dwThreadID = GetCurrentThreadId();
	info.dwFlags = 0;
	info.szName = thread_name;

	__try
	{
#ifdef X64
		RaiseException(0x406D1388, 0, sizeof(info)/sizeof(DWORD), (ULONG_PTR*)&info);
#else
		RaiseException(0x406D1388, 0, sizeof(info)/sizeof(DWORD), (DWORD*)&info);
#endif
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{

	}
#else
#ifdef MAC
    pthread_setname_np(thread_name);
#else
    pthread_setname_np(pthread_self(), thread_name);
#endif
#endif

	va_end(ap);
}





















