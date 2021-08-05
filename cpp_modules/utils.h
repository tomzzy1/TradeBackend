#pragma once
#include <vector>
#include <string>

using std::vector;
using std::string;

vector<string> explode(const string& s, char c)                      //作用：分割函数，便于将.tbl文件中的数据按照“|”分割开来，写入到tuple中
{
	string buff{ "" };
	vector<string> v;
    bool in_str = false;
	for (auto n : s)
	{
        if (n == '"')
            in_str = !in_str; 
        if (in_str)
        {
            buff += n;
        }
        else
        {
            if (n != c) 
                buff += n;
		    else if (n == c /*&& buff != ""*/) 
            { v.push_back(buff); buff = ""; }
        }
	}
	//if (buff != "") 
    v.push_back(buff);
	return v;
}

vector<string> split(const string& s, const string& delim)                    //split分割函数
{
	vector<string> elems;
	size_t pos = 0;
	size_t len = s.length();
	size_t delim_len = delim.length();
	if (delim_len == 0) return elems;
	while (pos < len)
	{
		int find_pos = (int)s.find(delim, pos);
		if (find_pos < 0)
		{
			elems.push_back(s.substr(pos, len - pos));
			break;
		}
		elems.push_back(s.substr(pos, find_pos - pos));
		pos = find_pos + delim_len;
	}
	return elems;
}