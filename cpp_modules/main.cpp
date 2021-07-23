#include "queryPrice.h"

int main()
{
	QueryPrice qp;
	qp.load({ "games", "games_details", "players", "ranking", "teams" });
	qp.query("");
	return 0;
}