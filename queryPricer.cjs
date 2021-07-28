
let queryPricer = require('bindings')('queryPricer.node')

//queryPricer.queryPrice(["games", "games_details", "players", "ranking", "teams"]
//,'SELECT * FROM teams t, players p WHERE t.TEAM_ID = p.TEAM_ID')

module.exports = {queryPricer}