#define _CRT_SECURE_NO_WARNINGS  
#include "pricing.h"
#include <fstream>
#include "math.h"
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <ctime>
#include <iostream>
#include <algorithm>
#include "sqlParser.h"

using namespace std;

#define MAX_DIM 200
#define RAND_MAX 0x7FFF

class QueryPrice
{

public:
	float g_price = 0;
	float g_alpha = 0.5;
	float g_a = 0.5;
	float g_b0 = 1;
	int type = -1;
	string datatype = " ";
	int g_exact_price = 1;
	float g_missing_rate = 0.2;
	int g_label = 1;
	int datasize = 0;
	int g_viewNumber = 17; //表的个数
	//int g_queryNumber = 10;// for history-aware pricing problem
	int g_queryNumber = 1;
	float g_quality = 1;
	int g_qualityNumber = 1;
	vector<Tuple> token;
	float history_price = 0;
	float no_history_price = 0;
	float history_obvilious_price = 0;
	vector<Query> HQ;

	vector<string> paths;
	vector<string> names;
	vector<int> rowNumber;

	vector<int> dim; //所有表的属性个数
	Query Q; //查询
	View T; // 查询结果

	vector<Attribute> DA; //所有表的属性
	vector<View> D; //整个数据集


	//vector<vector<double> > MR; //各个tuple的缺失率
	//double P = -1; //最终价格

	//unordered_map<int, double> PD; //每个质量对应的价格


	Attribute A; // 结果属性
	unordered_map<int, int> table; //在读取了多个表的情况下进行映射。

	sqlParser parser;


	//vector<vector<double>>tuple_price;//不停的更新
	//vector<int>dis;//0 1表

	unordered_set<string> attrSet;
	vector<int> selectIndex;
	vector<int> projectionIndex;
	//unordered_set<string> S;
	//int newu = 0;
	unordered_set<string> nullSet; //查找起来比较方便的容器

	string default_path = "C:\\Users\\tom zh\\source\\repos\\backend\\query\\";
	ofstream output;

	void countRows()
	{
		for (const auto& p : paths)
		{
			auto file = ifstream(p);
			if (!file)
			{
				std::cout << p << " fail to open!!!" << '\n';
			}
			string buffer;
			int i = 0;
			while (getline(file, buffer))
				++i;
			rowNumber.push_back(i - 1);
			file.close();
		}
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
	void output_result(string s)                     //作用：将输出的字符串信息写入到output.txt中
	{
		output << s << endl;
	}
	void output_number(double s)                     //作用：将输出的数字型信息写入到output.txt中
	{
		output << s << endl;
		//output.close();
	}
	/*注意：query.txt中语句的格式是：
	第一行为join的表的数量
	第二行为join的各个表的标号
	第三行为两两join时的属性名
	第四行为select的表标号（若为-1则使用操作后的结果）和select的条件数
	第五行为select的属性名
	第六行为select各属性对应的值
	第七行为projection表标号（若为-1则使用操作后的结果）和projection的维数
	第八行为projection的属性名*/

	void readin_query(string queryPath) {
		// 作用：读入查询语句到Q中，到此Q形成，查询的信息在query.txt中
		//std::cout <<"Entering readin_query." << endl;
		ifstream queryin(queryPath);

		if (!queryin) {
			exit(0);
		}



		//output_result("***************************************");
		output_result("Query Information: ");
		//int type = -1;
		queryin >> datatype; // dataset name
		output_result(datatype);
		std::cout << datatype << endl;

		queryin >> type; //S=0, P =1, J =2, SPJ =3;
		if (type == 2)
		{
			std::cout << "This is a Join query: " << endl;
			queryin >> Q.joinNumber; //第一行是join表的个数
			string joinNumber;
			getline(queryin, joinNumber);
			if (Q.joinNumber != 0)
			{
				output_result("Number of Tables in Join: ");
				char c[8];
				output_result(_itoa(Q.joinNumber, c, 10));
				std::cout << "Number of Tables in Join: " << Q.joinNumber << endl;
			}
			for (int i = 0; i < Q.joinNumber; i++) {
				int t;
				queryin >> t;        //第二行是join时各个表的编号
				Q.joinIndex.push_back(t);
				output_result("Table Index in Join:");
				char c1[8];
				output_result(_itoa(t, c1, 10));
				std::cout << "Table's Index in Join: " << t << endl;
			}

			for (int i = 1; i < Q.joinNumber; i++) {
				string attr;
				queryin >> attr;
				output_result("Foreign Keys:");
				Q.joinAttr.push_back(attr);//第三行是join时各个表的属性名
				output_result(attr);
			}
			queryin >> Q.selectNumber;//select的条件数
			output_result("#Constraints in Selection:");
			char c3[8];
			output_result(_itoa(Q.selectNumber, c3, 10));
			std::cout << "#Constraints in Selection: " << Q.selectNumber << endl;
			for (int i = 0; i < Q.selectNumber; i++) {
				string attr;
				queryin >> attr;
				Q.selectAttr.push_back(attr);// select的属性名
				output_result("Critical attributes in Selection:");
				output_result(attr);
			}
			for (int i = 0; i < Q.selectNumber; i++) {
				string attr;
				//getline(queryin, attr);
				queryin >> attr;         // select时各属性名对应的值
				Q.selecctValue.push_back(attr);
				output_result("Values on critical attributes in Selection:");
				output_result(attr);
			}
			for (int i = 0; i < Q.selectNumber; i++) {
				int t;
				queryin >> t;
				Q.selectOperation.push_back(t);
				output_result("Select operation: ");
				char c1[8];
				output_result(_itoa(t, c1, 10));
				std::cout << "Select operation: " << t << endl;
			}
		}
		else if (type == 0)
		{
			std::cout << "This is a Selection query: " << endl;
			queryin >> Q.selectP; // select的表编号
			output_result("Table Index in Selection:");
			char c2[8];
			output_result(_itoa(Q.selectP, c2, 10));
			std::cout << "Table's Index in Selection: " << Q.selectP << endl;



			queryin >> Q.selectNumber;//select的条件数
			output_result("#Constraints in Selection:");
			char c3[8];
			output_result(_itoa(Q.selectNumber, c3, 10));
			std::cout << "#Constraints in Selection: " << Q.selectNumber << endl;
			for (int i = 0; i < Q.selectNumber; i++) {
				string attr;
				queryin >> attr;
				Q.selectAttr.push_back(attr);// select的属性名
				output_result("Critical attributes in Selection:");
				output_result(attr);
			}
			for (int i = 0; i < Q.selectNumber; i++) {
				string attr;
				queryin >> attr;         // select时各属性名对应的值
				Q.selecctValue.push_back(attr);
				output_result("Values on critical attributes in Selection:");
				output_result(attr);
			}
			for (int i = 0; i < Q.selectNumber; i++) {
				int t;
				queryin >> t;
				Q.selectOperation.push_back(t);
				output_result("Select operation: ");
				char c1[8];
				output_result(_itoa(t, c1, 10));
				std::cout << "Select operation: " << t << endl;
			}
		}
		else if (type == 1)
		{
			std::cout << "This is a Projection query: " << endl;
			queryin >> Q.projectionP;   //第七行是投影的表编号
			output_result("Table index in Projection:");
			char c4[8];
			output_result(_itoa(Q.projectionP, c4, 10));
			queryin >> Q.projectionNumber;//投影的维度数
			output_result("#Critical attributes in Projection:");
			char c5[8];
			output_result(_itoa(Q.projectionNumber, c5, 10));
			std::cout << "Table index in Projection:" << Q.projectionP << endl;
			std::cout << "#Critical attributes  in Projection:" << Q.projectionNumber << endl;


			for (int i = 0; i < Q.projectionNumber; i++) {
				string attr;
				queryin >> attr;    //第八行是投影的属性名
				Q.projectionAttr.push_back(attr);
				output_result("Critical attributes in Projection:");
				output_result(attr);
			}

		}
		else if (type == 3)// SPJ query 
		{
			std::cout << "This is a SPJ query: " << endl;
			queryin >> Q.joinNumber; //第一行是join表的个数
			string joinNumber;
			getline(queryin, joinNumber);
			if (Q.joinNumber != 0)
			{
				output_result("Number of Tables in Join: ");
				char c[8];
				output_result(_itoa(Q.joinNumber, c, 10));
				std::cout << "Number of Tables in Join: " << Q.joinNumber << endl;
			}
			for (int i = 0; i < Q.joinNumber; i++) {
				int t;
				queryin >> t;        //第二行是join时各个表的编号
				Q.joinIndex.push_back(t);
				output_result("Table Index in Join:");
				char c1[8];
				output_result(_itoa(t, c1, 10));
				std::cout << "Table's Index in Join: " << Q.joinNumber << endl;
			}

			for (int i = 0; i < Q.joinNumber; i++) {
				string attr;
				queryin >> attr;
				output_result("Foreign Keys:");
				Q.joinAttr.push_back(attr);//第三行是join时各个表的属性名
				output_result(attr);
			}


			queryin >> Q.selectP; //第四行是select的表编号
			output_result("Table Index in Selection:");
			char c2[8];
			output_result(_itoa(Q.selectP, c2, 10));
			std::cout << "Table's Index in Selection: " << Q.selectP << endl;



			queryin >> Q.selectNumber;//select的条件数
			output_result("#Constraints in Selection:");
			char c3[8];
			output_result(_itoa(Q.selectNumber, c3, 10));
			std::cout << "#Constraints in Selection: " << Q.selectNumber << endl;
			for (int i = 0; i < Q.selectNumber; i++) {
				string attr;
				queryin >> attr;
				Q.selectAttr.push_back(attr);//第五行是select的属性名
				output_result("Critical attributes in Selection:");
				output_result(attr);
			}
			for (int i = 0; i < Q.selectNumber; i++) {
				string attr;
				queryin >> attr;         //第六行是select时各属性名对应的值
				Q.selecctValue.push_back(attr);
				output_result("Values on critical attributes in Selection:");
				output_result(attr);
			}


			queryin >> Q.projectionP;   //第七行是投影的表编号
			output_result("Table index in Projection:");
			char c4[8];
			output_result(_itoa(Q.projectionP, c4, 10));
			queryin >> Q.projectionNumber;//投影的维度数
			output_result("#Critical attributes in Projection:");
			char c5[8];
			output_result(_itoa(Q.projectionNumber, c5, 10));
			std::cout << "Table index in Projection:" << Q.projectionP << endl;
			std::cout << "#Critical attributes  in Projection:" + Q.projectionNumber << endl;


			for (int i = 0; i < Q.projectionNumber; i++) {
				string attr;
				queryin >> attr;    //第八行是投影的属性名
				Q.projectionAttr.push_back(attr);
				output_result("Critical attributes in Projection:");
				output_result(attr);
			}
		}
		output_result("***************************************");
		queryin.close();
		std::cout << "Have read the query.." << endl;
	}


	/*void readin_history_query(string queryPath) {
		ifstream queryin(queryPath);
		if (!queryin) {
			exit(0);
		}
		std::cout << "The number of queries for history-aware pricing: " << g_queryNumber << endl;

		//output_result("Query Information: ");
		//int type = -1;
		queryin >> datatype; // dataset name
		output_result(datatype);
		std::cout << datatype << endl;
		HQ.resize(g_queryNumber);
		for (int r = 0; r < g_queryNumber; r++)
		{
			std::cout << "Query --" << r << "-- is as follows. " << endl;

			queryin >> type; //S=0, P =1, J =2, SPJ =3;
			HQ[r].queryType = type;
			HQ[r].joinNumber = 0;
			if (type == 2)
			{
				std::cout << "This is a Join query: " << endl;
				queryin >> HQ[r].joinNumber; //第一行是join表的个数
				string joinNumber;
				getline(queryin, joinNumber);
				if (HQ[r].joinNumber != 0)
				{
					output_result("Number of Tables in Join: ");
					char c[8];
					output_result(_itoa(HQ[r].joinNumber, c, 10));
					std::cout << "Number of Tables in Join: " << HQ[r].joinNumber << endl;
				}
				for (int i = 0; i < HQ[r].joinNumber; i++) {
					int t;
					queryin >> t;        //第二行是join时各个表的编号
					HQ[r].joinIndex.push_back(t);
					output_result("Table Index in Join:");
					char c1[8];
					output_result(_itoa(t, c1, 10));
					std::cout << "Table's Index in Join: " << t << endl;
				}

				for (int i = 1; i < HQ[r].joinNumber; i++) {
					string attr;
					queryin >> attr;
					output_result("Foreign Keys:");
					HQ[r].joinAttr.push_back(attr);//第三行是join时各个表的属性名
					output_result(attr);
				}
				queryin >> HQ[r].selectNumber;//select的条件数
				output_result("#Constraints in Selection:");
				char c3[8];
				output_result(_itoa(HQ[r].selectNumber, c3, 10));
				std::cout << "#Constraints in Selection: " << HQ[r].selectNumber << endl;
				if (HQ[r].selectNumber != 0)
				{
					for (int i = 0; i < HQ[r].selectNumber; i++) {
						string attr;
						queryin >> attr;
						HQ[r].selectAttr.push_back(attr);// select的属性名
						output_result("Critical attributes in Selection:");
						output_result(attr);
					}
					for (int i = 0; i < HQ[r].selectNumber; i++) {
						string attr;
						//getline(queryin, attr);
						queryin >> attr;         // select时各属性名对应的值
						HQ[r].selecctValue.push_back(attr);
						output_result("Values on critical attributes in Selection:");
						output_result(attr);
					}
					for (int i = 0; i < HQ[r].selectNumber; i++) {
						int t;
						queryin >> t;
						HQ[r].selectOperation.push_back(t);
						output_result("Select operation: ");
						char c1[8];
						output_result(_itoa(t, c1, 10));
						std::cout << "Select operation: " << t << endl;
					}

				}

			}
			else if (type == 0)
			{
				std::cout << "This is a Selection query: " << endl;
				queryin >> HQ[r].selectP; // select的表编号
				output_result("Table Index in Selection:");
				char c2[8];
				output_result(_itoa(HQ[r].selectP, c2, 10));
				std::cout << "Table's Index in Selection: " << HQ[r].selectP << endl;



				queryin >> HQ[r].selectNumber;//select的条件数
				output_result("#Constraints in Selection:");
				char c3[8];
				output_result(_itoa(HQ[r].selectNumber, c3, 10));
				std::cout << "#Constraints in Selection: " << Q.selectNumber << endl;
				for (int i = 0; i < HQ[r].selectNumber; i++) {
					string attr;
					queryin >> attr;
					HQ[r].selectAttr.push_back(attr);// select的属性名
					output_result("Critical attributes in Selection:");
					output_result(attr);
				}
				for (int i = 0; i < HQ[r].selectNumber; i++) {
					string attr;
					queryin >> attr;         // select时各属性名对应的值
					HQ[r].selecctValue.push_back(attr);
					output_result("Values on critical attributes in Selection:");
					output_result(attr);
				}
				for (int i = 0; i < HQ[r].selectNumber; i++) {
					int t;
					queryin >> t;
					HQ[r].selectOperation.push_back(t);
					output_result("Select operation: ");
					char c1[8];
					output_result(_itoa(t, c1, 10));
					std::cout << "Select operation: " << t << endl;
				}
			}
			else if (type == 1)
			{
				std::cout << "This is a Projection query: " << endl;
				queryin >> HQ[r].projectionP;   //第七行是投影的表编号
				output_result("Table index in Projection:");
				char c4[8];
				output_result(_itoa(HQ[r].projectionP, c4, 10));
				queryin >> HQ[r].projectionNumber;//投影的维度数
				output_result("#Critical attributes in Projection:");
				char c5[8];
				output_result(_itoa(HQ[r].projectionNumber, c5, 10));
				std::cout << "Table index in Projection:" << HQ[r].projectionP << endl;
				std::cout << "#Critical attributes  in Projection:" << HQ[r].projectionNumber << endl;


				for (int i = 0; i < HQ[r].projectionNumber; i++) {
					string attr;
					queryin >> attr;    //第八行是投影的属性名
					HQ[r].projectionAttr.push_back(attr);
					output_result("Critical attributes in Projection:");
					output_result(attr);
				}

			}
		}
		output_result("***************************************");
		queryin.close();
		std::cout << "Have read all the queries.." << endl;
	}*/

	void readin_history_query() {
		/*ifstream queryin(queryPath);
		if (!queryin) {
			exit(0);
		}*/
		std::cout << "The number of queries for history-aware pricing: " << g_queryNumber << endl;

		//output_result("Query Information: ");
		//int type = -1;
		//queryin >> datatype; // dataset name
		//datatype = queryPath;
		//output_result(datatype);
		//std::cout << datatype << endl;
		HQ.resize(1);
		//for (int r = 0; r < g_queryNumber; r++)
		//{
		std::cout << "Query --" << 1 << "-- is as follows. " << endl;

		type = 1;
		
		if (parser.tables.size() > 1)
			type = 2;
		else if (parser.operations.size() > 1)
			type = 0;
		std::cout << "table size " << parser.tables.size() << "type " << type << '\n';
		//queryin >> type; //S=0, P =1, J =2, SPJ =3;
		HQ[0].queryType = type;
		HQ[0].joinNumber = 0;
		if (type == 2)
		{
			std::cout << "This is a Join query: " << endl;
			HQ[0].joinNumber = parser.tables.size(); //第一行是join表的个数
			if (HQ[0].joinNumber != 0)
			{
				output_result("Number of Tables in Join: ");
				char c[8];
				output_result(_itoa(HQ[0].joinNumber, c, 10));
				std::cout << "Number of Tables in Join: " << HQ[0].joinNumber << endl;
			}
			for (int i = 0; i < HQ[0].joinNumber; i++) {
				int t = find(names.begin(), names.end(), parser.tables[i].first) - names.begin();
				//第二行是join时各个表的编号
				HQ[0].joinIndex.push_back(t);
				output_result("Table Index in Join:");
				char c1[8];
				output_result(_itoa(t, c1, 10));
				std::cout << "Table's Index in Join: " << t << endl;
			}

			/*for (int i = 1; i < HQ[0].joinNumber; i++) {
				string attr;
				queryin >> attr;
				output_result("Foreign Keys:");
				HQ[0].joinAttr.push_back(attr);//第三行是join时各个表的属性名
				output_result(attr);
			}*/
			HQ[0].joinAttr.push_back(parser.values[0].first);

			//queryin >> HQ[r].selectNumber;//select的条件数
			HQ[0].selectNumber = parser.conditions.size() - 1;
			output_result("#Constraints in Selection:");
			char c3[8];
			output_result(_itoa(HQ[0].selectNumber, c3, 10));
			std::cout << "#Constraints in Selection: " << HQ[0].selectNumber << endl;
			if (HQ[0].selectNumber != 0)
			{
				for (int i = 0; i < HQ[0].selectNumber; i++) {
					string attr = parser.conditions[i].first;
					//queryin >> attr;
					HQ[0].selectAttr.push_back(attr);// select的属性名
					output_result("Critical attributes in Selection:");
					output_result(attr);
				}
				for (int i = 0; i < HQ[0].selectNumber; i++) {
					string attr = parser.values[i].second;
					//getline(queryin, attr);
					//queryin >> attr         // select时各属性名对应的值
					HQ[0].selecctValue.push_back(attr);
					output_result("Values on critical attributes in Selection:");
					output_result(attr);
				}
				for (int i = 0; i < HQ[0].selectNumber; i++) {
					int t = parser.operations[i];
					//queryin >> t;
					HQ[0].selectOperation.push_back(t);
					output_result("Select operation: ");
					char c1[8];
					output_result(_itoa(t, c1, 10));
					std::cout << "Select operation: " << t << endl;
				}

			}

		}
		else if (type == 0)
		{
			std::cout << "This is a Selection query: " << endl;
			//queryin >> HQ[r].selectP; // select的表编号
			int t = find(names.begin(), names.end(), parser.tables[0].first) - names.begin();
			HQ[0].selectP = t;
			output_result("Table Index in Selection:");
			char c2[8];
			output_result(_itoa(HQ[0].selectP, c2, 10));
			std::cout << "Table's Index in Selection: " << HQ[0].selectP << endl;



			HQ[0].selectNumber = parser.conditions.size();
			output_result("#Constraints in Selection:");
			char c3[8];
			output_result(_itoa(HQ[0].selectNumber, c3, 10));
			std::cout << "#Constraints in Selection: " << HQ[0].selectNumber << endl;
			if (HQ[0].selectNumber != 0)
			{
				for (int i = 0; i < HQ[0].selectNumber; i++) {
					string attr = parser.conditions[i].first;
					//queryin >> attr;
					HQ[0].selectAttr.push_back(attr);// select的属性名
					output_result("Critical attributes in Selection:");
					output_result(attr);
				}
				for (int i = 0; i < HQ[0].selectNumber; i++) {
					string attr = parser.values[i].second;
					//getline(queryin, attr);
					//queryin >> attr         // select时各属性名对应的值
					HQ[0].selecctValue.push_back(attr);
					output_result("Values on critical attributes in Selection:");
					output_result(attr);
				}
				for (int i = 0; i < HQ[0].selectNumber; i++) {
					int t = parser.operations[i];
					//queryin >> t;
					HQ[0].selectOperation.push_back(t);
					output_result("Select operation: ");
					char c1[8];
					output_result(_itoa(t, c1, 10));
					std::cout << "Select operation: " << t << endl;
				}

			}
		}
		else if (type == 1)
		{
			std::cout << "This is a Projection query: " << endl;
			//queryin >> HQ[r].projectionP;   //第七行是投影的表编号
			int t = find(names.begin(), names.end(), parser.tables[0].first) - names.begin();
			HQ[0].projectionP = t;
			output_result("Table index in Projection:");
			char c4[8];
			output_result(_itoa(HQ[0].projectionP, c4, 10));
			//queryin >> HQ[0].projectionNumber;//投影的维度数
			HQ[0].projectionNumber = parser.select_fields.size();
			output_result("#Critical attributes in Projection:");
			char c5[8];
			output_result(_itoa(HQ[0].projectionNumber, c5, 10));
			std::cout << "Table index in Projection:" << HQ[0].projectionP << endl;
			std::cout << "#Critical attributes  in Projection:" << HQ[0].projectionNumber << endl;

			for (int i = 0; i < HQ[0].projectionNumber; i++) {
				string attr = parser.select_fields[i];
				//queryin >> attr;    //第八行是投影的属性名
				HQ[0].projectionAttr.push_back(parser.select_fields[i]);
				output_result("Critical attributes in Projection:");
				output_result(attr);
			}

		}
		//}
		output_result("***************************************");
		//queryin.close();
		std::cout << "Have read all the queries.." << endl;
	}


	vector<int> convert_str2int_vec(const vector<string>& str_vec)
	{
		vector<int> ret_vec;
		ret_vec.reserve(str_vec.size());
		for (int i = 0; i < str_vec.size(); i++) {
			ret_vec.push_back(atoi(str_vec[i].c_str()));
		}
		return ret_vec;
	}

	vector<string> explode(const string& s, char c)                      //作用：分割函数，便于将.tbl文件中的数据按照“|”分割开来，写入到tuple中
	{
		string buff{ "" };
		vector<string> v;

		for (auto n : s)
		{
			if (n != c) buff += n;
			else if (n == c && buff != "") { v.push_back(buff); buff = ""; }
		}
		if (buff != "") v.push_back(buff);

		return v;
	}

	/*void init_price()                            //作用：初始化各个元组的价格，便于历史价格的更新；第一个for循环的10代表有10个表
	{

	for (int i = 0; i<10; i++)
	{
	ifstream input("D:\\" + to_string(i) + ".tbl");
	string line;
	int tupleNumber = 0;
	while (getline(input, line)) {
	tupleNumber++;
	}
	vector<double> temp;  ///定义一个一维的  也就是 二维的元素类型，二维可以看做 一维的一维数组。
	for (int j = 0; j < tupleNumber; j++)
	{
	temp.push_back(pr[i]);
	}
	tuple_price.push_back(temp);
	///将一维的压入二维中

	}

	}*/


	void readin_helper(int x, View& table) {	//作用：读单个表 ，x表示表号，将.tbl文件中的数据读入到内存中
		//View table; //读入的表
		// read the table
		table.tupleNumber = 0;
		table.tuple.clear();
		table.tuple.resize(rowNumber[x]);
		ifstream inputFile(paths[x]); // 读入文件
		ifstream labelFile(default_path + std::to_string(x) + std::string(".l")); // 读入文件
		ifstream missingFile(default_path + std::to_string(x) + std::string(".m")); // 读入文件
		if (!inputFile)
			cout << "open input file failed!\n";
		if (!labelFile)
			cout << "open label file failed!\n";
		if (!missingFile)
			cout << "open missing file failed!\n";
		int tupleNumber = 0;
		string line, lineL, lineM;
		vector<string> row;
		getline(inputFile, line); // skip the headers
		while (getline(inputFile, line)) {
			vector<string> row{ explode(line, ',') };
			table.tuple[tupleNumber].Values = row;
			table.tuple[tupleNumber].x = 0;
			/*for (int i = 0; i < row.size(); i++)
			{
			table.tuple[tupleNumber].Values[i].push_back(row[i]);
			}*/
			// = row;//.push_back(row); //添加tuple到表
			if (getline(labelFile, lineL))
			{
				//ector<string> row{ explode(lineL, '|') };
				table.tuple[tupleNumber].label = atoi(lineL.c_str());//need to transfer the data type
			}
			// read the missing information, record the completeNum
			if (getline(missingFile, lineM))
			{
				vector<int> row{ convert_str2int_vec(explode(lineM, ' ')) };// here need a data type transfer....
				table.tuple[tupleNumber].mState = row;
				/*
				for (vector<int>:: iterator iter = row.begin(); iter != row.end(); ++iter)
				{
				if (*iter == 1)
				{
				tup.completeNum++;
				}
				}*/
			}
			// TODO : 需要确认table.L的初始化情况
			//table.L.push_back(to_string(x) + '.' + to_string(tupleNumber) + ';'); //当前lineage（即元组自己）
			//table.u.push_back("0;");
			tupleNumber++;
			/*if (tupleNumber % 10000 == 0) {
				std::cout << "read in " << tupleNumber << "tuples" << endl;
			}*/
		}

		table.tupleNumber = tupleNumber;

		inputFile.close();
		labelFile.close();
		missingFile.close();
	}

	void calcSelectIndex() {
		for (int i = 0; i < Q.selectNumber; i++)
			for (int j = 0; j < A.attrNumber; j++)
				if (Q.selectAttr[i] == A.attrName[j])
					selectIndex.push_back(j); //确定select属性的列号
	}

	bool predicate(View& T, int i) {
		int v = 0;
		if (Q.selectNumber == 0)
		{
			return true;
		}
		for (int j = 0; j < Q.selectNumber; j++) {
			int pos = selectIndex[j];
			if (T.tuple[i].mState[pos] == 0) {
				//if (attrSet.find(Q.selectAttr[j]) == attrSet.end())
				//	v++;
				T.tuple[i].x++;
				//continue;
				return false;
			}
			if (Q.selectOperation[j] == 0) // require being equal
			{
				if (T.tuple[i].Values[pos] != Q.selecctValue[j])//maybe should be more complex constraint, not just the equality
					return false;
			}
			else if (Q.selectOperation[j] == 1)// require being not equal
			{
				if (T.tuple[i].Values[pos] == Q.selecctValue[j])//maybe should be more complex constraint, not just the equality
					return false;
			}
			else if (Q.selectOperation[j] == 2) // require being larger than
			{
				string t = T.tuple[i].Values[pos];
				string q = Q.selecctValue[j];
				if (atof(t.c_str()) <= atof(q.c_str()))//maybe should be more complex constraint, not just the equality
					return false;
			}
			else if (Q.selectOperation[j] == 3)// require being smaller than
			{
				string t = T.tuple[i].Values[pos];
				string q = Q.selecctValue[j];
				if (atof(t.c_str()) >= atof(q.c_str()))//maybe should be more complex constraint, not just the equality
					return false;
			}
		}
		//u.pop_back();
		//int tmp = stoi(u) + v;
		//u = to_string(tmp) + ';';
		return true;
	}

	void join(View& T1, View& T2, int pos1, int pos2) {                 //作用：表的连接操作，更新结果表的同时也要更新DA中的属性
		T.tupleNumber = 0;
		//g_quality = 0;
		//g_qualityNumber = 0;
		if (Q.selectNumber > 0)
		{
			calcSelectIndex();//确定select属性的列号
			for (int i = 0; i < T1.tupleNumber; i++) {

				if (T1.tuple[i].mState[pos1] == 0)  //连接第一个表的外键为空
				{
					T1.tuple[i].x++;//the number of missing critical attributes
					//std::cout << "mstate is zero. " << endl;
					predicate(T1, i);
					g_quality += pow(g_a, T1.tuple[i].x);
					g_qualityNumber++;
					continue;
				}

				else if (predicate(T1, i)) {
					for (int j = 0; j < T2.tupleNumber; j++) {
						if (T2.tuple[j].mState[pos2] == 0)
						{
							if (T2.tuple[j].x == 0)
							{
								T2.tuple[j].x++;
								g_quality += pow(g_a, T2.tuple[j].x);
								g_qualityNumber++;
							}
							continue;
						}
						else if (T1.tuple[i].Values[pos1] == T2.tuple[j].Values[pos2])
						{
							//std::cout << "At join of two tuples.." << endl;
							Tuple tup = T1.tuple[i];
							//std::cout << "T2 value" << T2.tuple[j].Values.size() << endl;
							for (int k = 0; k < T2.tuple[j].Values.size(); k++) //循环T2的值
							{
								if (k != pos2) //若非外键值
								{
									tup.Values.push_back(T2.tuple[j].Values[k]);
									tup.mState.push_back(T2.tuple[j].mState[k]);
									//newT.tuple[newT.tupleNumber-1].Values.push_back(T2.tuple[j].Values[k]); //将T2每一列的值加入到新的tuple中
									//newT.tuple[newT.tupleNumber - 1].mState.push_back(T2.tuple[j].mState[k]); //record the missing information
									//std::cout << "size() = "<< tup.Values.size() << " k = " << k << endl;
								}

							}

							vector<Tuple> lset;
							lset.push_back(T2.tuple[j]);
							lset.push_back(T1.tuple[i]);
							tup.L.push_back(lset);
							T.tuple.push_back(tup);
							T.tupleNumber++;
						}// end of join operation
					}//end of T2 loop
				}// if predicate
				else {
					if (T1.tuple[i].x > 0)
					{
						g_quality += pow(g_a, T1.tuple[i].x);
						g_qualityNumber++;
					}
				}//end of if predicate

			}// end of T1 loop
		}// end of Q.selectNum>0
		else
		{
			for (int i = 0; i < T1.tupleNumber; i++) {
				if (T1.tuple[i].mState[pos1] == 0)  //连接第一个表的外键为空
				{
					T1.tuple[i].x++;//the number of missing critical attributes
					//std::cout << "mstate is zero. " << endl;
					//predicate(T1, i);
					g_quality += pow(g_a, T1.tuple[i].x);
					g_qualityNumber++;
					continue;
				}
				else {
					for (int j = 0; j < T2.tupleNumber; j++) {
						if (T2.tuple[j].mState[pos2] == 0)
						{
							if (T2.tuple[j].x == 0)
							{
								T2.tuple[j].x++;
								g_quality += pow(g_a, T2.tuple[j].x);
								g_qualityNumber++;
							}
							continue;
						}
						else if (T1.tuple[i].Values[pos1] == T2.tuple[j].Values[pos2])
						{
							//std::cout << "At join of two tuples.." << endl;
							Tuple tup = T1.tuple[i];
							for (int k = 0; k < T2.tuple[j].Values.size(); k++) //循环T2的值
							{
								if (k != pos2) //若非外键值
								{
									tup.Values.push_back(T2.tuple[j].Values[k]);
									tup.mState.push_back(T2.tuple[j].mState[k]);
									//newT.tuple[newT.tupleNumber-1].Values.push_back(T2.tuple[j].Values[k]); //将T2每一列的值加入到新的tuple中
									//newT.tuple[newT.tupleNumber - 1].mState.push_back(T2.tuple[j].mState[k]); //record the missing information
								}

							}

							vector<Tuple> lset;
							lset.push_back(T2.tuple[j]);
							lset.push_back(T1.tuple[i]);
							tup.L.push_back(lset);
							T.tuple.push_back(tup);
							T.tupleNumber++;
						}
					}// end of join operation
				}// end of mstate =1
			}// end of T1 loop

		}
		// TODO : 这里注释掉的代码需要完善
		//newT.L
		//string s = T1.L[i];
		//s.pop_back();
		//newT.L.push_back(s + ',' + T2.L[j]); //更新结果的lineage
		/*string s1 = T1.u[i];
		s1.pop_back();
		int v = stoi(s1);
		if (T2.tuple[j][pos2] == "null")
		v++;
		newT.u.push_back(to_string(v) + ';'); //更新结果对应的质量
		*/




		//T = newT;
		//T.tupleNumber = newT.tupleNumber;
	}


	void select_eager() {
		View newT;
		newT.tupleNumber = 0;
		std::cout << "Q.joinNum == " << Q.joinNumber << endl;
		if (Q.joinNumber > 0)
			for (int i = 0; i < Q.joinNumber - 1; i++)
				attrSet.insert(Q.joinAttr[i]);
		calcSelectIndex();//确定select属性的列号
		for (int i = 0; i < T.tupleNumber; i++) {
			if (predicate(T, i)) {

				Tuple tup;
				vector<Tuple> lset;
				lset.push_back(T.tuple[i]);
				tup = T.tuple[i];
				tup.L.push_back(lset);
				newT.tuple.push_back(tup);
				//int id_t = newT.tuple.size();
				//int id_l = newT.tuple[id_t].L.size();
				//newT.tuple[id_t].L[id_l].push_back(T.tuple[i]);
				newT.tupleNumber++;
			}
			else if (T.tuple[i].x > 0)
			{
				g_quality += pow(g_a, T.tuple[i].x);
				g_qualityNumber++;
			}
		}
		T = newT;
		T.tupleNumber = newT.tupleNumber;
	}

	//vector<int> projectionIndex;

	void projection_eager() {
		View newT;
		newT.tupleNumber = 0;
		unordered_map<string, int> hash;
		for (int i = 0; i < Q.projectionNumber; i++)
			for (int j = 0; j < A.attrNumber; j++)
				if (Q.projectionAttr[i] == A.attrName[j])
					projectionIndex.push_back(j); //确定映射列号

		for (int i = 0; i < T.tupleNumber; i++) {
			string s = "";
			for (int j = 0; j < Q.projectionNumber; j++) {// consider the missing values
				if (T.tuple[i].mState[projectionIndex[j]] = 1)
				{
					s += T.tuple[i].Values[projectionIndex[j]];// make the projection, and  get the result s
				}
				else
				{
					T.tuple[i].x++;
					s += "null";// here, you should be considered whether it is right.
				}

			}
			if (T.tuple[i].x > 0)
			{
				g_quality += pow(g_a, T.tuple[i].x);
				g_qualityNumber++;
			}
			if (hash.find(s) == hash.end()) {// does not exist in hash
				Tuple t;
				for (int j = 0; j < Q.projectionNumber; j++)
				{
					t.Values.push_back(T.tuple[i].Values[projectionIndex[j]]);
					//t.label = T.tuple[i].label;

					t.mState.push_back(T.tuple[i].mState[j]);

				}
				//Tuple tup;
				vector<Tuple> lset;

				lset.push_back(T.tuple[i]);
				t.L.push_back(lset);

				newT.tuple.push_back(t); // the result table should only have the projected attributes.. But it doesn't matter here.
				hash[s] = newT.tupleNumber;
				//newT.tuple[hash[s]] = T.tuple[i];
				newT.tupleNumber++;
				// TODO : 这里被注释掉了
				//newT.L.push_back(T.L[i]);
				//newT.u.push_back(T.u[i]);
			}
			else { //there exists the projected tuple..
				// TODO: 这里被注释掉了
				//newT.L[hash[s]] += T.L[i];
				vector<Tuple> lset;
				lset.push_back(T.tuple[i]);
				newT.tuple[hash[s]].L.push_back(lset);
				//int id = newT.tuple[hash[s]].L.size();
				//newT.tuple[hash[s]].L[id].push_back(T.tuple[i]);
			}
		}
		A.attrNumber = Q.projectionNumber;
		A.attrName.clear();
		for (int i = 0; i < Q.projectionNumber; i++)
			A.attrName.push_back(Q.projectionAttr[i]);
		T = newT;
		T.tupleNumber = newT.tupleNumber;
	}

	void requery(View T, int x) {              //ti是结果中元组的编号，x是表的编号
		View D;
		D.tupleNumber = 0;
		D.tuple.clear();
		readin_helper(x, D);
		for (int ti = 0; ti < T.tupleNumber; ti++)
		{
			T.tuple[ti].L.clear();
		}
		int tid;
		for (int i = 0; i < D.tupleNumber; i++) {
			for (int ti = 0; ti < T.tupleNumber; ti++)
			{
				bool flag = true;
				for (int j = 0; j < DA[x].attrNumber; j++) {
					for (int k = 0; k < A.attrNumber; k++) {
						if (DA[x].attrName[j] == A.attrName[k]) {
							if (D.tuple[i].mState[j] == 0 && T.tuple[ti].mState[k] == 0 && nullSet.find(DA[x].attrName[j]) != nullSet.end())
							{
								D.tuple[i].x++; //  it misses the critical attributes
							}
							else
								if ((D.tuple[i].mState[j] == 0 && T.tuple[ti].mState[k] == 1) && (D.tuple[i].mState[j] == 1 && T.tuple[ti].mState[k] == 0))
								{
									flag = false;// not match
									break;    //exit the for loop of k
								}
								else if (D.tuple[i].mState[j] == 1 && T.tuple[ti].mState[k] == 1)
								{
									if (D.tuple[i].Values[j] != T.tuple[ti].Values[k])
									{
										flag = false;// not match
										break;     //exit the for loop of k
									}
								}
							/*
							if (D.tuple[i].Values[j] == "null" && nullSet.find(DA[x].attrName[j]) != nullSet.end())
							D.tuple[i].x++; //  it misses the critical attributes
							if (D.tuple[i].Values[j] != "null" && D.tuple[i].Values[j] != T.tuple[ti].Values[k])
							flag = false;
							*/
						}
					}
					if (flag == false)
					{
						D.tuple[i].x = 0;
						break; //exit the for loop of j;
					}
				}
				if (flag)
				{
					vector<Tuple> lset;
					lset.push_back(D.tuple[i]);
					T.tuple[ti].L.push_back(lset);
					if (D.tuple[i].x > 0)
					{
						g_qualityNumber++;
						g_quality += pow(g_a, D.tuple[i].x);
					}
				}
			}


		}
	}


	void requery_join(int ti, int x) {              //ti是结果中元组的编号，x是表的编号
		View D;
		D.tupleNumber = 0;
		D.tuple.clear();
		readin_helper(x, D);
		for (int i = 0; i < D.tupleNumber; i++) {
			bool flag = true;
			for (int j = 0; j < DA[x].attrNumber; j++) {
				for (int k = 0; k < A.attrNumber; k++) {
					if (DA[x].attrName[j] == A.attrName[k]) {
						if (T.tuple[ti].mState[k] == 0) // result tuple is missing on this attribute
						{
							if (D.tuple[i].mState[j] == 1) // it is not missing
							{
								flag = false;// not match
								break;    //exit the for loop of k
							}
							/*
							else if (D.tuple[i].mState[j] == 0) //  it is missing
							{
								if (nullSet.find(DA[x].attrName[j]) != nullSet.end()) // the missing attribute is the critical one
								{
									D.tuple[i].x = 1; //  it misses the critical attributes
								}
							}*/
						}
						else if (T.tuple[ti].mState[k] == 1) // the result tuple is observed in this attribute
						{

							if (D.tuple[i].mState[j] == 1 && D.tuple[i].Values[j] != T.tuple[ti].Values[k]) // it is observed but not equals the result tuple
							{
								flag = false;// not match
								break;    //exit the for loop of k
							}
							if (D.tuple[i].mState[j] == 0)  // it is missing on the non-critical attribute
							{
								flag = false;// not match
								if (nullSet.find(DA[x].attrName[j]) != nullSet.end())
								{
									D.tuple[i].x++;
								}
								break;    //exit the for loop of k
							}
						}

					}

				}
				if (flag == false)
				{
					break; //exit the for loop of j;
				}
			}
			if (flag)
			{
				T.tuple[ti].L[0].push_back(D.tuple[i]);// for join query, there is only one lineage set, and thus it's L[0].
			}
			if (D.tuple[i].x > 0)
			{
				g_quality += pow(g_a, D.tuple[i].x);
				g_qualityNumber++;
			}
		}
	}



	void load(const std::vector<std::string>& v)
	{
		names = v;
		for (auto& n : v)
		{
			paths.push_back(default_path + "archive\\" + n + ".csv");
		}
	}

	void init()                                                                     //作用：将各个表的属性写入到DA中，注意：前后的顺序和表的前后顺序是相关的
	{
		//前面的8个表是tpc_h数据集中的8个表；第9个表是com-dblp.ungraph.txt；第10个表是Crash_Data.csv
		//-----------TPC-H dataset: table attribute name-------------------------------------
		/*DA.push_back(Attribute(3, {"REGIONKEY", "NAME", "COMMENT"}));
		DA.push_back(Attribute(4, { "NATIONKEY", "NAME", "REGIONKEY", "COMMENT" }));
		DA.push_back(Attribute(9, { "PARTKEY", "NAME", "MFGR", "BRAND", "TYPE", "SIZE", "CONTAINER", "RETAILPRICE", "COMMENT" }));
		DA.push_back(Attribute(7, { "SUPPKEY", "NAME", "ADDRESS", "NATIONKEY", "PHONE", "ACCTBAL", "COMMENT" }));
		DA.push_back(Attribute(5, { "PARTKEY", "SUPPKEY", "AVAILQTY", "SUPPLYCOST", "COMMENT" }));
		DA.push_back(Attribute(8, { "CUSTKEY", "NAME", "ADDRESS", "NATIONKEY", "PHONE", "ACCTBAL", "MKTSEGMENT", "COMMENT" }));
		DA.push_back(Attribute(9, { "ORDERKEY", "CUSTKEY", "ORDERSTATUS", "TOTALPRICE", "ORDERDATE", "ORDERPRIORITY", "CLERK", "SHIPPRIORITY", "COMMENT" }));
		DA.push_back(Attribute(16, { "ORDERKEY", "PARTKEY", "SUPPKEY", "LINENUMBER", "QUANTITY", "EXTENDEDPRICE", "DISCOUNT", "TAX", "RETURNFLAG", "LINESTATUS", "SHIPDATE", "COMMITDATE", "RECEIPTDATE", "SHIPINSTRUCT", "SHIPMODE", "COMMENT" }));

		dim.push_back(3);
		dim.push_back(4);
		dim.push_back(9);
		dim.push_back(7);
		dim.push_back(5);
		dim.push_back(8);
		dim.push_back(9);
		dim.push_back(16);*/
		for (auto& path : paths)
		{
			auto file = ifstream(path);
			if (!file)
			{
				std::cout << path << " fail to open!!!" << '\n';
			}
			string headers;
			getline(file, headers);
			auto attrs = split(headers, std::string(",", 1));
			DA.push_back(Attribute(attrs.size(), attrs));
			dim.push_back(attrs.size());
			file.close();
		}
		countRows();
		//-----------DBLP dataset: table attribute name-------------------------------------
		/*DA.push_back(Attribute(2, {"FromNodeId", "ToNodeId"}));
		dim.push_back(2);

		//-----------Crash_Data dataset: table attribute name-------------------------------------
		DA.push_back(Attribute(40, { "CRASH_KEY", "CASENUMBER", "LECASENUM", "CRASH_DATE", "CRASH_MONTH", "CRASH_DAY", "TIMESTR", "DISTRICT", "COUNTY_NUMBER", "CITYNAME", "SYSTEMSTR", "LITERAL", "FRSTHARM", "LOCFSTHRM", "CRCOMNNR", "MAJCSE", "DRUGALC", "ECNTCRC", "LIGHT", "CSRFCND", "WEATHER", "RCNTCRC", "RDTYP", "PAVED", "WZRELATED", "CSEV", "FATALITIES", "INJURIES", "MAJINJURY", "MININJURY", "POSSINJURY", "UNKINJURY", "PROPDMG", "VEHICLES", "TOCCUPANTS", "REPORT", "XCOORD", "YCOORD", "OBJECTID", "SHAPE"
			}));
		dim.push_back(40);

		//-----------World dataset: table attribute name-------------------------------------
		DA.push_back(Attribute(5, { "CITYID", "CITYNAME", "COUNTRYCODE", "DISTRICT", "POPULATION" })); // city , "POPULATION"
		DA.push_back(Attribute(15, { "COUNTRYCODE", "COUNTRYNAME", "CONTINENT", "REGION", "SurfaceArea", "IndepYear", "POPULATION", "LifeExpectancy", "GNP", "GNPOld", "LocalNam", "GovernmentForm", "HeadOfState", "Capital", "Code2" })); //country "Code1",
		DA.push_back(Attribute(4, { "COUNTRYCODE", "LANGUAGE", "IsOfficial", "Percentage" })); //country language
		dim.push_back(5);
		dim.push_back(15);
		dim.push_back(4);


		//-----------SSB dataset: table attribute name-------------------------------------
		DA.push_back(Attribute(8, { "CUSTKEY", "NAME", "ADDRESS", "CITY", "NATION", "REGION", "PHONE", "MKTSEGMENT" }));//Customer
		DA.push_back(Attribute(7, { "SUPPKEY", "NAME", "ADDRESS", "CITY", "NATION", "REGION", "PHONE" })); //Supplier
		DA.push_back(Attribute(17, { "ORDERKEY", "LINENUMBER", "CUSTKEY", "PAPTKEY", "SUPPKEY", "ORDERDATE", "ORDERPRIORITY", "SHIPPRIORITY", "QUANTITY", "EXTENDEDPRICE", "ORDTOTALPRICE", "DISCOUNT", "REVENUE", "SUPPLYCOST", "TAX", "COMMITDATE", "SHIPMODE" }));//lineorder:
		DA.push_back(Attribute(9, { "PARTKEY", "NAME", "MFGR", "GATEGORY", "BRAND1", "COLOR", "TYPE", "SIZE", "CONTAINER" })); //part
		DA.push_back(Attribute(17, { "DATEKEY", "DATE", "DAYOFWEEK", "MONTH", "YEAR", "YEARMONTHNUM", "YEARMONTH", "DAYNUMINWEEK", "DAYNUMINMONTH", "DAYNUMINYEAR", "MONTHNUMINYEAR", "WEEKNUMINYEAR", "SELLINGSEASON", "LASTDAYINWEEKFL", "LASTDAYINMONTHFL", "HOLIDAYFL", "WEEKDAYFL" })); // dwDate
		dim.push_back(8);
		dim.push_back(7);
		dim.push_back(17);
		dim.push_back(9);
		dim.push_back(17);
		std::cout << "Have initialized attribute.." << endl;*/

	}
	/*
	void output_dataset()                                                         //作用：输出你选择的数据集的信息
	{
	if (type == 2 || type == 3)
	{
	if (Q.joinIndex[0] >= 2)
	{
	output_result("DATASET: TPC-H");
	std::cout << "DATASET: TPC-H" << endl;
	}
	}
	if (Q.joinIndex[0] >= 2 || Q.selectP >= 0 || Q.selectP <= 7 || Q.projectionP >= 0 || Q.projectionP <= 7)
	{
	output_result("DATASET: TPC-H");
	std::cout << "DATASET: TPC-H" << endl;
	}
	else if (Q.selectP == 8 || Q.projectionP == 8)
	{
	output_result("DATASET: DBLP");
	std::cout << "DATASET: DBLP" << endl;
	}
	else if (Q.selectP == 9 || Q.projectionP == 9)
	{
	output_result("DATASET: Crash");
	std::cout << "DATASET: Crash" << endl;
	}
	else if (Q.joinIndex[0] >= 2 || Q.selectP >= 10 || Q.selectP <= 12 || Q.projectionP >= 10 || Q.projectionP <= 12)
	{
	output_result("DATASET: World");
	std::cout << "DATASET: World" << endl;
	}
	else if (Q.joinIndex[0] >= 2 || Q.selectP >= 13 || Q.selectP <= 17 || Q.projectionP >= 13 || Q.projectionP <= 17)
	{
	output_result("DATASET: SSB");
	std::cout << "DATASET: SSB" << endl;
	}
	}
	*/

	/*<vector<string>> GetMlineage(<vector<string>> M, i)
	{

	//<vector<string>> M = " ; "; //the lineage set for the query, which gets one lineage set from each T.L[i]
	for (i = 0; i <= T.tupleNumber; i++) // T is the result tuple set of the query
	{
	for (j = 0; j <= T.L[i].size; j++)// T.L[i] is the collection of the lineage set of the i-th result tuple
	{
	//we need to get every lineage set  of the i-th result tuple
	//M = M union with the tuples in T.L[i][j]
	}
	}
	if (i = T.tupleNumber - 1)
	{
	return M;
	}
	}
	*/

	/*void GetCompleteRate()
	{
	if (Q.joinNumber > 0)
	{

	}
	Q.projectionP
	Q.selectP
	Q.joinIndex
	}*/

	void EagerStrategy()
	{
		//*************eager-approach************    
		//clock_t eagerstart = clock();

		if (type == 2)
		{
			int pos, pos1, pos2, j, x;
			if (Q.joinNumber > 0) {
				View T1;
				T1.tuple.clear();//执行JOIN操作
				T1.tupleNumber = 0;
				readin_helper(Q.joinIndex[0], T1); //将第一个要join的表赋给结果
				//std::cout << "T.size, i.e., the size of first join table: " << T.tupleNumber << endl;
				//std::cout << endl;
				A = DA[Q.joinIndex[0]];//连接的表号
				table[Q.joinIndex[0]] = 0;//在读取了多个表的情况下进行映射
				for (int i = 0; i < A.attrNumber; i++) { //维数
					if (A.attrName[i] == Q.joinAttr[0]) {
						pos = i;  //得到第一个表连接的属性
						break;
					}
				}
				if (pos < 0)
				{
					std::cout << "Not find the foreign key!" << endl;
				}
				g_quality = 0;
				g_qualityNumber = 0;
				for (int i = 1; i < Q.joinNumber; i++) {
					pos1 = pos;
					int x = Q.joinIndex[i]; //当前表号,join 的第二个表的编号
					table[x] = i;
					for (int j = 0; j < DA[x].attrNumber; j++)
					{ //循环当前表属性
						if (DA[x].attrName[j] != Q.joinAttr[i - 1])
						{ //如果不是外键属性
							A.attrNumber++;
							A.attrName.push_back(DA[x].attrName[j]);//将属性添加到结果属性中
						}
						else
						{
							pos2 = j;
						}
					}
					View T2;
					T2.tuple.clear();
					T2.tupleNumber = 0;
					readin_helper(x, T2);
					join(T1, T2, pos1, pos2); //将当前结果与另一个表join然后更新结果
					//T1.tuple.clear;
					//T2.tuple.clear;
				}
				// compute quality of join;
				if (g_quality != 0)
				{
					g_quality = (g_quality + T.tupleNumber) / (g_qualityNumber + T.tupleNumber);
				}
				else g_quality = 1;
				std::cout << "join ending!" << endl;
			}
		}
		else if (type == 0)
		{
			g_quality = 0;
			g_qualityNumber = 0;
			//if (Q.selectNumber > 0) {                                           //执行select操作
			if (Q.selectP >= 0) {
				A = DA[Q.selectP];
				readin_helper(Q.selectP, T);
				table[Q.selectP] = 0;
			}
			select_eager();
			// compute quality of join;
			if (g_quality != 0)
			{
				g_quality = (g_quality + T.tupleNumber) / (g_qualityNumber + T.tupleNumber);
			}
			else g_quality = 1;
			output_result("select ending!");
			//}
		}
		else if (type == 1)
		{
			g_quality = 0;
			g_qualityNumber = 0;
			if (Q.projectionNumber > 0) {                                        //执行投影操作
				if (Q.projectionP >= 0) {
					A = DA[Q.projectionP];
					readin_helper(Q.projectionP, T);
					table[Q.projectionP] = 0;
				}
				projection_eager();
				// compute quality of join;
				if (g_quality != 0)
				{
					g_quality = (g_quality + T.tupleNumber) / (g_qualityNumber + T.tupleNumber);
				}
				else g_quality = 1;
				output_result("projecting ending!");
			}
		}
		else if (type == 3)
		{
		}
	}

	float PriceComputation(const vector<Tuple>& m)
	{
		float price = 0;
		//int d;//the table ID 
		for (int i = 0; i < m.size(); i++)
		{
			int completeNum = 0;
			// TODO: 这里的price怎么计算的？怎么从M来获取各自对应的信息？
			for (int j = 0; j < m[i].mState.size(); j++)
			{
				if (m[i].mState[j] == 1)
				{
					completeNum++;
				}

			}

			if (completeNum > m[i].Values.size())
			{
				//std::cout << "completeNum  " << completeNum << " > attributeNum  " << m[i].Values.size() << endl;
				break;
			}
			price += g_b0 * ((double)completeNum / m[i].Values.size());
			//price = price + base-price of M[i] * (alpha * M[i]'s complete rate + (1-alpha) * M[i]'s quality) 
		}
		return price;
	}
	/*
	float PriceComputation(View m)
	{
		float price = 0;
		//int d;//the table ID
		for (int i = 0; i < m.tupleNumber; i++)
		{
			int completeNum = 0;
			// TODO: 这里的price怎么计算的？怎么从M来获取各自对应的信息？
			for (int j = 0; j < m.tuple[i].mState.size(); j++)
			{
				if (m.tuple[i].mState[j] == 1)
				{
					completeNum++;
				}

			}

			if (completeNum > m.tuple[i].Values.size())
			{
				std::cout << "completeNum  " << completeNum << " > attributeNum  " << m.tuple[i].Values.size() << endl;
				break;
			}
			price += g_b0*(completeNum / m.tuple[i].Values.size());
			//price = price + base-price of M[i] * (alpha * M[i]'s complete rate + (1-alpha) * M[i]'s quality)
		}
		return price;
	}
	*/

	float Quality(const vector<Tuple>& m)
	{
		float q = 0;
		//int d;//the table ID 
		for (int i = 0; i < m.size(); i++)
		{
			// TODO: 这里要计算M[i]的质量，是怎么获取的？
			// q += 0.1;
			q = q + pow(g_a, m[i].x); // sum of the quality of all the tuples in M
		}
		return q / m.size();
	}



	void LazyStrategy()
	{
		/****************lazy-approach**************/
		if (type == 0)
		{
			g_qualityNumber = 0;
			g_quality = 0;
			for (int i = 0; i < Q.selectNumber; i++)
				nullSet.insert(DA[Q.selectP].attrName[i]);
			//for (int i = 0; i < T.tupleNumber; i++)
			//{

			//}
			requery(T, Q.selectP);
			if (g_quality != 0)
			{
				g_quality = g_quality / g_qualityNumber;
			}
			else g_quality = 1;
		}
		else if (type == 1)
		{
			g_qualityNumber = 0;
			g_quality = 0;
			for (int i = 0; i < Q.projectionNumber; i++)
				nullSet.insert(DA[Q.projectionP].attrName[i]);
			//for (int i = 0; i < T.tupleNumber; i++)
			//{

			//}
			requery(T, Q.projectionP);
			if (g_quality != 0)
			{
				g_quality = g_quality / g_qualityNumber;
			}
			else g_quality = 1;
		}
		else if (type == 2)
		{
			int i, j;
			g_qualityNumber = 0;
			g_quality = 0;
			for (i = 0; i < Q.joinNumber - 1; i++)
				nullSet.insert(Q.joinAttr[i]);

			for (i = 0; i < T.tupleNumber; i++)
			{
				for (j = 0; j < Q.joinNumber; j++) {
					requery_join(i, Q.joinIndex[j]);
				}
			}
			if (g_quality != 0)
			{
				g_quality = g_quality / g_qualityNumber;
			}
			else g_quality = 1;
		}
		/*
		else if (Q.selectNumber > 0) {
		for (int i = 0; i < T.tupleNumber; i++)
		{
		requery(T, i, Q.selectP);
		}
		}
		else if (Q.projectionNumber > 0) {
		for (int i = 0; i < T.tupleNumber; i++)
		{
		requery(T, i, Q.projectionP);
		}
		}*/

	}

	vector<Tuple> tuple_set_union(const vector<Tuple>& s1, const vector<Tuple>& s2)
	{
		vector<Tuple> tempS = s1;
		//tempS.reserve(s1.size() + s2.size());
		for (int i = 0; i < s2.size(); i++)
		{
			bool flag = true;
			for (int j = 0; j < s1.size(); j++)
			{
				if (s2[i].Values == tempS[j].Values)
				{
					flag = false;
					break;
				}

			}
			if (flag)
			{
				tempS.push_back(s2[i]);
			}
		}

		return tempS;
	}

	bool match_tuple_vec(const vector<Tuple>& a, const vector<Tuple>& b) {
		if (a.size() != b.size())
			return false;
		for (int i = 0; i < a.size(); i++) {
			if (!(a[i] == b[i])) {
				return false;
			}
		}

		return true;
	}

	bool search_tuple_vec(const vector<vector<Tuple>>& set, const vector<Tuple>& target)
	{
		/*for (int i = 0; i < set.size(); i++) {
			if (match_tuple_vec(set[i], target))
				return true;
		}*/

		//return false;
		return std::find_if(set.begin(), set.end()
			, [&](const vector<Tuple>& t) {return match_tuple_vec(t, target); }) != set.end();
	}

	vector<vector<Tuple>> get_Mlineage(const View& T, int layer, const vector<Tuple>& m)
	{
		vector<vector<Tuple>> totalM;
		int J = m.size();
		if (layer == T.tupleNumber - 1)// the last layer
		{
			if (J == 0)
			{
				for (int i = 0; i < T.tuple[layer].L.size(); i++)
				{
					//m = T.tuple[layer].L[i];
					//totalM.push_back(m);
					totalM.push_back(T.tuple[layer].L[i]);
				}
			}
			else {
				for (int i = 0; i < T.tuple[layer].L.size(); i++)
				{
					vector<Tuple> temp = tuple_set_union(m, T.tuple[layer].L[i]);
					if (!search_tuple_vec(totalM, temp))
					{
						totalM.push_back(temp);
					}

				}
			}
			return totalM;
		}
		else if (layer < T.tupleNumber - 1)// not the last layer
		{
			if (layer == 0)
			{
				//std::cout <<"At the first layer of get-Mlineage"<< endl;
				for (int i = 0; i < T.tuple[layer].L.size(); i++)
				{
					//m = T.tuple[layer].L[i];
					//vector<Tuple> newm = m;
					int newlayer = layer + 1;
					//vector<vector<Tuple>> newM = get_Mlineage(T, newlayer, m);
					vector<vector<Tuple>> newM = get_Mlineage(T, newlayer, T.tuple[layer].L[i]);
					for (int k = 0; k < newM.size(); k++)
					{
						if (!search_tuple_vec(totalM, newM[k]))
						{
							totalM.push_back(newM[k]);
						}
					}

				}
			}
			else {
				//std::cout << "At the " <<layer<<" layer of get-Mlineage" << endl;
				for (int i = 0; i < T.tuple[layer].L.size(); i++)
				{
					vector<Tuple> temp = tuple_set_union(m, T.tuple[layer].L[i]);
					int newlayer = layer + 1;
					vector<vector<Tuple>> newM = get_Mlineage(T, newlayer, temp);
					for (int k = 0; k < newM.size(); k++)
					{
						if (!search_tuple_vec(totalM, newM[k]))
						{
							totalM.push_back(newM[k]);
						}
					}

				}
			}
		}
		return totalM;
	}

	//function for Algorithm1: Exact with Eager strategy (Exact-E)
	void Exact(float Phi)
	{
		/*Results result;
		std::cout <<"Entering ExactEager Algorithm!" << endl;
		clock_t starttime = clock();
		EagerStrategy();// it only gets the lineage set for each result tuple
		result.ResTuples = T;
		result.ResAttrs = A;
		*/
		//Results result;
		//result.ResTuples = T;

		//result.ResAttrs = A;

		//std::cout << "Query result tuple Size: " << T.tupleNumber << endl;

		if (T.tupleNumber == 0)
		{
			std::cout << "The result set is empty!" << endl;
		}
		/*
		std::cout << "Query result tuple Size: " << T.tupleNumber<<endl;
		for (int i = 0; i < T.tupleNumber; i++)
		{
		std::cout << T.tuple[i].Values[0] <<  endl;
		}

		std::cout << "Query result attributes:" << endl;
		for (int i = 0; i < A.attrNumber; i++)
		{
		std::cout << A.attrName[i].data() << endl;
		} */

		clock_t starttime = clock();
		// then get every possible lineage set of the query via the union operation	
		vector<Tuple> m;//the lineage set for the query, which gets one lineage set from each T.L[i]
		// as you said, perhaps a recursive function is useful here. I am lost.
		m.clear();
		std::cout << "Entering get_Mlineage function.." << endl;
		vector<vector<Tuple>> M = get_Mlineage(T, 0, m);  // get all the possible lineage set of the query


		std::cout << "Get_Mlineage function finishes!" << endl;
		float price = -1;
		for (int i = 0; i < M.size(); i++)
		{
			float p = PriceComputation(M[i]);
			if (price == -1 || p < price) {
				price = p;
				m = M[i];
			}
		}


		//result.Mlineage = m; // the lineage with the lowest price
		//result.price = price*Phi;//phi is the complete rate of the related table to query
		//std::cout << "Price of Exact algorithm: " << price << endl;
		clock_t endtime = clock();
		//float quality = Quality(m);

		g_exact_price = price;

		output_result("Time used of Exact: ");
		output_number(endtime - starttime);
		output_result("The number of tuple in Lineage  set: ");
		output_number(m.size());
		output_result("Exact's derived base price: ");
		output_number(price);
		output_result("Exact's derived price: ");
		output_number(price * Phi);
		output_result("The quality: ");
		output_number(g_quality);

		//output_result("The lowest-price lineage set size: ");
		std::cout << "The lowest-price lineage set size: " << m.size() << endl;
		/*for (int i = 0; i < m.size(); i++)
		{
			output_result(m[i].Values[0]);
			output_result(m[i].Values[1]);
		std::cout << m[i].Values[0] << " " << m[i].Values[1] << endl;
		}*/
		std::cout << "Based_price of Exact algorithm: " << price << endl;
		std::cout << "Quality of Exact algorithm: " << g_quality << endl;
		//write result in an external file; // best at an excel file
		//write the time of(endtime - starttime);
	}





	//function for Algorithm2: Approx with Eager strategy (Approx-E)
	void Approx(float Phi)
	{
		//Results result;
		//result.ResTuples = T;
		//result.ResAttrs = A;
		clock_t starttime = clock();
		//EagerStrategy();// it only get the lineage set for each result tuple



		/*In the following, different from exact, we find the lineage set of each result tuple with the lowest price*/
		// TODO : price先改成了-1；原本是无穷

		float base_price = 0;
		vector<Tuple> m;
		m.clear();
		/*
		if (type == 0 || type == 2)
		{
			base_price = PriceComputation(T);

		}
		else
			*/
		if (true)
		{
			float price = -1;
			vector<vector<Tuple>> minL; // it is used to store the lowest-price lineage sets of all the result tuples
			minL.clear();
			for (int i = 0; i < T.tupleNumber; i++) // T is the result tuple set of the query
			{

				// get the union set of the minL[i]
				// TODO: price先改成了-1；原本是无穷
				float price = -1;
				vector<Tuple> lset;
				for (int j = 0; j < T.tuple[i].L.size(); j++)// T.L[i] is the collection of the lineage set of the i-th result tuple
				{
					float p = PriceComputation(T.tuple[i].L[j]);
					if (price == -1 || p < price) {
						price = p;
						lset = T.tuple[i].L[j];
					}
				}
				minL.push_back(lset);
				//std::cout <<"Processing result tuple: "<< i << endl;
			}
			std::cout << "starting get the query lineage via join.. " << endl;
			for (int i = 0; i < T.tupleNumber; i++)
			{
				m = tuple_set_union(m, minL[i]);
			}

			//result.Mlineage = m;  
			base_price = PriceComputation(m);
		}
		//result.price = base_price *Phi; //phi is the complete rate of the related table to query
		clock_t endtime = clock();
		//result.quality = Quality(m);
		float quality = 0;
		/*if (type == 0 || type == 2)
		{
			//quality = Quality(T);
			output_result("The number of tuple in Lineage  set: ");
			output_number(T.tupleNumber);

		}
		else*/
		if (true)
		{
			//quality = Quality(m);
			output_result("The number of tuple in Lineage  set: ");
			output_number(m.size());
			std::cout << "The lowest-price lineage set size of Approx: " << m.size() << endl;
		}

		output_result("Time used of Approx: ");
		output_number(endtime - starttime);

		output_result("Approx's derived base price: ");
		output_number(base_price);
		output_result("Approx's derived price: ");
		output_number(base_price * Phi);
		output_result("The quality: ");
		output_number(g_quality);

		no_history_price += base_price;
		//output_result("The approximate ratio: ");
		//output_number(result.ratioApprox);


		/*for (int i = 0; i < m.size(); i++)
		{
		std::cout << m[i].Values[0] << " " << m[i].Values[1] << endl;
		}*/
		std::cout << "Price of Approx algorithm: " << base_price << endl;
		std::cout << "Quality of Approx  algorithm: " << g_quality << endl;
		//write result in an external file; // best at an excel file
		//write the time of(endtime - starttime);
	}


	/*
	float progressivePriceComputation(vector<Tuple> m, vector<Tuple> History)
	{
	float price = 0;
	for (int i = 0; i < m.size(); i++)
	{
	// TODO: 这里需要重新实现一下
	bool flag = true;
	for (int j = 0; j < History.size(); j++)
	{
	if (m[i].Values == History[j].Values)
	{
	flag = false;
	break;
	}
	}
	if (flag)
	{
	if (m[i].completeNum > m[i].Values.size)
	{
	std::cout << "completeNum > attributeNum" << endl;
	break;
	}
	price += b0*(alpha * m[i].completeNum / m[i].Values.size + (1 - alpha)*pow(a, m[i].x));
	}
	}
	return price;
	}
	*/

	float HistoryPriceComputation(const vector<Tuple>& m, vector<Tuple> History)
	{
		float price = 0;
		vector<Tuple>::const_iterator m_iter;
		for (m_iter = m.cbegin(); m_iter != m.cend(); m_iter++) {
			// TODO: 这里需要重新实现一下
			bool flag = true;

			vector<Tuple>::iterator h_iter;
			for (h_iter = History.begin(); h_iter != History.end();) {
				if (m_iter->Values == h_iter->Values)
				{
					if (m_iter->label == h_iter->label)
					{
						flag = false;
						h_iter++;
					}
					else {
						h_iter = History.erase(h_iter);// delete the tuple History[j]
					}
					break;
				}
				else {
					h_iter++;
				}
			}
			if (flag)
			{
				int completeNum = 0;
				for (int j = 0; j < m_iter->mState.size(); j++)
				{
					if (m_iter->mState[j] == 1)
					{
						completeNum++;
					}

				}

				if (completeNum > m_iter->Values.size())
				{
					std::cout << "completeNum  " << completeNum << " > attributeNum  " << m_iter->Values.size() << endl;
					break;
				}
				/*
				if (m_iter->completeNum > m_iter->Values.size())
				{
				std::cout << "completeNum > attributeNum" << endl;
				break;
				}*/
				price += g_b0 * ((double)completeNum / m_iter->Values.size());
			}
		}
		return price;
	}


	void ProgressiveApprox(float Phi)
	{
		//Results result;
		//result.ResTuples = T;
		//result.ResAttrs = A;

		clock_t starttime = clock();
		//EagerStrategy();// it only get the lineage set for each result tuple

		/*In the following, different from exact, we find the lineage set of each result tuple with the lowest price*/
		vector<Tuple> m; // it is used to get the unique lineage set of the query via collecting all the lowest-price lineage set of each result tuple
		m.clear();
		float price = -1;
		vector<Tuple> minl; // it is used to store the lowest-price lineage sets of all the result tuples
		minl.clear();
		for (int i = 0; i < T.tupleNumber; i++) // T is the result tuple set of the query
		{
			float price = -1;
			for (int j = 0; j < T.tuple[i].L.size(); j++)// T.L[i] is the collection of the lineage set of the i-th result tuple
			{
				float p = HistoryPriceComputation(T.tuple[i].L[j], m);
				if (price == -1 || p < price) {
					price = p;
					minl = T.tuple[i].L[j];
				}
			}
			//TODO : 这里需要实现并的操作
			//M = M union with minL[i];
			m = tuple_set_union(m, minl);
			//std::cout << "Processing result tuple: " << i << endl;
		}

		//result.Mlineage = m;
		float base_price = PriceComputation(m);
		//result.price = result.base_price*Phi;//phi is the complete rate of the related table to query
		clock_t endtime = clock();

		//float quality = Quality(m);

		//result.ratioApprox = result.price / g_exact_price;

		output_result("Time used of PApprox: ");
		output_number(endtime - starttime);
		output_result("The number of tuple in Lineage  set: ");
		output_number(m.size());
		output_result("PApprox's derived base price: ");
		output_number(base_price);
		output_result("PApprox's derived price: ");
		output_number(base_price * Phi);
		output_result("The quality: ");
		output_number(g_quality);
		//output_result("The approximate ratio: ");
		//output_number(result.ratioApprox);

		std::cout << "The lowest-price lineage set size of PApprox: " << m.size() << endl;
		/*for (int i = 0; i < m.size(); i++)
		{
		std::cout << m[i].Values[0] << " " << m[i].Values[1] << endl;
		}*/
		std::cout << "Base_price of PApprox  algorithm: " << base_price << endl;
		std::cout << "Quality of PApprox PExact algorithm: " << g_quality << endl;
		//write result in an external file; // best at an excel file
		//write the time of(endtime - starttime);
	}



	void HistoryExactEager(float Phi, vector<Tuple>& History)
	{
		Results result;

		clock_t starttime = clock();
		EagerStrategy();// it only get the lineage set for each result tuple
		result.ResTuples = T;
		result.ResAttrs = A;
		// then get every possible lineage set of the query via the union operation
		vector<Tuple> m;//the lineage set for the query, which gets one lineage set from each T.L[i]
		// as you said, perhaps a recursive function is useful here. I am lost.
		m.clear();
		vector<vector<Tuple>> M = get_Mlineage(T, 0, m);  // get all the possible lineage set of the query


		//-----------------------------------Below is the special for history-aware-------------------------//
		/*vector<Tuple> H; // used to store the non-updated tuples in History
		for (int i = 0; i < History.size(); i++)
		{
		//TODO : xucheng
		//if (History[i].label is not equal to Tuple[IndexofHistory[i]].label)// i.e., this tuple has been updated
		//{
		//	H.insert(History[i]);
		//}
		//else{
		//	History.delete(History[i]);
		//}
		if (History[i].label!=T.tuple)
		{
		}
		}*/
		//-----------------------------------Above is the special for history-aware-------------------------//


		float price = -1;
		for (int i = 0; i < M.size(); i++)
		{
			float p = HistoryPriceComputation(M[i], History);
			if (price == -1 || p < price) {
				price = p;
				m = M[i];
			}
		}


		result.Mlineage = m; // the lineage with the lowest price
		result.price = price * Phi;//phi is the complete rate of the related table to query
		clock_t endtime = clock();
		result.quality = Quality(result.Mlineage);



		/*different from the previous algorithms, we now have a tuple set H, it is assumed the set that
		the consumer have the rights to re-use, and thus we should not count the price of the tuples in H
		in find the lowest-price lineage set M */


		/*compute the pricing by recalling HistoryPriceComputation(M, H)*/



		//------------------ in the following, update History for pricing the next query
		for (int j = 0; j < result.Mlineage.size(); j++)
		{
			//TODO : xucheng
			//if (result.Mlineage[i] is not in History)
			//{
			//	History.insert(result.Mlineage[i]);
			//}
			//History[index of result.Mlineage[i]].label = result.Mlineage[i].label;
			bool flag = true;
			for (int k = 0; k < History.size(); k++)
			{
				if (result.Mlineage[j].Values == History[k].Values)
				{
					History[k].label = result.Mlineage[j].label;
					flag = false;
					break;
				}
			}
			if (flag)
			{
				History.push_back(result.Mlineage[j]);
				int id = History.size();
				History[id - 1].label = result.Mlineage[j].label;
			}
		}
	}


	void HistoryApprox(float Phi)
	{
		clock_t starttime = clock();
		float base_price = 0;
		vector<Tuple> m;
		m.clear();

		float price = -1;
		vector<vector<Tuple>> minL; // it is used to store the lowest-price lineage sets of all the result tuples
		minL.clear();
		for (int i = 0; i < T.tupleNumber; i++) // T is the result tuple set of the query
		{
			float price = -1;
			vector<Tuple> lset;
			for (int j = 0; j < T.tuple[i].L.size(); j++)// T.L[i] is the collection of the lineage set of the i-th result tuple
			{
				float p = HistoryPriceComputation(T.tuple[i].L[j], token);
				if (price == -1 || p < price) {
					price = p;
					lset = T.tuple[i].L[j];
				}
			}
			minL.push_back(lset);
			//std::cout <<"Processing result tuple: "<< i << endl;
		}
		std::cout << "starting get the query lineage via join.. " << endl;
		for (int i = 0; i < T.tupleNumber; i++)
		{
			m = tuple_set_union(m, minL[i]);
		}

		//result.Mlineage = m;  
		base_price = HistoryPriceComputation(m, token);

		//result.price = base_price *Phi; //phi is the complete rate of the related table to query
		clock_t endtime = clock();
		//result.quality = Quality(m);
		float quality = 0;
		//quality = Quality(m);
		output_result("The number of tuple in Lineage  set: ");
		output_number(m.size());
		std::cout << "The lowest-price lineage set size of Approx: " << m.size() << endl;


		output_result("Time used of Approx: ");
		output_number(endtime - starttime);

		output_result("Approx's derived base price: ");
		output_number(base_price);
		g_price = base_price;
		output_result("Approx's derived price: ");
		output_number(base_price * Phi);
		output_result("The quality: ");
		output_number(g_quality);
		//output_result("The approximate ratio: ");
		//output_number(result.ratioApprox);


		/*for (int i = 0; i < m.size(); i++)
		{
		std::cout << m[i].Values[0] << " " << m[i].Values[1] << endl;
		}*/
		std::cout << "Price of Approx algorithm: " << base_price << endl;
		std::cout << "Quality of Approx  algorithm: " << g_quality << endl;
		//write result in an external file; // best at an excel file
		//write the time of(endtime - starttime);


		//------------------ in the following, update token for pricing the next query
		for (int j = 0; j < m.size(); j++)
		{
			bool flag = true;
			for (int k = 0; k < token.size(); k++)
			{
				if (m[j].Values == token[k].Values)
				{
					token[k].label = m[j].label;
					flag = false;
					break;
				}
			}
			if (flag)
			{
				token.push_back(m[j]);
				//int id = History.size();
				//History[id - 1].label = result.Mlineage[j].label;
			}
		}


	}


	void HistoryProgressiveApprox(float Phi)// it is not correct now...
	{
		//Results result;
		//result.ResTuples = T;
		//result.ResAttrs = A;

		clock_t starttime = clock();
		//EagerStrategy();// it only get the lineage set for each result tuple

		/*In the following, different from exact, we find the lineage set of each result tuple with the lowest price*/
		vector<Tuple> m; // it is used to get the unique lineage set of the query via collecting all the lowest-price lineage set of each result tuple
		m.clear();
		float price = -1;
		vector<Tuple> minl; // it is used to store the lowest-price lineage sets of all the result tuples
		minl.clear();
		vector<Tuple> temp = token;
		for (int i = 0; i < T.tupleNumber; i++) // T is the result tuple set of the query
		{
			float price = -1;
			for (int j = 0; j < T.tuple[i].L.size(); j++)// T.L[i] is the collection of the lineage set of the i-th result tuple
			{
				float p = HistoryPriceComputation(T.tuple[i].L[j], temp);
				if (price == -1 || p < price) {
					price = p;
					minl = T.tuple[i].L[j];
				}
			}
			m = tuple_set_union(m, minl);
			temp = tuple_set_union(temp, minl);/////////// there is  some wrong on the tuple label update in temp...
		}

		//result.Mlineage = m;
		float base_price = HistoryPriceComputation(m, token);
		//result.price = result.base_price*Phi;//phi is the complete rate of the related table to query
		clock_t endtime = clock();

		//float quality = Quality(m);

		//result.ratioApprox = result.price / g_exact_price;

		output_result("Time used of PApprox: ");
		output_number(endtime - starttime);
		output_result("The number of tuple in Lineage  set: ");
		output_number(m.size());
		output_result("PApprox's derived base price: ");
		output_number(base_price);
		output_result("PApprox's derived price: ");
		output_number(base_price * Phi);
		output_result("The quality: ");
		output_number(g_quality);
		//output_result("The approximate ratio: ");
		//output_number(result.ratioApprox);

		std::cout << "The lowest-price lineage set size of PApprox: " << m.size() << endl;
		/*for (int i = 0; i < m.size(); i++)
		{
		std::cout << m[i].Values[0] << " " << m[i].Values[1] << endl;
		}*/
		std::cout << "Base_price of PApprox  algorithm: " << base_price << endl;
		std::cout << "Quality of PApprox PExact algorithm: " << g_quality << endl;
		//write result in an external file; // best at an excel file
		//write the time of(endtime - starttime);


		//------------------ in the following, update token for pricing the next query
		for (int j = 0; j < m.size(); j++)
		{
			bool flag = true;
			for (int k = 0; k < token.size(); k++)
			{
				if (m[j].Values == token[k].Values)
				{
					token[k].label = m[j].label;
					flag = false;
					break;
				}
			}
			if (flag)
			{
				token.push_back(m[j]);
				//int id = History.size();
				//History[id - 1].label = result.Mlineage[j].label;
			}
		}
	}


	void generate_miss_rate_file(int tuple_num, int dim, double miss_rate, string&& file_name) {
		ofstream file(file_name, ios::trunc);

		srand(time(0));

		if (!file)
			exit(0);

		int t0 = tuple_num % 1000; //need to change
		int t1 = tuple_num / 1000;//need to change
		for (int k = 0; k < t1; k++)
		{
			tuple_num = 1000;//need to change
			int exist_cnt = tuple_num * dim * (1 - miss_rate);
			unsigned char(*tuple)[MAX_DIM] = new unsigned char[tuple_num][MAX_DIM];


			for (int i = 0; i < tuple_num; i++) {
				memset(tuple[i], 0, dim);
				int rand_value = rand() % dim;

				tuple[i][rand_value] = 1;
				exist_cnt--;
			}

			while (exist_cnt) {
				int rand_value = rand() % (tuple_num * dim);
				int col = rand_value % dim;
				int row = rand_value / dim;
				if (tuple[row][col] == 1)
					continue;
				tuple[row][col] = 1;
				exist_cnt--;
			}

			for (int i = 0; i < tuple_num; i++) {
				for (int j = 0; j < dim; j++) {
					file << (tuple[i][j] == 0 ? "0" : "1");
					file << " ";
				}
				file << endl;
				if (i % 100 == 0)
				{
					std::cout << i << endl;
				}
			}
		}
		tuple_num = t0;
		std::cout << "tuple num: " << tuple_num << '\n';
		int exist_cnt = tuple_num * dim * (1 - miss_rate);
		unsigned char(*tuple)[MAX_DIM] = new unsigned char[tuple_num][MAX_DIM];
		//vector<vector<int>> tuple(tuple_num, std::vector<int>(0, MAX_DIM));


		for (int i = 0; i < tuple_num; i++) {
			memset(tuple[i], 0, dim);
			int rand_value = rand() % dim;

			tuple[i][rand_value] = 1;
			exist_cnt--;
		}

		while (exist_cnt) {
			int rand_value = rand() % (tuple_num * dim);
			int col = rand_value % dim;
			int row = rand_value / dim;
			if (tuple[row][col] == 1)
				continue;
			tuple[row][col] = 1;
			exist_cnt--;
		}

		for (int i = 0; i < tuple_num; i++) {
			for (int j = 0; j < dim; j++) {
				file << (tuple[i][j] == 0 ? "0" : "1");
				file << " ";
			}
			file << endl;
			if (i % 100 == 0)
			{
				std::cout << i << endl;
			}
		}

		file.close();
	}

	void generate_label_file(int tuple_num, int version, float update_rate, string&& file_name) {

		//initial the first label file
		vector<int>state;
		state.clear();
		ofstream file(file_name, ios::trunc);
		if (!file)
			exit(0);
		for (int i = 0; i < tuple_num; i++) {
			file << 0 << endl;
			state.push_back(0);
		}
		file.close();

		int update = update_rate * 10;
		srand(time(0));
		for (int k = 1; k < version; k++) {
			std::cout << "The version: " << k << endl;
			ofstream file(file_name + to_string(k) + ".l", ios::trunc);
			if (!file)
				exit(0);
			for (int i = 0; i < tuple_num; i++) {

				if (rand() % 10 < update)
				{
					file << state[i] + 1 << endl;
					state[i] = state[i] + 1;
				}
				else
				{
					file << state[i] << endl;
				}

			}
			file.close();
		}

	}

	float phi;

	int query(const std::string& sqlQuery)
	{
		//parser.parse("SELECT * FROM games g, games_details gd WHERE g.GAME_ID = gd.GAME_ID");
		//parser.parse("SELECT * FROM teams t, players p WHERE t.TEAM_ID = p.TEAM_ID");
		parser.parse(sqlQuery);
		parser.print();
		//cin.get();
		//if (false)


			/*if (argc != 11) {
				printf("error args!.\n");
				exit(0);
			}*/

			/*g_alpha = atof(argv[1]);
			g_a = atof(argv[2]);
			g_b0 = atof(argv[3]);
			g_label = atoi(argv[4]);
			g_viewNumber = atoi(argv[5]); //表的个数
			g_queryNumber = atoi(argv[6]); //表的个数
			g_missing_rate = atof(argv[7]);
			datasize = atoi(argv[8]);
			string out_file_name(argv[9]);
			string queryPath = default_path + argv[10];*/

		printf("g_alpha = %f, g_a = %f, g_b0 = %f, g_label = %d, g_viewNumber = %d, g_missing_rate = %f, datasize=%d, g_queryNumber = %d\n", g_alpha, g_a, g_b0, g_label, g_viewNumber, g_missing_rate, datasize, g_queryNumber);
		//std::cout << out_file_name << endl;
		//std::cout << queryPath << endl;
		/*


		//getchar();

		g_alpha = 0.5;
		g_a = 0.5;
		g_b0 = 1;
		g_label = 1;
		g_viewNumber = 17; //表的个数
		g_missing_rate = 0.2;
		datasize = 6002215;// 6001215; //1049866;// 5000;
		g_queryNumber = 20;
		string out_file_name = "q-t-0-history.txt";
		//string queryPath = default_path + argv[8];

		//float missing_rate = 0.2;
		string queryPath = default_path + "q-s-0.txt";
		 */
		 /*Before the execution of the main function, we need to generate the missing information of every table, and
		 the missing is random at each table using a random function. We can use the missing rate to control the missing degree.
		 Thus, for each table we have, generate the missing information file.
		 */


		 //Input file parameters: Query file, price file, 
		 //(ignore for now)Input algorithm type: h(=0, history-oblivious; =1, histroy-aware), algo(=0, exact,; =1, approx; =2, approx)
		 //Input algorithm parameters: a (for quality function), \alpha(for price function), \rho(for base price) 
		 //getchar();
		init();           //初始化所有相关数据集的表的属性名

		//readin_query(queryPath);
		readin_history_query();

		//string data = datatype;
		/*string s = datatype;
		s += "-";
		s += to_string(type);
		s += "-";
		s += to_string(g_missing_rate);
		s += "-";
		//srand(time(0));
		s += to_string(time(NULL));
		s += ".txt";*/
		output.open(".\\output.txt", ios::trunc);
		if (!output)
		{
			std::cout << "Open file error!" << endl;
			cin.get();
			exit(0);

		}

		//output_result("parameters: ");
		//output_result(queryPath);
		/*for (int i = 0; i < paths.size(); ++i)
		{
			generate_label_file(rowNumber[i], 1, 1, default_path + std::to_string(i) + std::string(".l"));
			generate_miss_rate_file(rowNumber[i], dim[i], 0.2
				, default_path + std::to_string(i) + std::string(".m"));
		}*/

		//-------------------------------generating the data labels simulating data update------------------------
		//TPC-H
		/*
		generate_label_file(5, 20, 1, default_path + "\\0-");
		generate_label_file(25, 20, 1, default_path + "\\1-");
		generate_label_file(200000, 20, 1, default_path + "\\2-");
		generate_label_file(10000, 20, 1, default_path + "\\3-");
		generate_label_file(800000, 20, 1, default_path + "\\4-");
		generate_label_file(150000, 20, 1, default_path + "\\5-");
		generate_label_file(1500000, 20, 1, default_path + "\\6-");
		generate_label_file(6001215, 20, 1, default_path + "\\7-");
		*/

		/*150000 customer.tbl
		6001215 lineitem.tbl
		25 nation.tbl
		1500000 orders.tbl
		800000 partsupp.tbl
		200000 part.tbl
		5 region.tbl
		10000 supplier.tbl
		8661245 总用量
		*/
		//DBLP
		//generate_label_file(1049866, 1, default_path + "\\8.l");

		//Crash
		//generate_label_file(322450, 1, default_path + "\\9.l");

		//World
		//generate_label_file(4079, 1, default_path + "\\10.l"); //city
		//generate_label_file(239, 1, default_path + "\\11.l"); //country
		//generate_label_file(984, 1, default_path + "\\12.l"); //country language

		//SSB
		/*
		generate_label_file(30000, 20, 1, default_path + "13-");
		generate_label_file(2000, 20, 1, default_path + "14-");
		generate_label_file(6001171, 20, 1, default_path + "15-");
		generate_label_file(200000, 20, 1, default_path + "16-");
		generate_label_file(2556, 20, 1, default_path + "17-");
		*/
		/*30000 customer.tbl
		2556 date.tbl
		6001171 lineorder.tbl
		200000 part.tbl
		2000 supplier.tbl
		6235727 总用量
		*/


		//----------------------generating the missing information------------------------------
		//TPC-H
		/*
		generate_miss_rate_file(5, dim[0], 0.2, default_path + "\\0.m");
		generate_miss_rate_file(25, dim[1], 0.2, default_path + "\\1.m");
		generate_miss_rate_file(200000, dim[2], 0.2, default_path + "\\2.m");
		generate_miss_rate_file(10000, dim[3], 0.2, default_path + "\\3.m");
		generate_miss_rate_file(800000, dim[4], 0.2, default_path + "\\4.m");
		generate_miss_rate_file(150000, dim[5], 0.2, default_path + "\\5.m");
		generate_miss_rate_file(1500000, dim[6], 0.2, default_path + "\\6.m");
		generate_miss_rate_file(6001215, dim[7], 0.2, default_path + "\\7.m");


		//DBLP
		generate_miss_rate_file(1049866, dim[8], 0.2, default_path + "\\8.m");

		//Crash
		//generate_miss_rate_file(322450, 20, 0.1, default_path + "\\9.m");


		//World
		generate_miss_rate_file(4079, dim[10], 0.2, default_path + "\\10.m");
		generate_miss_rate_file(239, dim[11], 0.2, default_path + "\\11.m");
		generate_miss_rate_file(984, dim[12], 0.2, default_path + "\\12.m");

		//SSB

		generate_miss_rate_file(30000, dim[13], 0.2, default_path + "\\13.m");
		generate_miss_rate_file(2000, dim[14], 0.2, default_path + "\\14.m");
		generate_miss_rate_file(6001171, dim[15], 0.2, default_path + "\\15.m");
		generate_miss_rate_file(200000, dim[16], 0.2, default_path + "\\16.m");
		generate_miss_rate_file(2556, dim[17], 0.2, default_path + "\\17.m");
		*/


		// 读入查询，输出查询语句的信息
		//readin_price();											//读入每个表对应价格，即该表中每个tuple的基本价格
		//output_dataset();															//输出数据集的信息
		//std::cout<<"Have read query, price, and table."<<endl;


		//Get_complete rate Phi used to compute price;

		phi = 1 - g_missing_rate;// the missing rate is just the rate we used to generate the missing information


		//****************************************Algorithm comment*************************************//
		/*
		Group I: history-oblivious type, including exact and approximate algorithms with two strategy: eager and lazy.
		Algorithm1: Exact with Eager strategy (Exact-E)
		Algorithm2: Exact with Lazy strategy (Exact-L)
		Algorithm3: Approx with Eager strategy (Approx-E)
		Algorithm4: Approx with Lazy strategy (Approx-L)
		Algorithm5: PApprox with Eager Strategy (PApprox-E)
		Algorithm6: PApprox with Lazy Strategy (PApprox-L)

		Group I: history-aware type, including exact and approximate algorithms with two strategy: eager and lazy.
		Algorithm7: Exact with Eager strategy (Exact-H-E)
		Algorithm8: Exact with Lazy strategy (Exact-H-L)
		Algorithm9: Approx with Eager strategy (Approx-H-E)
		Algorithm10: Approx with Lazy strategy (Approx-H-L)
		Algorithm11: PApprox with Eager Strategy (PApprox-H-E)
		Algorithm12: PApprox with Lazy Strategy (PApprox-H-L)
		*/

		// -------------------------------history-aware pricing problem----------------------------------------------
		if (true)
		{
			// the history-oblivious price should be derived separately...
			token.clear();
			history_price = 0;
			no_history_price = 0;
			std::cout << "**********************************************************" << endl;
			std::cout << "The size of token is: " << token.size() << endl;
			output_result("The size of token is: ");
			output_number(token.size());
			for (int i = 0; i < g_queryNumber; i++)
			{
				// reset parameters
				T.tuple.clear();
				T.tupleNumber = 0;
				g_price = 0;
				g_quality = 0;

				A.attrNumber = 0;
				A.attrName.clear();
				table.clear(); //在读取了多个表的情况下进行映射。
				attrSet.clear();
				selectIndex.clear();
				projectionIndex.clear();
				nullSet.clear(); //查找起来比较方便的容器

				// set a new query
				type = HQ[i].queryType;
				Q = HQ[i];
				std::cout << "The Query i = " << i << ",type = " << HQ[i].queryType << ", Q.joinnum = " << Q.joinNumber << endl;
				g_label = i;//--------------how to generate the label;
				std::cout << "---------------------------------------------------------------" << endl;
				std::cout << "Processing Eager Strategy..." << endl;
				clock_t starttime = clock();
				EagerStrategy();// it only gets the lineage set for each result tuple
				clock_t endtime = clock();
				output_result("Time used for Eager Strategy: ");
				output_number(endtime - starttime);
				std::cout << "Time used for Eager: " << endtime - starttime << endl;
				output_result("Result set size: ");
				output_number(T.tupleNumber);
				std::cout << "Result set size:  " << T.tupleNumber << endl;
				int sizeofm = 1;
				for (int i = 0; i < T.tupleNumber; i++)
				{
					sizeofm *= T.tuple[i].L.size();
				}
				output_result("The max number of query lineage set: ");
				output_number(sizeofm);
				std::cout << "The max number of query lineage set: " << sizeofm << endl;
				//algorithms using eager...

				std::cout << "---------------------------------------------------------------" << endl;
				std::cout << "Starting the approximate algorithm.." << endl;
				HistoryApprox(phi);
				history_price += g_price;
				Approx(phi);
				std::cout << "**********************************************************" << endl;
				std::cout << "The current history-oblivious price is: " << no_history_price << endl;
				std::cout << "The current history-aware price  is: " << history_price << endl;
				output_result("The current history-oblivious price is: ");
				output_number(no_history_price);
				output_result("The current history-aware price is: ");
				output_number(history_price);
				std::cout << "**********************************************************" << endl;
				std::cout << "The current size of token is: " << token.size() << endl;
				output_result("The current size of token is: ");
				output_number(token.size());

				//history_price += g_price;

			}
			std::cout << "**********************************************************" << endl;
			std::cout << "The final history-aware price is " << history_price << endl;
			std::cout << "The final history-ovlivious price is " << history_price << endl;
			output_result("The final history-oblivious price is: ");
			output_number(no_history_price);
			output_result("The final history-aware price is: ");
			output_number(history_price);
			std::cout << "**********************************************************" << endl;
			std::cout << "The final size of token is: " << token.size() << endl;
			output_result("The final size of token is: ");
			output_number(token.size());
		}


		// -------------------------------history-oblivious pricing problem----------------------------------------------

		if (false)// using Eager
		{
			std::cout << "---------------------------------------------------------------" << endl;
			std::cout << "Processing Eager Strategy..." << endl;
			clock_t starttime = clock();
			EagerStrategy();// it only gets the lineage set for each result tuple
			clock_t endtime = clock();
			output_result("Time used for Eager Strategy: ");
			output_number(endtime - starttime);
			std::cout << "Time used for Eager: " << endtime - starttime << endl;
			output_result("Result set size: ");
			output_number(T.tupleNumber);
			std::cout << "Result set size:  " << T.tupleNumber << endl;
			int sizeofm = 1;
			for (int i = 0; i < T.tupleNumber; i++)
			{
				sizeofm *= T.tuple[i].L.size();
			}
			output_result("The max number of query lineage set: ");
			output_number(sizeofm);
			std::cout << "The max number of query lineage set: " << sizeofm << endl;
			//algorithms using eager...

			std::cout << "---------------------------------------------------------------" << endl;
			std::cout << "Starting the approximate algorithm.." << endl;
			Approx(phi);

			std::cout << "Starting the progressive approximate algorithm.." << endl;
			ProgressiveApprox(phi);

			/*
			// parameters are reset.
			std::cout << "---------------------------------------------------------------" << endl;
			std::cout << "Processing Lazy Strategy..." << endl;
			starttime = clock();
			LazyStrategy();// T is the result tuples of the query.
			endtime = clock();
			output_result("Time used for Lazy Strategy: ");
			output_number(endtime - starttime);
			std::cout << "Time used for Lazy: " << endtime - starttime << endl;
			std::cout << "Starting Exact algorithm.." << endl;
			Exact(phi);

			*/







		}

		if (false) // using Lazy
		{



			//output_result("Result set size: ");
			//output_number(T.tupleNumber);
			int sizeofm = 1;
			for (int i = 0; i < T.tupleNumber; i++)
			{
				sizeofm *= T.tuple[i].L.size();
			}
			output_result("The max number of query lineage set: ");
			output_number(sizeofm);
			std::cout << "The max number of query lineage set: " << sizeofm << endl;






			//std::cout << "Staring Exact algorithm.." << endl;
			//Exact(phi);

			// algorithms using Lazy...






		}





		/* for the following algorithms, we assume that
		(i) there are a series of queries(e.g., S queries) required to perform from a consumer,
		(ii) and some tuples in these tables have been probably updated for each query.
		Hence, the input query file should be contain multiple queries, e.g., 20.
		In addition, we should generate a small fraction of updated tuples via labeling 1-100, for each of the S queries. */
		//TODO: xucheng, 默认设置了10

		/*
		int S = 10; //the number of queries
		std::cout << "Staring the history-aware pricing algorithm.." << endl;
		vector<Tuple> History; // initialized as an empty set
		for (int i = 0; i < S; i++)
		{
		HistoryExactEager(phi, History);
		}
		//parameters are reset
		//HistoryExactLazy(phi); just replace EagerStrategy with LazyStrategy in HistoryExactEager(phi, History);
		//parameters are reset

		History.clear(); // initialized as an empty set
		for (int i = 0; i < S; i++)
		{
		HistoryApproxEager(phi, History);
		}
		//parameters are reset
		//HistoryApproxLazy(phi); just replace EagerStrategy with LazyStrategy in HistoryApproxEager(phi, History);
		//parameters are reset

		//vector<string> History; // initialized as an empty set
		History.clear();
		for (int i = 0; i < S; i++)
		{
		HistoryProgressiveApproxEager(phi, History);
		}
		std::cout << "Have finished all the pricing algorithms.." << endl;
		//parameters are reset
		//HistoryPApproxLazy(phi); just replace EagerStrategy with LazyStrategy in HistoryPApproxEager(phi, History);
		//parameters are reset


		//clock_t eagerstart, lazystart, lazy, eager;
		//double lazytime, eagertime;

		*/
		output.close();

		return 0;
	}
};





