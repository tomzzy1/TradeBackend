import mysql from 'mysql2'

let mysqlPool = mysql.createPool({
    user: "root",
    password: "123456",
    database: "user_schema",
})

for (let i = 0; i < 100; ++i)
{
    mysqlPool.query("INSERT IGNORE INTO goods VALUES(?, 'db', '2000-01-01', ?)", [i, i * 1000], (err, results, fields) =>{
        //console.log(results)
        if (err)
            console.log(err)
    })
}

mysqlPool.query("SELECT * FROM goods", (err, results, fields) =>{
    console.log(results)
    if (err)
        console.log(err)
})

for (let i = 0; i < 100; i += 10)
{
    mysqlPool.query("INSERT IGNORE INTO shopping_cart VALUES(?, 1)", i, (err, results, fields) => {
        if (err)
            console.log(err)
    })
}

mysqlPool.query("SELECT * FROM shopping_cart", (err, results, fields) =>{
    console.log(results)
    console.log(results[5].id)
    if (err)
        console.log(err)
})
