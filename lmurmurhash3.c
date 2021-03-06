#include <stdint.h>
#include "lua.h"
#include "lauxlib.h"


#define LIBNAME		"murmurhash3"
#define LIBVERSION	"1.3.0"


uint32_t murmur3_32(const char *key, uint32_t len, uint32_t seed) {
	static const uint32_t c1 = 0xcc9e2d51;
	static const uint32_t c2 = 0x1b873593;
	static const uint32_t r1 = 15;
	static const uint32_t r2 = 13;
	static const uint32_t m = 5;
	static const uint32_t n = 0xe6546b64;

	uint32_t hash = seed;

	const int nblocks = len / 4;
	const uint32_t *blocks = (const uint32_t *) key;
	int i;
	for (i = 0; i < nblocks; i++) {
		uint32_t k = blocks[i];
		k *= c1;
		k = (k << r1) | (k >> (32 - r1));
		k *= c2;

		hash ^= k;
		hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
	}

	const uint8_t *tail = (const uint8_t *) (key + nblocks * 4);
	uint32_t k1 = 0;

	switch (len & 3) {
	case 3:
		k1 ^= tail[2] << 16;
	case 2:
		k1 ^= tail[1] << 8;
	case 1:
		k1 ^= tail[0];

		k1 *= c1;
		k1 = (k1 << r1) | (k1 >> (32 - r1));
		k1 *= c2;
		hash ^= k1;
	}

	hash ^= len;
	hash ^= (hash >> 16);
	hash *= 0x85ebca6b;
	hash ^= (hash >> 13);
	hash *= 0xc2b2ae35;
	hash ^= (hash >> 16);

	return hash;
}


static int l_hash_x86_32 (lua_State *L) {

	uint32_t seed = 0;
	double num;
	size_t len;
	uint32_t hash;

	size_t arguments = lua_gettop(L);

	const char *s = luaL_checklstring(L, 1, &len);

	if (arguments > 1 ) {

		num = luaL_checknumber(L, 2);

		if (num < 0 || num > 0xffffffff) {

			lua_pushstring(L, "Argument 2 must be a Uint32");
			return lua_error(L);
		}

		seed = num;
	}

	hash = murmur3_32(s, len, seed);

	lua_pushnumber(L, hash);
	return 1;
}

static int l_hash_x86_32_array (lua_State *L) {

	uint32_t seed = 0;
	double num;
	size_t len;
	uint32_t hash;

	size_t arguments = lua_gettop(L);

	luaL_checktype(L, 1, LUA_TTABLE);
	


	if (arguments > 1 ) {

		num = luaL_checknumber(L, 2);

		if (num < 0 || num > 0xffffffff) {

			lua_pushstring(L, "Argument 2 must be a Uint32");
			return lua_error(L);
		}

		seed = num;
	}

	len = lua_objlen(L, 1);

	static const uint32_t c1 = 0xcc9e2d51;
	static const uint32_t c2 = 0x1b873593;
	static const uint32_t r1 = 15;
	static const uint32_t r2 = 13;
	static const uint32_t m = 5;
	static const uint32_t n = 0xe6546b64;

	hash = seed;

	const int nblocks = len / 4;
	int i, b;
	for (i = 0; i < nblocks; i++) {
		uint32_t k = 0;
		for (b = 4; b > 0; --b) {
			lua_rawgeti(L, 1, i * 4 + b); k = (k << 8) | (lua_tointeger(L, -1) & 0xFF); lua_pop(L, 1);
		}

		k *= c1;
		k = (k << r1) | (k >> (32 - r1));
		k *= c2;

		hash ^= k;
		hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
	}

	uint32_t k1 = 0;

	switch (len & 3) {
	case 3:
		lua_rawgeti(L, 1, nblocks * 4 + 3); k1 ^= (lua_tointeger(L, -1) & 0xFF) << 16; lua_pop(L, 1);
	case 2:
		lua_rawgeti(L, 1, nblocks * 4 + 2); k1 ^= (lua_tointeger(L, -1) & 0xFF) <<  8; lua_pop(L, 1);
	case 1:
		lua_rawgeti(L, 1, nblocks * 4 + 1); k1 ^= (lua_tointeger(L, -1) & 0xFF)      ; lua_pop(L, 1);

		k1 *= c1;
		k1 = (k1 << r1) | (k1 >> (32 - r1));
		k1 *= c2;
		hash ^= k1;
	}

	hash ^= len;
	hash ^= (hash >> 16);
	hash *= 0x85ebca6b;
	hash ^= (hash >> 13);
	hash *= 0xc2b2ae35;
	hash ^= (hash >> 16);


	lua_pushnumber(L, hash);
	return 1;
}


static const struct luaL_Reg R[] =
{
	{ "hash32",	l_hash_x86_32},
	{ "hash32_array",	l_hash_x86_32_array},
	{ NULL,		NULL	}
};


LUALIB_API int luaopen_murmurhash3(lua_State *L)
{
#if !defined(LUA_VERSION_NUM) || LUA_VERSION_NUM < 502
	luaL_register(L, LIBNAME, R);
#else
	lua_newtable(L);
	luaL_setfuncs(L, R, 0);
#endif
	lua_pushliteral(L, "version");
	lua_pushliteral(L, LIBVERSION);
	lua_settable(L, -3);

	return 1;
}
