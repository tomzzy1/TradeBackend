//
//  pricing.h
//

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace std;

#ifndef pricing_h
#define pricing_h

class Tuple { //元组
public:
	vector<string> Values; //the tuples in the table
	vector<int> mState; //the tuples in the table
	int label; //label of every tuple for history-aware pricing
	vector<vector<Tuple>> L; //lineage 每个字符串表示单个元组的某一个lineage
	//vector<vector<Tuple>> Ly;
	int x; //the number of missing critical attributes of each tuple
	//int completeNum; //the number of missing critical attributes of each tuple
	//float quality;
	Tuple() :label(0), x(0) {};
	bool operator == (const Tuple & b) const {
		if (this->Values.size() != b.Values.size())
			return false;
		for (int i = 0; i < this->Values.size(); i++){
			if (!this->Values[i].compare(b.Values[i])) {
				return false;
			}
		}
		return true;
	}
	bool operator != (const Tuple &b) const {
		return !(*this == b);
	}
};

class View { // 表
public:
	int index;//used to identify the number of attributes, the price.....
	int tupleNumber; //元组个数
					 //unordered_map<string, int> attrIndex;
	vector<Tuple> tuple; //the tuples in the table
	//vector<vector<string>> label; //label of every tuple for history-aware pricing
	//vector<vector<vector<string>>> L; //lineage 每个字符串表示单个元组的某一个lineage
	//vector<int> x; //the number of missing critical attributes of each tuple
	//vector<int> complete_d; //the number of missing critical attributes of each tuple
	View() :tupleNumber(0) {};
/*	View(int m) :tupleNumber(m) {
		for (int i = 0; i < m; i++)
			u.push_back("0;");
	};
*/
};

class Query {
public:
	int joinNumber, selectNumber, projectionNumber, selectP, projectionP; //连接的表数，选择的条件数，映射的维数，选择的表号，映射的表号
	vector<int> joinIndex; //连接的表号
	vector<string> joinAttr; //外键属性
	vector<string> selectAttr; //选择条件的属性
	vector<string> selecctValue; //选择条件属性对应值
	vector<string> projectionAttr; //映射的属性
	vector<int> selectOperation; //选择的策略0: equal; 1: not equal; 2: larger than; 3: smaller than
	int queryType;
};

class Attribute {
public:
	string dataset_source;
	int attrNumber; //维数
	vector<string> attrName; //属性名
	Attribute() :attrNumber(0) {};
	Attribute(int n, vector<string> t) :attrNumber(n), attrName(t) {}
};

class Results {
public:
	View ResTuples; //result set
	Attribute ResAttrs;
	//vector<string> ResAttrs;
	float price; //query price
	vector<Tuple> Mlineage; // the lineage derived the query price
	float quality; // query quality
	float ratioApprox; // the approximate ratio of approximate algorithms

	// Function for  showing the results, write the "results" into an external file
	void ShowResult()
	{

	}

};

class AttrCollection {
public:
	AttrCollection() :_total_cnt(0){};
private:
	int _total_cnt;
	vector <Attribute> _attr_set;
};


#endif /* pricing_h */
