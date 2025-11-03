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
#ifndef Language_h_seen
#define Language_h_seen

#include <map>
#include "util/classhelper.h"

/** Define to make translation a bit easier.
 * @code
 *   print(T("url"));
 * @endcode
 * This would print the translation to "human" of url, which may be address,
 * dirección, etc.
 */
#define T(s) Language::instance().translate(s)
#include "FileParser.h"

class Language: public FileParser
{
  public:
    const static char * BUILTIN_LANGS[6];
    /** Singleton pattern used. Again. */
    static Language & instance();

    /** Translate a string to the locale specific translation. If a
     * translation doesn't exist, then s returned unchanged.
     * @param s the programmer string
     * @return the user string in the current language in UTF8.
     */
    const std::string &translate(const std::string & s);

    /** Set the language */
    void setLanguage(const std::string & l);

    /** Get the current language name.
     * @return the current language name.
     */
    std::string currentLanguage() const;

    /** Set the language directory where the translation files live.
     * @param dir  the directory name
     */
    void setDirectory(const std::string & dir);

    void callback(const std::string & first, const std::string & second);
  private:
    typedef std::map<std::string, std::string> TranslateMap;
    TranslateMap m_strings;
    std::string m_lang;
    std::string m_directory;

    /** Read language settings.
     */
    Language();
    ~Language();

    void loadLanguageFile();
    void setBuiltin();
    DISALLOW_COPY_AND_ASSIGN(Language);
};

#endif
