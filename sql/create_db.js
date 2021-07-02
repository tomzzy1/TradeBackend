import mysql from 'mysql2'

let mysqlPool = mysql.createPool({
    user: "root",
    password: "123456",
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

// for (let i = 0; i < 100; i += 1)
// {
//     let country = ['CHINA','USA','UK','JAPAN','RUSSIA','AUSTRALIA']
//     let source = ['-a-','-b-','-c-','-d-','-e-','-f-']
//     let time = ['2009','2010','2011','2012','2013','2014']
//     let length = country.length
//     console.log("======")
//     console.log(i%6)
//     mysqlPool.query("INSERT IGNORE INTO dataset VALUES(?, ?, ?, ?, ?, ?)", [i,country[i%6],'testing dataset' + String(i),String(i)+'GB',source[i%6],time[i%6]], (err, results, fields) => {
//         console.log(results)
//         if (err)
//             console.log(err)
//     })
// }

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

/* Generate random incomplete dataset for crowdsourcing testing */
    /* Step1: Generate a complete version */
for (let i = 0; i < 100; i += 1)
{
    let first_name = ['Zheyuan','Zhenke','Donglin','Huanhuan','Yangyang','Aaron','Abbott','Abel','Abner','Abraham']
    let last_name = ['Zhao','Qian','Sun','Li','Zhou','James','John','David','Daniel','Michael']
    let randidx1 = Math.floor(Math.random()*10)
    let randidx2 = Math.floor(Math.random()*10)
    let country = ['Native','Foreign']
    let curr_country = ''
    if (randidx2 <= 4){
        curr_country = 'Native'
    } else {
        curr_country = 'Foreign'
    }
    console.log("======")
    mysqlPool.query("INSERT IGNORE INTO namelist VALUES(?, ?, ?, ?)", [i, first_name[randidx1], last_name[randidx2], curr_country], (err, results, fields) => {
        console.log(results)
        if (err)
            console.log(err)
    })
}
    /* Step 2: Generate some incomplete tuples */
for (let i = 100; i < 110; i += 1){
    let first_name = ['Zheyuan','Zhenke','Donglin','Huanhuan','Yangyang','Aaron','Abbott','Abel','Abner','Abraham']
    let last_name = ['Zhao','Qian','Sun','Li','Zhou','James','John','David','Daniel','Michael']
    let randidx1 = Math.floor(Math.random()*10)
    let randidx2 = Math.floor(Math.random()*10)
    let country = ['Native','Foreign']
    let curr_country = ''
    if (randidx2 <= 4){
        curr_country = 'Native'
    } else {
        curr_country = 'Foreign'
    }
    console.log("======")
    mysqlPool.query("INSERT IGNORE INTO namelist VALUES(?, ?, ?, ?)", [i, first_name[randidx1], last_name[randidx2], ''], (err, results, fields) => {
        console.log(results)
        if (err)
            console.log(err)
    })
}
