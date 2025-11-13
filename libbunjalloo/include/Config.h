/*
  Copyright (C) 2007,2008 Richard Quirk

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef Config_h_seen
#define Config_h_seen

#include <string>
#include <vector>
#include "ParameterSet.h"
#include "FileParser.h"

/** Configuration file parser. */
class Config: public FileParser
{
  public:
    static const char PROXY_STR[];
    static const char FONT_STR[];
    static const char COOKIE_STR[];
    static const char CERT_FILE[];
    static const char CERT_CHECK_ENABLE[];
    static const char SEARCHFILE_STR[];
    static const char MAX_CONNECT[];
    static const char USECACHE[];
    static const char CLEARCACHE[];
    static const char DOWNLOAD[];
    static const char UPDATE[];
    static const char USER_AGENT_STR[];
    static const char FULL_REF[];
    static const char SHOW_IMAGES[];
    static const char BOOKMARK_URL[];
    static const char LEFTY[];

    /** Initialise the config class. Set the document parser and the controller.  */
    Config();

    /** End of life time. */
    ~Config();

    /** Get a configuration value as a string.
     * @param name the name of the parameter.
     * @param value the value of the parameter.
     * @return true if the value is setm, false otherwise.
     */
    bool resource(const std::string & name, std::string & value) const;

    /** Get a configuration value as a bool. */
    bool resource(const std::string & name, bool & value) const;

    /** Get a configuration value as an int. */
    bool resource(const std::string & name, int & value) const;

    /** Reload the config file.
     */
    void reload();

    /** Copy a template config file from src to dst, stripping blank lines and comments.
     * @param src the source file
     * @param dst the destination file
     */
    static void copyTemplate(const char * src, const char * dst);

    void callback(const std::string & first, const std::string & second);

    /**
     * Parse and update the configuration based on user input.
     * @param postedUrl the url with config data.
     */
    void postConfiguration(const std::string & postedUrl);

    /** Pre configuration check.  */
    void checkPre();
    /** Post configuration check.  */
    void checkPost();
  private:
    KeyValueMap m_resources;

    void configPathMember(const std::string & value, std::string & member);
    void updateConfig(const std::string & first, const std::string & second,
        std::vector<std::string> & lines);
};

#endif
