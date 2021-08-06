#include "napi.h"
#include "queryPrice.h"
#include "question.h"
#include <list>
#include "mysqlx/xdevapi.h"
#include "mysqlx/common/value.h"

using namespace Napi;

Number queryPrice(const CallbackInfo& args)
{
	QueryPrice qp;
	Array arr = args[1].As<Array>();
	std::vector<std::string> paths;

	for (int i = 0; i < arr.Length(); ++i)
	{
		std::string val = arr.Get(i).ToString();
		paths.push_back(val);
	}

	Array arr2 = args[3].As<Array>();
	std::vector<float> params;
	for (int i = 0; i < arr2.Length(); ++i)
	{
		params.push_back(arr2.Get(i).As<Number>().FloatValue());
	}

	qp.load(args[0].As<String>(), paths);
	float price = -1;
	try
	{
		price = qp.query(args[2].ToString(), params, args[4].As<Number>());
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return Number::New(args.Env(), price);
}

Boolean checkSQL(const CallbackInfo& args)
{
	sqlParser parser;
	std::string sql = args[0].As<String>();
	try
	{
		parser.parse(sql);
	}
	catch(...)
	{
		//std::cout << "Exception!\n";
		//throw Error::New(args.Env(), "Fail to parse sql");
		return Boolean::New(args.Env(), false);
	}

	return Boolean::New(args.Env(), true);
	
}

void getQuestionInfo(const CallbackInfo& args)
{
	Array arr = args[1].As<Array>();
	std::vector<std::string> names;

	for (int i = 0; i < arr.Length(); ++i)
	{
		std::string val = arr.Get(i).ToString();
		names.push_back(val);
	}
	std::string ds_name = args[0].As<String>();
	int index = args[2].As<Number>();
	auto result = questionInfo(ds_name, names, index);
	using mysqlx::SessionOption;
	try
	{
	mysqlx::Session sess(SessionOption::USER, "root",
                     SessionOption::PWD, "123456",
                     SessionOption::HOST, "localhost",
                     SessionOption::PORT, 33060,
                     SessionOption::DB, "user_schema");
	sess.sql("USE user_schema").execute();
	std::cout << "session setup\n";
	std::cout << "result size " << result[0].size() << '\n';
 	auto ids = sess.sql("SELECT id FROM goods WHERE name = '" + ds_name + "'").execute();
	std::cout << "selected\n";
	auto row = ids.fetchOne();
	auto id = static_cast<int>(row[0]);
	std::cout << "id " << id << '\n';
	for (int i = 0; i < result[0].size() && i < 20; ++i)
	{
		//std::cout << i << '\n' << result[0][i] << '\n'<< result[1][i] << '\n' << result[2][i] << '\n';
		//std::cout << i << '\n';
		
		std::string sql = "INSERT IGNORE INTO question_info (id, pos, row_info, col_info, dataset_id) VALUES(" 
		+ std::to_string(i + 1 + 20 * index) + ", '" + result[0][i] 
		+ "', '"  + result[1][i]
		+ "', '" + result[2][i] + "', '" + std::to_string(id) + "')";

		sess.sql(sql).execute();
	}
	std::cout << "insert finished\n";
	return;

	} catch (const std::exception& e){
		std::cout << e.what() << '\n';
		return;
	}
}

void generate(const CallbackInfo& args)
{
	Array arr = args[1].As<Array>();
	std::vector<std::string> names;

	for (int i = 0; i < arr.Length(); ++i)
	{
		std::string val = arr.Get(i).ToString();
		names.push_back(val);
	}
	QueryPrice qp;
	qp.generate(args[0].As<String>(), names, args[2].As<Number>(), args[3].As<Number>());
}

Object Init(Env env, Object exports)
{
	exports.Set(String::New(env, "queryPrice"), Function::New(env, queryPrice));
	exports.Set(String::New(env, "checkSQL"), Function::New(env, checkSQL));
	exports.Set(String::New(env, "getQuestionInfo"), Function::New(env, getQuestionInfo));
	exports.Set(String::New(env, "generate"), Function::New(env, generate));
	return exports;
}

NODE_API_MODULE(cppModule, Init)