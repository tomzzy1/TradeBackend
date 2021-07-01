import mysql from 'mysql2'

let pool = mysql.createPool({
    user: "root",
    password: "12345678",
    database: "user_schema",
}).promise()


pool.getConnection((err, connection)=>{
    if (err)
        console.log(err)
    else
    {
        connection.query("INSERT IGNORE INTO user (token, role) VALUES('admin-token','admin')",(err, results, fields) =>{

        console.log(results)
        if (err)
            console.log(err)
        })

        connection.query("SELECT * from user",(err, results, fields) =>{

            console.log(results)
            if (err)
                console.log(err)
            })
        connection.release()
    }
})

export default pool