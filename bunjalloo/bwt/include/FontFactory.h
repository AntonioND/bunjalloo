#ifndef FontFactory_h_seen
#define FontFactory_h_seen

class Font;

class FontFactory
{
  public:
    /**
     * Create a Font from a file name.
     * @param name the name of the base font.
     * @return a new Font (you are responsible for freeing it)
     */
    static Font *create(const char *name);

    /**
     *  Create a Font from raw data.
     * @param imageData raw image data
     * @param mapData raw map data
     * @return a new Font (you are responsible for freeing it)
     */
    static Font *create(const unsigned char * imageData, const unsigned char * mapData);
};
#endif
