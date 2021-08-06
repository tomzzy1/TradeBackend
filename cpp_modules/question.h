#pragma once
#include <array>
#include <fstream>
#include "utils.h"

using std::vector;
using std::string;
using std::array;
using std::ifstream;

string join(const vector<string>& strs, string delim)
{
	auto str = strs[0];
	for (auto i = strs.begin() + 1; i < strs.end(); ++i)
	{
		str += delim;
		str += *i;
	}
	return str;
}

auto questionInfo(string ds_name, vector<string>& names, int index) 
{
	string line_buf;
	string default_path = "C:\\Users\\tom zh\\source\\repos\\backend\\query\\";
	vector<string> row_info;
	vector<string> col_info;
	vector<string> pos;
    //int idx = 0;
	for (auto& n: names)
	{
        std::cout << "start\n";
        vector<vector<string>> table;
	    vector<vector<int>> m_state;
		ifstream table_file(default_path + "archive\\" + n + ".csv");
		ifstream missing_file(default_path + ds_name + '_' + std::to_string(index) + std::string(".m"));
        if (!table_file)
            std::cout << "fail to open table file\n";
        if (!table_file)
            std::cout << "fail to open missing file\n";
        //++idx;
		getline(table_file, line_buf);
		auto attrs = split(line_buf, string(",", 1));
        std::cout << "attrs size " << attrs.size() << '\n';
		while (getline(table_file, line_buf))
		{
			table.push_back(explode(line_buf, ','));
			getline(missing_file, line_buf);
			auto row = explode(line_buf, ' ');
			vector<int> num_row;
			for (const auto& s : row)
			{
				num_row.push_back(atoi(s.c_str()));
			}
			m_state.push_back(num_row);
            if (table.back().size() != m_state.back().size())
            {
                std::cout << "error!!!\n" << table.size() << ' ' << m_state.size() << ' ' <<
                table.back().size() << ' ' << m_state.back().size();
                exit(-1);
            }
		}
        std::cout << m_state.size() << ' ' << table.size() <<' ' << m_state[0].size() << ' ' << table[0].size() << '\n';
		for (int i = 0; i < m_state.size(); ++i)
		{
            vector<string> row_vec;
			string row_str;
			for (int j = 0; j < m_state[0].size(); ++j)
			{
                vector<string> col_vec;
                //if (idx > 1)
                //    std::cout << i << ' ' << j << '\n';
				if (m_state[i][j] == 0)
				{
					if (row_vec.empty())
					{
						for (int k = 0; k < table[0].size(); ++k)
						{
							if (m_state[i][k] == 1)
							{
								row_vec.push_back(attrs[k] + ": " + table[i][k]);
							}
							else
							{
								row_vec.push_back(attrs[k] + ": undefined");
							}
						}
						row_str = join(row_vec, ", ");
					}
					row_info.push_back(row_str);
					for (int k = max(0, i - 2); k < i; ++k)
					{
						if (m_state[k][j] == 1)
						{
							col_vec.push_back(attrs[j] + ": " + table[k][j]);
						}
						else
						{
							col_vec.push_back(attrs[j] + ": undefined");
						}
					}
					for (int k = i + 1; 
						k < min(i + 3, static_cast<int>(table.size())); ++k)
					{
						if (m_state[k][j] == 0)
						{
							col_vec.push_back(attrs[j] + ": " + table[k][j]);
						}
						else
						{
							col_vec.push_back(attrs[j] + ": undefined");
						}
					}
					col_info.push_back(join(col_vec, ", "));
					pos.push_back(attrs[j]);
				}
			}
		}
        table_file.close();
        missing_file.close();
        std::cout << "finished 1 file\n";
	}
    std::cout << "finished\n";
	return array<vector<string>, 3>{pos, row_info, col_info};
}