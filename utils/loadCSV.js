import pool from './sql/pool.js'
import fs from 'fs'
import path from 'path'
import readline from 'readline'

async function loadCSVIntoDataset(filePath)
{

//let filePath = './archive/games.csv'
let filename = filePath.split('/')
let dbname = (filename[filename.length - 1].split('.'))[0]
console.log(dbname)
const fileStream = fs.createReadStream(filePath);
const rl = readline.createInterface({
  input: fileStream,
  crlfDelay: Infinity
});
// Note: we use the crlfDelay option to recognize all instances of CR LF
// ('\r\n') in input.txt as a single line break.
for await (const line of rl) {
  // Each line in input.txt will be successively available here as `line`.
  let column_names = line.split(',')
  let query = "CREATE TABLE IF NOT EXISTS " + dbname + " ("
  let query_line = []
  for (const col of column_names) {
    query_line.push(col + " " + "VARCHAR(64)")
  }
  query += query_line.join() + ')'
  console.log(query)
  await pool.query(query) 
  break
}

let insert_csv = "LOAD DATA LOCAL INFILE 'path'\
 INTO TABLE " + dbname + 
" FIELDS TERMINATED BY ','\
 ENCLOSED BY '\"'\
 LINES TERMINATED BY '\\n'\
 IGNORE 1 ROWS"

console.log(insert_csv)

pool.query({
  sql: insert_csv,
  infileStreamFactory: path => fs.createReadStream(filePath)
})

let [table, _] = await pool.query("SELECT * FROM " + dbname)

console.log(table)
}

//loadCSVIntoDataset('./archive/teams.csv')

export default loadCSVIntoDataset