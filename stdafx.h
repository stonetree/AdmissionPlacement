// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <string>
#include <tchar.h>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <iterator>
#include <cmath>
#include <list>
#include <algorithm>

typedef unsigned int ID;

enum servicetype{LIGHT,MIDDLE,HEAVY};

enum VMtype{MEDIUM,LARGE,XLARGE};

enum eventtype{ARRIVAL,DEPARTURE};

enum requesttype{LOW,HIGHT,XHIGHT,NONE};

using namespace std;

