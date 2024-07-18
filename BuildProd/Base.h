#pragma once

#define DBG 1
#define LOG(msg) if(DBG) {log_s += String(msg) + "\n";}

typedef unsigned int uint;
