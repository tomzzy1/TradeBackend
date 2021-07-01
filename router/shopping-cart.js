import Router from 'koa-router'
import pool from '../sql/pool.js'
import {parsePostData, parseQueryStr} from '../utils/parsePostData.js'

let cart = new Router({
    prefix: '/cart'
})

cart.get('/list', async (ctx) => {
    console.log(ctx.query)
    let params = ctx.query
    let page = params.page
    let limit = params.limit
    let [goods, _] = await pool.query("SELECT s.id, g.name, g.date, g.price, s.number FROM shopping_cart s JOIN goods g ON s.id = g.id")
    let length = goods.length
    goods = goods.filter((value, index) => index < limit * page && index >= limit * (page - 1))
    console.log(goods)
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
})

export default cart