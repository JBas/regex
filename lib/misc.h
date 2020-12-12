
#define DEBUG

#ifdef DEBUG
#define dbg_printf(...) printf("%s", __VA_ARGS__)
#else
#define dbg_printf(...)
#endif

int gobble(const char* pattern, size_t plen, size_t i, const char m, const char mr, size_t *j);
