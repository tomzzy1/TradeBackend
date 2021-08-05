import pool from './pool.js'
import loadCSVIntoDataset from '../utils/loadCSV.js'

//await pool.query("INSERT IGNORE INTO goods VALUES(100, 'NBA', '2000-01-02', 5000)")
//await pool.query("INSERT IGNORE INTO goods_detail VALUES(100, 'NBA data', 'USA', 1)")
let [res, _] = await pool.query("SELECT * FROM question_info")
console.log(res)
//let [res, _] = await pool.query("INSERT INTO question_info (id, pos, row_info, col_info) VALUES(1, 'FG_PCT_home', 'GAME_DATE_EST: 2021-05-26, GAME_ID: 42000102, GAME_STATUS_TEXT: Final, HOME_TEAM_ID: 1610612755, VISITOR_TEAM_ID: 1610612764, SEASON: 2020, TEAM_ID_home: 1610612755, PTS_home: 120, FG_PCT_home: undefined, FT_PCT_home: 0.684, FG3_PCT_home: 0.429, AST_home: undefined, REB_home: 45, TEAM_ID_away: 1610612764, PTS_away: 95, FG_PCT_away: 0.402, FT_PCT_away: 0.633, FG3_PCT_away: 0.091, AST_away: 22, REB_away: 40, HOME_TEAM_WINS: undefined', 'FG_PCT_home: undefined, FG_PCT_home: undefined')")
//console.log(res)
//await pool.query("TRUNCATE orders")
//await pool.query("TRUNCATE order_contents")
//await pool.query("TRUNCATE shopping_cart")
//let [result, _] = await pool.query('INSERT INTO test_table (date) VALUES(NOW())')
//console.log(result)
//let [cart, _] = await pool.query('SELECT * FROM shopping_cart')
//console.log(cart)
//await pool.query('DELETE FROM shopping_cart')
/*await pool.query('DELETE FROM NBA')
await pool.query("CREATE TABLE IF NOT EXISTS NBA (id INT, name VARCHAR(16))")
for (let i = 0; i < 5; ++i)
{
    let root = './archive/'
    let names = ['games', 'games_details', 'players', 'ranking', 'teams']
    await pool.query("INSERT IGNORE INTO NBA VALUES(?, ?)", [i, names[i]])
    loadCSVIntoDataset(root + names[i] + '.csv')
}*/

// for (let i = 0; i < 100; ++i)
// {
//     mysqlPool.query("INSERT IGNORE INTO goods VALUES(?, 'db', '2000-01-01', ?)", [i, i * 1000], (err, results, fields) =>{
//         //console.log(results)
//         if (err)
//             console.log(err)
//     })
// }

// mysqlPool.query("SELECT * FROM goods", (err, results, fields) =>{
//     console.log(results)
//     if (err)
//         console.log(err)
// })

//await pool.query('DELETE FROM goods_detail')
/*for (let i = 0; i < 100; i += 1)
{
    let country = ['CHINA','USA','UK','JAPAN','RUSSIA','AUSTRALIA']
    console.log("======")
    console.log(i%6)
    pool.query("INSERT IGNORE INTO goods_detail VALUES(?, ?, ?, ?)", [i ,'testing dataset', country[i % 6], i * i % 6], (err, results, fields) => {
        console.log(results)
        if (err)
            console.log(err)
    })
}*/


// mysqlPool.query("SELECT * FROM shopping_cart", (err, results, fields) =>{
//     console.log(results)
//     console.log(results[5].id)
//     if (err)
//         console.log(err)
// })

// /* Generate random incomplete dataset for crowdsourcing testing */
//     /* Step1: Generate a complete version */
// for (let i = 0; i < 100; i += 1)
// {
//     let first_name = ['Zheyuan','Zhenke','Donglin','Huanhuan','Yangyang','Aaron','Abbott','Abel','Abner','Abraham']
//     let last_name = ['Zhao','Qian','Sun','Li','Zhou','James','John','David','Daniel','Michael']
//     let randidx1 = Math.floor(Math.random()*10)
//     let randidx2 = Math.floor(Math.random()*10)
//     let country = ['Native','Foreign']
//     let curr_country = ''
//     if (randidx2 <= 4){
//         curr_country = 'Native'
//     } else {
//         curr_country = 'Foreign'
//     }
//     console.log("======")
//     mysqlPool.query("INSERT IGNORE INTO namelist VALUES(?, ?, ?, ?)", [i, first_name[randidx1], last_name[randidx2], curr_country], (err, results, fields) => {
//         console.log(results)
//         if (err)
//             console.log(err)
//     })
// }
//     /* Step 2: Generate some incomplete tuples */
// for (let i = 100; i < 110; i += 1){
//     let first_name = ['Zheyuan','Zhenke','Donglin','Huanhuan','Yangyang','Aaron','Abbott','Abel','Abner','Abraham']
//     let last_name = ['Zhao','Qian','Sun','Li','Zhou','James','John','David','Daniel','Michael']
//     let randidx1 = Math.floor(Math.random()*10)
//     let randidx2 = Math.floor(Math.random()*10)
//     let country = ['Native','Foreign']
//     let curr_country = ''
//     if (randidx2 <= 4){
//         curr_country = 'Native'
//     } else {
//         curr_country = 'Foreign'
//     }
//     console.log("======")
//     mysqlPool.query("INSERT IGNORE INTO namelist VALUES(?, ?, ?, ?)", [i, first_name[randidx1], last_name[randidx2], ''], (err, results, fields) => {
//         console.log(results)
//         if (err)
//             console.log(err)
//     })
// }


// Generate Crowdsourcing problems
// for (let i = 0; i < 100; i += 1)
// {
/* only works for the testing dataset 'namelist' */
// let columns_list = ['FirstName','LastName','Country']

// let question = {}
// for (let i = 0; i < columns_list.length; i +=1 ){
//     /* build a dict for different fields */
//     let query = "SELECT * FROM namelist WHERE " + columns_list[i] + "= '' "
//     let [results, _] = await pool.query(query)
//     question[columns_list[i]] = results
// }
// // console.log()
 

// /* Insert into the dataset: crowdsourcing */
// let id = 0
// let ans_list = []
// let keys = Object.keys(question)
// for (let key in keys){
//     let column = keys[key]
//     // console.log(question[column])
//     for (let i in question[column]){
//         let curr_item = question[column][i]
//         /* TODO: update the q_str */
//         let q_str = "What's the '" + column + "' of the object with following properties? "
//         let hint = "LastName"
//         q_str += (hint + " is " + curr_item[hint] + "." )
//         // console.log(q_str)
//         let answer = ""
        
//         /* filling the answer string */
//         let [res,_] = await pool.query("SELECT " + column + " FROM namelist")
//         for(let i = 0; i<res.length;i+=1){
//             if(res[i][column] == '') continue 
//             if(ans_list.indexOf(res[i][column]) != -1){
//                 // console.log("!")
//                 continue
//             }
//             ans_list.push(res[i][column]) 
//         }
//         answer = ans_list.join(";")
//         // console.log(answer)
        
//         mysqlPool.query("INSERT IGNORE INTO crowdsourcing VALUES(?, ?, ?, ?, ?, ?)", [id, q_str, hint, answer, -1, 1], (err, results, fields) => {
//             // console.log(results)
//             if (err)
//                 console.log(err)
//         })
        
        
//         id += 1
//     }

// }




// /* if the question for complete data is needed, please uncomment the following lines */
// let number = 10
// let counter = 0
// while (counter != number){
//     let [length,_] = await pool.query("SELECT COUNT(*) FROM namelist") // number of the tuples in a dataset table
//     let randid = Math.floor(Math.random()*(length-1)) // random ID for the given dataset table
//     let randcol = Math.floor(Math.random()*(columns_list.length))   // random column index for the given dataset table
//     counter += 1
// }

/* lines till here */

// let condition = ""
// let incomplete_list = []
// for (let col = 0;col<columns_list.length;col+=1){
//     if (col != 0){
//         condition += "or "
//     }
//     let curr_column = columns_list[col]
//     condition += (curr_column + " = " + "'' ")
// }
// query += condition    
// let [results, _] = await pool.query(query)
// console.log(results)

