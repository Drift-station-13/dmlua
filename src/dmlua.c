#include "lua/lua.h"
#include "lua/lauxlib.h"
#include <string.h>
#include <windows.h>

lua_State *state;

#define STRING_TYPE 0
#define INT_TYPE 1

struct Var
{
	char *name;
	int type;
	char *sval;
	int ival;
};

struct Var *entries[64] = {0};

void init(void){
	if (!state)
	{
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		freopen("CONIN$", "r", stdin);
		
		state = luaL_newstate();
		luaL_openlibs(state);
	}
}

__declspec(dllexport) const char **clear_data(int num_args, const char **args)
{
	for (int i = 0; i < 64; i++)
	{
		if (entries[i] != NULL)
			free(entries[i]);
		entries[i] = NULL;
	}
	return "";
}

__declspec(dllexport) const char **set_data(int num_args, const char **args)
{
	if (num_args != 3)
		return "bad num args";
	const char *name = args[0];
	const char *type = args[1];
	const char *data = args[2];
	for (int i = 0; i < 64; i++)
	{
		if (entries[i] == NULL)
		{
			entries[i] = malloc(sizeof(struct Var));
			if (strcmp(type, "0") == 0)
			{
				entries[i]->name = strdup(name);
				entries[i]->type = 0;
				entries[i]->sval = strdup(data);
				entries[i]->ival = 0;
			}
			else
			{
				entries[i]->name = strdup(name);
				entries[i]->type = 1;
				entries[i]->sval = "";
				entries[i]->ival = atoi((data));
			}
			break;
		}
	}
	return "set";
}

__declspec(dllexport) const char **run(int num_args, const char **args)
{
	if (num_args != 1)
		return "bad num args";
	init();
	const char *file = strdup(args[0]);
	lua_newtable(state);
	for (int i = 0; i < 64; i++)
	{
		if (entries[i] == NULL)
			break;
		switch (entries[i]->type)
		{
		case 0:
			lua_pushstring(state, entries[i]->sval);
			break;
		case 1:
			lua_pushnumber(state, entries[i]->ival);
			break;
		default:
			break;
		}
		lua_setfield(state, -2, entries[i]->name);
	}
	lua_setglobal(state, "data");
	lua_pushstring(state, "");
	lua_setglobal(state, "ret");
	lua_pop(state, lua_gettop(state));
	luaL_dofile(state, "lua/global.lua");
	luaL_dofile(state, file);
	lua_getglobal(state, "ret");
	char* ret = luaL_checkstring(state, 1);
	if (ret == NULL)
		return "err";
	return strdup(ret);
}

// misc functions
__declspec(dllexport) const char **external_print(int num_args, const char **args){
	if (num_args != 1)
		return "bad num args";
	init();
	printf(args[0]);
	return "";
}