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

#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H

#include "../Defines.h"
#include "../clib/Buffers/CByteBuffer.h"

class ByteBuffer
{
public:
	ByteBuffer()
	{
        bbuff_create(m_pBuff);
        bbuff_reserve(m_pBuff, 512);
	}
    
	ByteBuffer(size_t res)
	{
        bbuff_create(m_pBuff);
        bbuff_reserve(m_pBuff, res);
	}
    
    //copy ctor
	ByteBuffer(const ByteBuffer &buf)
    {
        *this = buf;
    }
    
    //moveable ctor
    ByteBuffer(ByteBuffer &&buf)
    {
        *this = std::move(buf);
    }
    
	virtual ~ByteBuffer()
    {
        bbuff_destroy(m_pBuff);
        m_pBuff = NULL;
    }
    
    ByteBuffer &operator=(const ByteBuffer &buf)
    {
        if(this != &buf)
        {
            bbuff_clear(m_pBuff);
            bbuff_append(m_pBuff, buf.m_pBuff->storage, buf.m_pBuff->size);
            m_pBuff->rpos = buf.m_pBuff->rpos;
            m_pBuff->wpos = buf.m_pBuff->wpos;
        }
        return *this;
    }
    
    ByteBuffer &operator=(ByteBuffer &&buf)
    {
        if(this != &buf)
        {
            m_pBuff = std::move(buf.m_pBuff);
        }
        return *this;
    }
    
	void clear()
	{
        bbuff_clear(m_pBuff);
	}
    
	template <typename T>
    void append(T value)
	{
		append((uint8*)&value, sizeof(value));
	}
    
	template <typename T>
    void put(size_t pos, T value)
	{
		put(pos,(uint8*)&value, sizeof(value));
	}
    
	// stream like operators for storing data
	ByteBuffer &operator<<(bool value)
	{
		append<char>((char)value);
		return *this;
	}
    
	// unsigned
	ByteBuffer &operator<<(uint8 value)
	{
		append<uint8>(value);
		return *this;
	}
    
	ByteBuffer &operator<<(uint16 value)
	{
		append<uint16>(value);
		return *this;
	}
    
	ByteBuffer &operator<<(uint32 value)
	{
		append<uint32>(value);
		return *this;
	}
    
	ByteBuffer &operator<<(uint64 value)
	{
		append<uint64>(value);
		return *this;
	}
    
	// signed as in 2e complement
	ByteBuffer &operator<<(int8 value)
	{
		append<int8>(value);
		return *this;
	}
    
	ByteBuffer &operator<<(int16 value)
	{
		append<int16>(value);
		return *this;
	}
    
	ByteBuffer &operator<<(int32 value)
	{
		append<int32>(value);
		return *this;
	}
    
	ByteBuffer &operator<<(int64 value)
	{
		append<int64>(value);
		return *this;
	}
    
	ByteBuffer &operator<<(float value)
	{
		append<float>(value);
		return *this;
	}
    
	ByteBuffer &operator<<(double value)
	{
		append<double>(value);
		return *this;
	}
    
	ByteBuffer &operator<<(const std::string &value)
	{
		append((uint8*)value.c_str(), value.length());
		append((uint8)0);
		return *this;
	}
    
	ByteBuffer &operator<<(const char *str)
	{
		append((uint8*)str, strlen(str));
		append((uint8)0);
		return *this;
	}
    
	// stream like operators for reading data
	ByteBuffer &operator>>(bool &value)
	{
		value = read<char>() > 0 ? true : false;
		return *this;
	}
    
	//unsigned
	ByteBuffer &operator>>(uint8 &value)
	{
		value = read<uint8>();
		return *this;
	}
    
	ByteBuffer &operator>>(uint16 &value)
	{
		value = read<uint16>();
		return *this;
	}
    
	ByteBuffer &operator>>(uint32 &value)
	{
		value = read<uint32>();
		return *this;
	}
    
	ByteBuffer &operator>>(uint64 &value)
	{
		value = read<uint64>();
		return *this;
	}
    
	//signed as in 2e complement
	ByteBuffer &operator>>(int8 &value)
	{
		value = read<int8>();
		return *this;
	}
    
	ByteBuffer &operator>>(int16 &value)
	{
		value = read<int16>();
		return *this;
	}
    
	ByteBuffer &operator>>(int32 &value)
	{
		value = read<int32>();
		return *this;
	}
    
	ByteBuffer &operator>>(int64 &value)
	{
		value = read<int64>();
		return *this;
	}
    
	ByteBuffer &operator>>(float &value)
	{
		value = read<float>();
		return *this;
	}
    
	ByteBuffer &operator>>(double &value)
	{
		value = read<double>();
		return *this;
	}
    
	ByteBuffer &operator>>(std::string& value)
	{
		value.clear();
		value.reserve(16);
		for(;;)
		{
			char c = read<char>();
			if(c == 0)
                break;
            
			value += c;
		}
		return *this;
	}
    
	uint8 operator[](size_t pos) const
	{
		return read<uint8>(pos);
	}
    
	size_t rpos() const
	{
		return m_pBuff->rpos;
	};
    
	size_t rpos(size_t rpos)
	{
		m_pBuff->rpos = rpos;
		return m_pBuff->rpos;
	}
    
	size_t wpos()
	{
		return m_pBuff->wpos;
	}
    
	size_t wpos(size_t wpos)
	{
		m_pBuff->wpos = wpos;
		return m_pBuff->wpos;
	}
    
	template <typename T>
    T read()
	{
        T r;
        bbuff_read(m_pBuff, &r, sizeof(T));
		return r;
	}
    
	template <typename T>
    T read(size_t pos) const
	{
		if(pos + sizeof(T) > size())
		{
			return (T)0;
		}
		else
		{
            T value;
            bbuff_read(m_pBuff, &value, sizeof(T));
			return value;
		}
	}
    
	void read(uint8 *dest, size_t len)
	{
        bbuff_read(m_pBuff, dest, len);
	}
    
	const uint8 *contents() const
	{
		return m_pBuff->storage;
	}
    
	size_t size() const
	{
		return m_pBuff->size;
	}
    
	void resize(size_t newsize)
	{
        bbuff_resize(m_pBuff, newsize);
	}
    
	void reserve(size_t ressize)
	{
		if(ressize > size())
		{
            bbuff_reserve(m_pBuff, ressize);
		}
	}
    
	void append(const std::string & str)
	{
		append((uint8*)str.c_str(), str.length());
		append<uint8>(0);
	}
    
	void append(const char * src, size_t cnt)
	{
		append((const uint8*)src, cnt);
	}
    
	void append(const void * src, size_t cnt)
	{
		append((const uint8*)src, cnt);
	}
    
	void append(const uint8 *src, size_t cnt)
	{
        bbuff_append(m_pBuff, src, cnt);
	}
    
	void append(const ByteBuffer& buffer)
	{
		if(buffer.size() > 0)
		{
			append(buffer.contents(), buffer.size());
		}
	}
    
	void put(size_t pos, const uint8 * src, size_t cnt)
	{
        bbuff_put(m_pBuff, pos, src, cnt);
	}
    
	void hexlike()
	{
		size_t j = 1, k = 1;
		printf("STORAGE_SIZE: %u\n", (unsigned int)size());
		for(size_t i = 0; i < size(); i++)
		{
			if ((i == (j*8)) && ((i != (k*16))))
			{
				if (read<uint8>(i) <= 0x0F)
				{
					printf("| 0%X ", read<uint8>(i));
				}
				else
				{
					printf("| %X ", read<uint8>(i));
				}
                
				j++;
			}
			else if (i == (k*16))
			{
				rpos(rpos()-16);	// move read pointer 16 places back
				printf(" | ");	  // write split char
                
				for (int x = 0; x <16; x++)
				{
					printf("%c", read<uint8>(i-16 + x));
				}
                
				if (read<uint8>(i) <= 0x0F)
				{
					printf("\n0%X ", read<uint8>(i));
				}
				else
				{
					printf("\n%X ", read<uint8>(i));
				}
                
				k++;
				j++;
			}
			else
			{
				if (read<uint8>(i) <= 0x0F)
				{
					printf("0%X ", read<uint8>(i));
				}
				else
				{
					printf("%X ", read<uint8>(i));
				}
			}
		}
		printf("\n");
	}
    
	void reverse()
	{
		std::reverse(m_pBuff->storage, m_pBuff->storage + m_pBuff->size);
	}
    
protected:
    bbuff*  m_pBuff;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template <typename T> ByteBuffer &operator<<(ByteBuffer &b, std::vector<T> v)
{
	b <<(uint32)v.size();
	for (typename std::vector<T>::iterator i = v.begin(); i != v.end(); ++i) {
		b <<*i;
	}
	return b;
}

template <typename T> ByteBuffer &operator>>(ByteBuffer &b, std::vector<T> &v)
{
	uint32 vsize;
	b>> vsize;
	v.clear();
	while(vsize--) {
		T t;
		b>> t;
		v.push_back(t);
	}
	return b;
}

template <typename T> ByteBuffer &operator<<(ByteBuffer &b, std::list<T> v)
{
	b <<(uint32)v.size();
	for (typename std::list<T>::iterator i = v.begin(); i != v.end(); ++i) {
		b <<*i;
	}
	return b;
}

template <typename T> ByteBuffer &operator>>(ByteBuffer &b, std::list<T> &v)
{
	uint32 vsize;
	b>> vsize;
	v.clear();
	while(vsize--) {
		T t;
		b>> t;
		v.push_back(t);
	}
	return b;
}

template <typename K, typename V> ByteBuffer &operator<<(ByteBuffer &b, std::map<K, V> &m)
{
	b << (uint32)m.size();
	for (typename std::map<K, V>::iterator i = m.begin(); i != m.end(); ++i)
    {
		b << i->first << i->second;
	}
	return b;
}

template <typename K, typename V> ByteBuffer &operator>>(ByteBuffer &b, std::map<K, V> &m)
{
	uint32 msize;
	b >> msize;
	m.clear();
	while(msize--)
    {
		K k;
		V v;
		b >> k >> v;
		m.insert(make_pair(k, v));
	}
	return b;
}

#endif
