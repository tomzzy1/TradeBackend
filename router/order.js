import Router from 'koa-router'
import pool from '../sql/pool.js'
import {parsePostData, parseQueryStr} from '../utils/parsePostData.js'

let order = new Router({
    prefix: '/order'
})

order.get('/list', async (ctx) => {
    console.log('order list')
    console.log(ctx.query)
    let params = ctx.query
    let page = params.page
    let limit = params.limit
    
    let [oc, _0] = await pool.query("SELECT id FROM orders")
    let id = oc.length
    let [goods, _1] = await pool.query("SELECT g.name, o.query, o.price, o.date FROM order_contents o JOIN \
    goods g ON o.dataset_id = g.id WHERE o.order_id = ?", id)
    goods = goods.filter((value, index) => index < limit * page && index >= limit * (page - 1))
    console.log("=====After filter=====")
    console.log(goods)
    let length = goods.length
    let total_prices = 0
    for (let i = 0; i < goods.length; ++i)
    {
        total_prices += goods[i].price
    }
    await pool.query("UPDATE orders SET total_price = ? WHERE id = ?", [total_prices, id])
    console.log(goods)
    ctx.body = {
        code: 20000,
        data: 
        {
            total_prices,
            items: goods,
            total: length
        }
    }
}).post('/cancel', async (ctx) => {
    //let postData = await parsePostData(ctx)
    //let parsedData = JSON.parse(postData)
    //console.log(parsedData)
    let [oc, _0] = await pool.query("SELECT id FROM orders")
    let id = oc.length
    await pool.query("UPDATE orders SET state = 1 WHERE id = ?", id)
    ctx.body = {
        code: 20000
    }
}).post('/complete', async (ctx) => {
    //let postData = await parsePostData(ctx)
    //let parsedData = JSON.parse(postData)
    let [oc, _0] = await pool.query("SELECT id FROM orders")
    let id = oc.length
    await pool.query("UPDATE orders SET state = 2 WHERE id = ?", id)
    ctx.body = {
        code: 20000
    }
})

export default order