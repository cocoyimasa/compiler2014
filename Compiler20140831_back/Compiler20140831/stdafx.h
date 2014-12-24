#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <memory>
#include <string>
#include <vector>
#include <iterator>
#include <map>
#include <cstdlib>
#include <crtdbg.h>
#include <windows.h>
#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif
#define _CRTDBG_MAP_ALLOC

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif
using namespace std;