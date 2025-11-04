#ifndef file_wrapper_h_seen
#define file_wrapper_h_seen

class FileWrapper
{
  public:
    FileWrapper();
    ~FileWrapper();

    void open(const char *filename, const char *mode);
    bool is_open() const;

    void write8(unsigned int value);
    void write16(unsigned int value);
    void write32(unsigned int value);
    void writeData(const void *data, int count);

  private:
    FILE *m_fp;
};

#endif
