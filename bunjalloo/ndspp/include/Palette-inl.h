#ifndef Palette_inl_h_seen
#define Palette_inl_h_seen

inline unsigned int Color::red() const
{
  return m_c&31;
}
inline void Color::red(unsigned int c)
{
  set(c, green(), blue());
}
inline unsigned int Color::green() const
{
  return (m_c>>5)&31;
}
inline void Color::green(unsigned int c)
{
  set(red(), c, blue());
}
inline unsigned int Color::blue() const
{
  return (m_c>>10)&31;
}
inline void Color::blue(unsigned int c)
{
  set(red(), green(), c);
}
inline Color::operator unsigned short () const {
  return m_c;
};

inline const Color Palette::operator[] (int i) const
{
  return m_p[i];
};
inline unsigned short & Palette::operator[] (int i)
{
  return m_p[i];
};
#endif
