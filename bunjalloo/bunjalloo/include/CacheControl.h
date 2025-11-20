#ifndef CacheControler_h_seen
#define CacheControler_h_seen

#include <string>
#include <time.h>

class CacheControl
{
  public:

    CacheControl();

    /**
     * Reset cache flags to default.
     */
    void reset();

    /** Set the age header value.
     * @param age the age header.
     */
    void setAge(time_t age);

    /**
     * Set the cache-control header value.
     * @param value the cache-control header value that needs parsing
     */
    void setCacheControl(const std::string &value);

    /**
     * Set the date header.
     * @param date the date of the server
     */
    void setDate(time_t date);

    /**
     * Set the expires header.
     * @param expires the time since epoch to exires
     */
    void setExpires(time_t expires);

    /**
     * Set the time when the request was made.
     * @param response the time of the request
     */
    void setRequestTime(time_t request);

    /**
     * Set the time when the response was received.
     * @param response the time of the response
     */
    void setResponseTime(time_t response);

    /**
     * Set the last-modified header
     * @param lastmodified the last modified date
     */
    void setLastModified(time_t lastmodified);

    /**
     * Find out if we should be caching the page based on what we know.
     * @param now the current time (for dependency injection reasons...)
     * @returns true if we should cache the page
     */
    bool shouldCache(time_t now) const;
  private:
    bool m_noCache;
    int m_ageValue;
    int m_date;
    int m_expires;
    int m_maxAge;
    int m_lastModified;
    time_t m_requestTime;
    time_t m_responseTime;
};

#endif
