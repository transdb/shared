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

#include "Config.h"
#include "../Logs/Log.h"

ConfigMgr g_rConfig;

//#define _CONFIG_DEBUG

ConfigFile::ConfigFile()
{
}

ConfigFile::~ConfigFile()
{	
}

static void remove_spaces(std::string& str)
{
	while(str.size() && (*str.begin() == ' ' || *str.begin() == '\t'))
		str.erase(str.begin());
}

static void remove_all_spaces(std::string& str)
{
	std::string::size_type off = str.find(" ");
	while(off != std::string::npos)
	{
		str.erase(off, 1);
		off = str.find(" ");
	}

	off = str.find("\t");
	while(off != std::string::npos)
	{
		str.erase(off, 1);
		off = str.find("\t");
	}
}

static bool is_comment(std::string& str, bool * in_multiline_quote)
{
	std::string stemp = str;
	remove_spaces(stemp);
	if(stemp.length() == 0)
		return false;

	if(stemp[0] == '/')
	{
		if(stemp.length() <2)
			return false;

		if(stemp[1] == '*')
		{
			*in_multiline_quote = true;
			return true;
		}
		else if(stemp[2] == '/')
		{
			return true;
		}
	}

	if(stemp[0] == '#')
		return true;

	return false;
}

static void apply_setting(std::string & str, ConfigSetting & setting)
{
	setting.AsString = str;
	setting.AsInt = atoi(str.c_str());
	setting.AsBool = (setting.AsInt > 0);
	setting.AsFloat = (float)atof(str.c_str());

	/* check for verbal yes/no answers */
	if(str.length() > 1)
	{
		// this might be a yes/no?
		if(str.size() >= 3 && !strnicmp("yes", str.c_str(), 3))
		{
			setting.AsBool = true;
			setting.AsInt = 1;
		}
		else if(str.size() >= 2 && !strnicmp("no", str.c_str(), 2))
		{
			setting.AsBool = false;
			setting.AsInt = 0;
		}
	}
}

bool ConfigFile::SetSource(const char *file)
{
	/* wipe any existing settings. */
	m_settings.clear();

	/* open the file */
	if(file != 0)
	{
        //save patj to config file
        m_rFilePath = std::string(file);
        
        //start parsing config file
		std::ifstream inputData(file);
		if(!inputData.is_open())
		{
			Log.Debug(__FUNCTION__, "Could not open %s.", file);
			return false;
		}

		/* let's parse it. */
		std::string line;
		std::string::size_type end;
		std::string::size_type offset;
		bool in_multiline_comment = false;
		bool in_multiline_quote = false;
		bool in_block = false;
		std::string current_setting = "";
		std::string current_variable = "";
		std::string current_block = "";
		ConfigBlock current_block_map;
		ConfigSetting current_setting_struct;

		for(;;)
		{
			if(inputData.eof())
				break;

			/* grab a line. */
			getline(inputData, line);

parse:
			if(!line.size())
				continue;

			/* are we a comment? */
			if(!in_multiline_comment && is_comment(line, &in_multiline_comment))
			{
				/* our line is a comment. */
				if(!in_multiline_comment)
				{
					/* the entire line is a comment, skip it. */
					continue;
				}
			}

			/* handle our cases */
			if(in_multiline_comment)
			{
				// we need to find a "*/".
				offset = line.find("*/", 0);
				
				/* skip this entire line, eh? */
				if(offset == std::string::npos)
					continue;

				/* remove up to the end of the comment block. */
				line.erase(0, offset + 2);
				in_multiline_comment = false;
			}
		
			if(in_block)
			{
				/* handle settings across multiple lines */
				if(in_multiline_quote)
				{
					/* attempt to find the end of the quote block. */
					offset = line.find("\"");

					if(offset == std::string::npos)
					{
						/* append the whole line to the quote. */
						current_setting += line;
						current_setting += "\n";
						continue;
					}
	                
					/* only append part of the line to the setting. */
					current_setting.append(line.c_str(), offset+1);
					line.erase(0, offset + 1);
				
					/* append the setting to the config block. */
					if(current_block == "" || current_variable == "")
					{
						Log.Debug(__FUNCTION__, "Quote without variable.");
						return false;
					}

					/* apply the setting */
					apply_setting(current_setting, current_setting_struct);

					/* the setting is done, append it to the current block. */
                    current_block_map.insert(make_pair(current_variable, current_setting_struct));
                    
#ifdef _CONFIG_DEBUG
					printf("Block: '%s', Setting: '%s', Value: '%s'\n", current_block.c_str(), current_variable.c_str(), current_setting_struct.AsString.c_str());
#endif
					/* no longer doing this setting, or in a quote. */
					current_setting = "";
					current_variable = "";
                    in_multiline_quote = false;					
				}

				/* remove any leading spaces */
				remove_spaces(line);

				if(!line.size())
					continue;

				/* our target is a *setting*. look for an '=' sign, this is our seperator. */
                offset = line.find("=");
				if(offset != std::string::npos)
				{
					assert(current_variable == "");
					current_variable = line.substr(0, offset);

					/* remove any spaces from the end of the setting */
					remove_all_spaces(current_variable);

					/* remove the directive *and* the = from the line */
					line.erase(0, offset + 1);
				}

				/* look for the opening quote. this signifies the start of a setting. */
				offset = line.find("\"");
				if(offset != std::string::npos)
				{
					assert(current_setting == "");
					assert(current_variable != "");

					/* try and find the ending quote */
					end = line.find("\"", offset + 1);
					if(end != std::string::npos)
					{
						/* the closing quote is on the same line, oh goody. */
						current_setting = line.substr(offset+1, end-offset-1);

						/* erase up to the end */
						line.erase(0, end + 1);
						
						/* apply the setting */
						apply_setting(current_setting, current_setting_struct);

						/* the setting is done, append it to the current block. */
                        current_block_map.insert(make_pair(current_variable, current_setting_struct));

#ifdef _CONFIG_DEBUG
						printf("Block: '%s', Setting: '%s', Value: '%s'\n", current_block.c_str(), current_variable.c_str(), current_setting_struct.AsString.c_str());
#endif
						/* no longer doing this setting, or in a quote. */
						current_setting = "";
						current_variable = "";
						in_multiline_quote = false;		

						/* attempt to grab more settings from the same line. */
						goto parse;
					}
					else
					{
						/* the closing quote is not on the same line. means we'll try and find it on
						   the next. */
						current_setting.append(line.c_str(), offset);

						/* skip to the next line. (after setting our condition first, of course :P */
						in_multiline_quote = true;
						continue;
					}
				}

				/* are we at the end of the block yet? */
				offset = line.find(">");
				if(offset != std::string::npos)
				{
					line.erase(0, offset+1);

					// freeeee!
					in_block = false;
					
					/* assign this block to the main "big" map. */
                    m_settings.insert(ConfigSettings::value_type(current_block, current_block_map));

					/* erase all data for this so it doesn't seep through */
					current_block_map.clear();
					current_setting = "";
					current_variable = "";
					current_block = "";
				}
			}
			else
			{
				/* we're not in a block. look for the start of one. */
				offset = line.find("<");

				if(offset != std::string::npos)
				{
					in_block = true;

					/* whee, a block! let's cut the string and re-parse. */
					line.erase(0, offset + 1);

					/* find the name of the block first, though. */
					offset = line.find(" ");
					if(offset != std::string::npos)
					{
						current_block = line.substr(0, offset);
						line.erase(0, offset + 1);
					}
					else
					{
						Log.Debug(__FUNCTION__, "Block without name.");
						return false;
					}

					/* skip back */
					goto parse;
				}
			}
		}

		//close stream
		inputData.close();

		/* handle any errors */
		if(in_block)
		{
			Log.Debug(__FUNCTION__, "Unterminated block.");
			return false;
		}

		if(in_multiline_comment)
		{
			Log.Debug(__FUNCTION__, "Unterminated comment.");
			return false;
		}

		if(in_multiline_quote)
		{
			Log.Debug(__FUNCTION__, "Unterminated quote.");
			return false;
		}

		/* we're all good :) */
		return true;
	}

	return false;
}

ConfigSetting * ConfigFile::GetSetting(const char * Block, const char * Setting)
{
	/* find it in the big map */
    ConfigSettings::iterator itr = m_settings.find(Block);
	if(itr != m_settings.end())
	{
		ConfigBlock::iterator it2 = itr->second.find(Setting);
		if(it2 != itr->second.end())
			return &it2->second;
        else
            return NULL;
	}
	return NULL;
}

bool ConfigFile::GetString(const char * block, const char* name, std::string *value)
{
	ConfigSetting * Setting = GetSetting(block, name);
	if(Setting == 0)
		return false;

	*value = Setting->AsString;
	return true;
}


std::string ConfigFile::GetStringDefault(const char * block, const char* name, const char* def)
{
	std::string ret;
	return GetString(block, name, &ret) ? ret : def;
}


bool ConfigFile::GetBool(const char * block, const char* name, bool *value)
{
	ConfigSetting * Setting = GetSetting(block, name);
	if(Setting == 0)
		return false;

	*value = Setting->AsBool;
	return true;
}


bool ConfigFile::GetBoolDefault(const char * block, const char* name, const bool def /* = false */)
{
	bool val;
	return GetBool(block, name, &val) ? val : def;
}

bool ConfigFile::GetInt(const char * block, const char* name, int *value)
{
	ConfigSetting * Setting = GetSetting(block, name);
	if(Setting == 0)
		return false;

	*value = Setting->AsInt;
	return true;
}

bool ConfigFile::GetFloat(const char * block, const char* name, float *value)
{
	ConfigSetting * Setting = GetSetting(block, name);
	if(Setting == 0)
		return false;

	*value = Setting->AsFloat;
	return true;
}

int ConfigFile::GetIntDefault(const char * block, const char* name, const int def)
{
	int val;
	return GetInt(block, name, &val) ? val : def;
}

float ConfigFile::GetFloatDefault(const char * block, const char* name, const float def)
{
	float val;
	return (GetFloat(block, name, &val) ? val : def);
}

int ConfigFile::GetIntVA(const char * block, int def, const char* name, ...)
{
	va_list ap;
	va_start(ap, name);
	char str[256];
	vsnprintf(str, sizeof(str), name, ap);
	va_end(ap);
	int val;
	return GetInt(str, block, &val) ? val : def;
}

float ConfigFile::GetFloatVA(const char * block, float def, const char* name, ...)
{
	va_list ap;
	va_start(ap, name);
	char str[256];
	vsnprintf(str, sizeof(str), name, ap);
	va_end(ap);
	float val;
	return GetFloat(str, block, &val) ? val : def;
}

std::string ConfigFile::GetStringVA(const char * block, const char* def, const char * name, ...)
{
	va_list ap;
	va_start(ap, name);
	char str[256];
	vsnprintf(str, sizeof(str), name, ap);
	va_end(ap);

	return GetStringDefault(str, block, def);
}

bool ConfigFile::GetString(const char * block, char * buffer, const char * name, const char * def, size_t len)
{
	std::string val = GetStringDefault(block, name, def);
	size_t blen = val.length();
	if(blen > len)
		blen = len;

	memcpy(buffer, val.c_str(), blen);
	buffer[blen] = 0;
    
	return true;
}

