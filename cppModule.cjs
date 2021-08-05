
let cppModule = require('bindings')('cppModule.node')
//let cppModule = require("./build/Release/cppModule.node")
//queryPricer.queryPrice(["games", "games_details", "players", "ranking", "teams"]
//,'SELECT * FROM teams t, players p WHERE t.TEAM_ID = p.TEAM_ID')
//cppModule.generate("NBA", ["games", "games_details", "players", "ranking", "teams"], 0.2)
//cppModule.getQuestionInfo("NBA", ["games", "games_details", "players", "ranking", "teams"])

module.exports = {cppModule}