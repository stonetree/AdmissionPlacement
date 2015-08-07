// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <tchar.h>
#include <string>
#include <iostream>
#include <iterator>
#include <cmath>
#include <list>
#include <algorithm>
#include <fstream>

typedef unsigned int ID;

enum servicetype{LIGHT,MIDDLE,HEAVY};

enum VMtype{MEDIUM,LARGE,XLARGE};

enum eventtype{ARRIVAL,DEPARTURE};

enum requesttype{LOW,HIGHT,XHIGHT,NONE};

typedef  unsigned int resource_unit;

using namespace std;

