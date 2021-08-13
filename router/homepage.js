import Router from 'koa-router'
import pool from '../sql/pool.js'

let homepage = new Router({
    prefix: '/homepage'
})

function count_if(array, func)
{
    let count = 0
    for (let i of array)
    {
        if (func(i))
            count += 1
    }
    return count
}

homepage.get('/admin', async (ctx) => {
    let [admin_count, _0] = await pool.query("SELECT COUNT(1) AS count FROM login_count WHERE role = 'admin'")
    console.log(admin_count)
    let strategy_count = []
    for (let i = 0; i < 3; ++i)
    {
        let [counts, _1] = await pool.query("SELECT COUNT(strategy) AS count FROM goods_detail WHERE strategy = ?", i)
        //console.log(counts)
        strategy_count.push(counts[0].count)
    }
    let [logins, _2] = await pool.query("SELECT role, date FROM login_count")
    let now = new Date()
    let role = ['buyer', 'seller', 'worker']
    let barchart = []
    let [shoppings, _3] = await pool.query("SELECT DISTINCT COUNT(od.dataset_id) AS count FROM order_contents od \
JOIN orders o ON od.order_id = o.id WHERE o.state = 2")
    for (let i = 0; i < 7; ++i)
    {
        let counts = {}
        for (let j = 0; j < 3; ++j)
        {
            counts[role[j]] = count_if(logins, (value) => {
                let date = new Date(value.date)
                return value.role == role[j] && (now - date) < 1000 * 3600 * 24 * 7 && (date.getDay() == i)
            })
        }
        barchart.push(counts)
    }
    console.log(strategy_count)
    console.log(barchart)
    
    let [datasets, _4] = await pool.query("SELECT COUNT(1) AS count FROM goods")
    console.log(datasets)
    let [orderCompleted, _5] = await pool.query("SELECT COUNT(1) AS count FROM orders WHERE state = 2")
    console.log(orderCompleted)
    ctx.body = 
    {
        code : 20000,
        data: {
            items: {
                visits: admin_count[0].count,
                datasets: datasets[0].count,
                purchases: orderCompleted[0].count,
                strategies: {UP: strategy_count[2], UCP: strategy_count[1], QUCA: strategy_count[0]},
                shoppings: shoppings[0].count,
                barchart
            }
        }
    }
}).get('/buyer', async (ctx) => {
    let [buyer_count, _0] = await pool.query("SELECT COUNT(1) AS count FROM login_count WHERE role = 'buyer'")
    console.log(buyer_count)
    let [orderCompleted, _1] = await pool.query("SELECT COUNT(1) AS count FROM orders WHERE state = 2")
    console.log(orderCompleted)
    let barchart = []
    let [details, _2] = await pool.query("SELECT date FROM detail_count")
    let [orders, _3] = await pool.query("SELECT date FROM orders WHERE state = 2")
    let now = new Date()
    for (let i = 0; i < 7; ++i)
    {
        let counts = {}
        counts.visited = count_if(details, (value) => {
            let date = new Date(value.date)
            return (now - date) < 1000 * 3600 * 24 * 7 && (date.getDay() == i)
        })
        counts.ordered = count_if(orders, (value) => {
            let date = new Date(value.date)
            return (now - date) < 1000 * 3600 * 24 * 7 && (date.getDay() == i)
        })
        barchart.push(counts)
    }
    let [table, _4] = await pool.query("SELECT g.name AS dataset, od.price, od.query FROM orders o, order_contents od, goods g \
WHERE o.id = od.order_id AND od.dataset_id = g.id AND o.state = 2")
    ctx.body = {
        code : 20000,
        data: {
            items: {
                visits: buyer_count[0].count,
                orders: orderCompleted[0].count,
                barchart,
                table
            }
        }
    }
}).get('/seller', async (ctx) => {
    let [seller_count, _0] = await pool.query("SELECT COUNT(1) AS count FROM login_count WHERE role = 'seller'")
    console.log(seller_count)
    let [datasets_count, _1] = await pool.query("SELECT COUNT(1) AS count FROM goods")
    console.log(datasets_count)
    let [orderCompleted, _2] = await pool.query("SELECT COUNT(1) AS count FROM orders WHERE state = 2")
    console.log(orderCompleted)
    let [shoppings, _3] = await pool.query("SELECT DISTINCT COUNT(od.dataset_id) AS count FROM order_contents od \
JOIN orders o ON od.order_id = o.id WHERE o.state = 2")
    let barchart = []
    let [complements, _4] = await pool.query("SELECT date, need_complement FROM complement_count")
    let now = new Date()
    let key = ['completed', 'original']
    for (let i = 0; i < 7; ++i)
    {
        let counts = {}
        for (let j = 0; j < 2; ++j)
        {
            counts[key[j]] = count_if(complements, (value) => {
                let date = new Date(value.date)
                return value.need_complement == j && (now - date) < 1000 * 3600 * 24 * 7 && (date.getDay() == i)
            })
        }
        barchart.push(counts)
    }
    let [orders, _5] = await pool.query("SELECT total_price, state FROM orders")
    let table = orders.map((value) => {return {buyer: "buyer", price: value.total_price, status: value.state}})
    console.log(table)
    console.log(barchart)
    ctx.body = {
        code : 20000,
        data: {
            items: {
                visits: seller_count[0].count,
                datasets: datasets_count[0].count,
                purchases: orderCompleted[0].count,
                shoppings: shoppings[0].count,
                barchart,
                table
            }
        }
    }
}).get('/worker', async (ctx) => {
    let [answer_count, _0] = await pool.query("SELECT DISTINCT COUNT(q.dataset_id) AS count FROM answers a JOIN question q ON a.question_id = q.id")
    let [total, _1] = await pool.query("SELECT COUNT(1) FROM goods")
    let answered = answer_count[0].count
    let unaswered = total[0].count - answered
    let barchart = []
    let [dates, _2] = await pool.query("SELECT date FROM answers")
    let now = new Date()
    for (let i = 0; i < 7; ++i)
    {
        let count = count_if(dates, (value) => {
                let date = new Date(value.date)
                return (now - date) < 1000 * 3600 * 24 * 7 && (date.getDay() == i)
            })
        barchart.push(count)
    }
    let [table, _3] = await pool.query("SELECT SUM(a.point) AS point, g.name AS dataset \
FROM answers a, question q, goods g WHERE a.question_id = q.id AND q.dataset_id = g.id GROUP BY q.dataset_id")
    ctx.body = {
        code : 20000,
        data: {
            items: {
                answered,
                unaswered,
                pieChart: {answered, unaswered},
                barchart,
                table
            }
        }
    }
})
 

export default homepage