#include "util.h"
#include <sys/time.h>
#include <string.h>
#include <iostream>
using namespace std;



uint64_t get_tick_count()
{
    struct timeval tval;
	uint64_t ret_tick;

	gettimeofday(&tval, NULL);//线程安全的

	ret_tick = tval.tv_sec * 1000L + tval.tv_usec / 1000L;
	return ret_tick;
}

void util_sleep(uint32_t millisecond)
{
    usleep(millisecond * 1000);
}

CStrExplode::CStrExplode(char *str,char seperator)
{
    m_item_cnt = 1;
    char *pos=str;
	while (*pos) {
		if (*pos == seperator) {
			m_item_cnt++;
		}
		pos++;
	}
    m_item_list = new char *[m_item_cnt];

  	int idx = 0;
	char* start = pos = str;
    while (*pos) {
		if ( pos != start && *pos == seperator) {
			uint32_t len = pos - start;
			m_item_list[idx] = new char [len + 1];
			strncpy(m_item_list[idx], start, len);
			m_item_list[idx][len]  = '\0';
			idx++;

			start = pos + 1;
		}

		pos++;
	}

	uint32_t len = pos - start;
    if(len != 0)
    {
        m_item_list[idx] = new char [len + 1];
        strncpy(m_item_list[idx], start, len);
        m_item_list[idx][len]  = '\0';
    }
}


CStrExplode::~CStrExplode()
{
    for (uint32_t i = 0; i < m_item_cnt; i++) {
		delete [] m_item_list[i];
	}

	delete [] m_item_list;
}


