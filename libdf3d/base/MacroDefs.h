#pragma once

static const int DEFAULT_WINDOW_WIDTH = 640;
static const int DEFAULT_WINDOW_HEIGHT = 480;

#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define SAFE_DELETE(x) delete x; x = nullptr;
#define SAFE_ARRAY_DELETE(x) delete [] x; x = nullptr;

#define FWD_MODULE_CLASS(_namespace, _class) namespace df3d { namespace _namespace { class _class; } }

#if defined(max)
#undef max
#endif

#if defined(min)
#undef min
#endif