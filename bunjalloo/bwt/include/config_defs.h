#define sprintf_platform sprintf
#define sscanf_platform sscanf

#ifdef __NDS__
#define DATADIR "data/bunjalloo"
#else
#define DATADIR "sdroot/data/bunjalloo"
#endif
