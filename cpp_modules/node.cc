#include "napi.h"
#include "queryPrice.h"

using namespace Napi;

Number queryPrice(const CallbackInfo& args)
{
	QueryPrice qp;
	Array arr = args[0].As<Array>();
	std::vector<std::string> paths;

	for (int i = 0; i < arr.Length(); ++i)
	{
		std::string val = arr.Get(i).ToString();
		paths.push_back(val);
	}

	Array arr2 = args[2].As<Array>();
	std::vector<float> params;
	for (int i = 0; i < arr2.Length(); ++i)
	{
		params.push_back(arr2.Get(i).As<Number>().FloatValue());
	}

	qp.load(paths);
	float price = -1;
	try
	{
		price = qp.query(args[1].ToString(), params);
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

Object Init(Env env, Object exports)
{
	exports.Set(String::New(env, "queryPrice"), Function::New(env, queryPrice));
	exports.Set(String::New(env, "checkSQL"), Function::New(env, checkSQL));
	return exports;
}

NODE_API_MODULE(queryPricer, Init)