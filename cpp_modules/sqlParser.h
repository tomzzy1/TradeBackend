#pragma once
#include <string_view>
#include <vector>
#include <iostream>
#include <unordered_set>

class sqlParser
{
public:
	friend class QueryPrice;
	void parse(std::string q)
	{								
		query = q;
		int i = 7; // skip SELECT and a space
		std::string select_field;
		while (!check_FROM(i))
		{
			while (q[i] != ' ' && q[i] != ',')
			{
				select_field += q[i];
				++i;
			}
			if (q[i] == ',')
			{
				select_fields.push_back(select_field);
				std::cout << select_field << '\n';
				select_field.clear();
			}
			if (q[i] == ' ')
			{	
				++i;
			}
		}
		if (!select_field.empty())
			select_fields.push_back(select_field);
		std::cout << "from clause: current position " << i << ' ' << q[i] << '\n';
		i += 5; // skip FROM and a space

		std::string table;
		std::string alias;
		while (true)
		{
			while (i < query.size() && q[i] != ' ' && q[i] != ',')
			{
				table += q[i];
				++i;
			}
			std::cout << i << ' ' << query.size() << '\n';
			if (i == query.size())
			{
				tables.emplace_back(table, alias);
				return;
			}
			if (check_WHERE(i + 1))
			{
				++i;
				std::cout << "break\n";
				break;
			}
			// the alias case
			if (q[i] == ' ' && q[i + 1] != ' ')
			{
				std::cout << "alias\n";
				++i;
				while (q[i] != ' ' && q[i] != ',')
				{
					alias += q[i];
					++i;
				}
				tables.emplace_back(table, alias);
				table.clear();
				alias.clear();
				if (i == query.size())
					return;
				if (check_WHERE(i + 1))
				{
					++i;
					break;
				}
			}
			++i;
			// eat trailing space, expecting ','
			while (q[i] == ' ')
				++i;
			// read next table 
			//++i;
		}
		i += 6; // skip WHERE and a space
		std::cout << "where clause: current position " << i << ' ' << q[i] << '\n';
		std::string attr;
		std::string attr_alias;
		std::string value;
		while (true)
		{
			while (q[i] == ' ')
			{
				++i;
			}
			while (q[i] != ' ' && q[i] != '.')
			{
				attr += q[i];
				++i;
			}
			if (q[i] == '.')
			{
				++i;
				std::swap(attr, attr_alias);
				while (q[i] != ' ')
				{
					attr += q[i];
					++i;
				}
			}
			conditions.emplace_back(attr, attr_alias);
			std::cout << "position: " << i << "push attribute\n";
			attr.clear();
			attr_alias.clear();
			++i; // a space is required
			switch (q[i])
			{
			case '=':
				operations.push_back(0);
				break;
			case '!':
				operations.push_back(1);
				++i;
				break;
			case '>':
				operations.push_back(2);
				break;
			case'<':
				operations.push_back(3);
				break;
			default:
				break;
			}
			i += 2; // skip a space
			while (i < query.size() && q[i] != ' ' && q[i] != ',' && q[i] != '.')
			{
				value += q[i];
				++i;
			}
			if (q[i] == '.')
			{
				++i;
				while (i < query.size() && q[i] != ' ' && q[i] != ',' && q[i] != '.')
				{
					attr += q[i];
					++i;
				}
				values.emplace_back(attr, value);
				attr.clear();
				value.clear();
			}
			else
			{
				values.emplace_back(value, std::string());
			}
			if (i == query.size())
			{

				std::cout << "where end\n";
				break;
			}
			else
			{
				std::cout << "position " << i << "push value\n";
				value.clear();
				++i;
			}
		}

	}
	void print()
	{
		using std::cout;
		cout << "select fields:\n";
		for (auto& s : select_fields)
		{
			cout << s << ' ';
		}
		cout << '\n';
		for (auto& t : tables)
		{
			cout << "table name : " << t.first << " alias " << t.second << '\n';
		}
		for (auto& c : conditions)
		{
			cout << "attribute name : " << c.first << " alias " << c.second << '\n';
		}
		cout << "operations: ";
		for (auto i : operations)
		{
			cout << i << ' ';
		}
		cout << '\n';
		cout << "values: \n";
		for (auto& p : values)
		{
			cout << p.first << ' ' << p.second << '\n';
		}
	}

private:
	bool check_FROM(int i)
	{
		return query.substr(i, 5) == FROM;
	}
	bool check_WHERE(int i)
	{
		if (i + 5 < query.size())
		{
			return query.substr(i, 6) == WHERE;
		}
		return true;
	}
	const std::string FROM = std::string("FROM ", 5);
	const std::string WHERE = std::string("WHERE ", 6);
	std::vector<std::string> select_fields;
	std::vector<std::pair<std::string, std::string>> tables;
	std::vector<std::pair<std::string, std::string>> conditions;
	std::vector<int> operations;
	std::vector<std::pair<std::string, std::string>> values;
	std::string query;
};