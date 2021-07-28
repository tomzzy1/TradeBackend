#include "napi.h"
#include "queryPrice.h"

using namespace Napi;

void queryPrice(const CallbackInfo& args)
{
	QueryPrice qp;
	Array arr = args[0].As<Array>();
	std::vector<std::string> paths;

	for (int i = 0; i < arr.Length(); ++i)
	{
		std::string val = arr.Get(i).ToString();
		paths.push_back(val);
	}
	qp.load(paths);
	qp.query(args[1].ToString());
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