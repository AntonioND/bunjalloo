#ifndef Version_h_seen
#define Version_h_seen

class Version
{
  public:
    Version(const char * version);
    bool operator >(const Version & other) const;
    bool operator <(const Version & other) const;
    bool operator ==(const Version & other) const;
    bool operator !=(const Version & other) const;
  private:
    int m_major;
    int m_minor;
    int m_point;
};
#endif
