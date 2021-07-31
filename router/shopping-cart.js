import Router from 'koa-router'
import pool from '../sql/pool.js'
import {parsePostData, parseQueryStr} from '../utils/parsePostData.js'

let cart = new Router({
    prefix: '/cart'
})

let order_id = -1

cart.get('/list', async (ctx) => {
    console.log(ctx.query)
    let params = ctx.query
    let page = params.page
    let limit = params.limit
    let [goods, _] = await pool.query("SELECT s.id, g.name, s.query, s.price, s.date FROM \
    shopping_cart s JOIN goods g ON s.dataset_id = g.id")
    //console.log(goods)
    if (params.searching_content !== undefined)
    {
        goods = goods.filter((value) => value[params.search] == params.searching_content)
    }
    let length = goods.length
    goods = goods.filter((value, index) => index < limit * page && index >= limit * (page - 1))
    console.log(goods)
    for (let i = 0; i < length; ++i)
    {
        goods[i].checked = false
    }
    ctx.body = {
        code: 20000,
        data: 
        {
            items: goods,
            total: length
        }
    }

}).post('/update', async (ctx) => {
    let postData = await parsePostData(ctx)
    console.log("=====testing the connectivity of backend and platform !! =====")
    if (!postData)
        console.log('error!')
    let parsedData = JSON.parse(postData)
    console.log(parsedData)
    let id = parsedData.id
    let new_num = parsedData.num
    console.log(id, new_num)
    pool.query("UPDATE shopping_cart SET number=? WHERE id=?", [new_num, id], (err, result, field)=>{
        console.log("The number of ID: {0} is updated to {1}".format(id, new_num))
    })
    ctx.body = 
    {
        code: 20000
    }
}).post('/delete', async (ctx) => {
    let postData = await parsePostData(ctx)
    let parsedData = JSON.parse(postData)
    console.log(parsedData)
    let id = parsedData
    let result = await pool.query("DELETE FROM shopping_cart WHERE id=?", id)
    console.log(result)
    ctx.body = 
    {
        code: 20000
    }

}).post('/add', async (ctx) => {
    let postData = await parsePostData(ctx)
    let parsedData = JSON.parse(postData)
    console.log(parsedData)
    let id = parsedData['id']
    pool.query("INSERT INTO shopping_cart (id, number) VALUES(?, ?)", [id, 1], (err, result, field)=>{
        console.log(result)
    })
    ctx.body = 
    {
        code: 20000
    }
}).post('/buy', async (ctx, next) => {
    console.log('processing buying dataset')
    let postData = await parsePostData(ctx)
    let parsedData = JSON.parse(postData)
    console.log("parsedData", parsedData)
    let [result, _0] = await pool.query("INSERT IGNORE INTO orders (date, state) VALUES(NOW(), 0)")
    console.log('result:')
    console.log(result)
    order_id = result.insertId
    console.log('buy id:', order_id)
    for (let id of parsedData)
    {
        console.log("id:", id)
        let [cart, _1] = await pool.query("SELECT query, date, dataset_id, price FROM shopping_cart WHERE \
        id = ?", id)
        let goods = cart[0]
        console.log("cart", cart)
        await pool.query("INSERT INTO order_contents (query, date, dataset_id, price, order_id) \
        VALUES(?, ?, ?, ?, ?)", [goods.query, goods.date, goods.dataset_id, goods.price, result.insertId])
    }
    ctx.body = 
    {
        code: 20000
    }
}).get('/get_order_id', async (ctx) => {
    console.log('return order id')
    //console.log(ctx.state)
    console.log('get order id:', order_id)
    ctx.body = 
    {
        code: 20000,
        data: {
            order_id: order_id
        }
    }
})

export default cart