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

Object Init(Env env, Object exports)
{
	exports.Set(String::New(env, "queryPrice"), Function::New(env, queryPrice));
	return exports;
}

NODE_API_MODULE(queryPricer, Init)