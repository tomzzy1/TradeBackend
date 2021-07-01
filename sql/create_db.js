import mysql from 'mysql2'

let mysqlPool = mysql.createPool({
    user: "root",
    password: "12345678",
    database: "user_schema",
})

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

for (let i = 0; i < 10; i += 1)
{
    let country = ['CHINA','USA','UK','JAPAN','RUSSIA','AUSTRALIA']
    let source = ['-a-','-b-','-c-','-d-','-e-','-f-']
    let time = ['2009','2010','2011','2012','2013','2014']
    let length = country.length
    console.log("======")
    console.log(i%6)
    mysqlPool.query("INSERT IGNORE INTO dataset VALUES(?, 'ZJU', 'testing dataset', '1GB', 'ZJU Data Center', '2021')", [i,country[i%6],'testing dataset' + String(i),String(i)+'GB',source[i%6],time[i%6]], (err, results, fields) => {
        console.log(results)
        if (err)
            console.log(err)
    })
}

// for (let i = 0; i < 100; i++)
// {
//     mysqlPool.query("INSERT IGNORE INTO dataset VALUES(?)")
// }

// mysqlPool.query("SELECT * FROM shopping_cart", (err, results, fields) =>{
//     console.log(results)
//     console.log(results[5].id)
//     if (err)
//         console.log(err)
// })
