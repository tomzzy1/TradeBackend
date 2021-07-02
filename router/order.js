import Router from 'koa-router'
import pool from '../sql/pool.js'
import {parsePostData, parseQueryStr} from '../utils/parsePostData.js'

let order = new Router({
    prefix: '/order'
})

order.get('/list', async (ctx) => {
    console.log(ctx.query)
    let params = ctx.query
    let page = params.page
    let limit = params.limit
    let [goods, _] = await pool.query("SELECT g.name, g.price, s.number FROM shopping_cart s JOIN goods g ON s.id = g.id")
    console.log("=====Checking Goods=====")
    console.log(page)
    console.log(limit)
    console.log(goods)
    goods = goods.filter((value, index) => index < limit * page && index >= limit * (page - 1))
    console.log("=====After filter=====")
    console.log(goods)
    let length = goods.length
    let total_prices = 0
    for (let i = 0; i < goods.length; ++i)
    {
        total_prices += goods[i].number * goods[i].price
    }
    console.log(goods)
    console.log("=====Checking=====")
    console.log(total_prices)
    console.log(goods)
    console.log(length)
    ctx.body = {
        code: 20000,
        data: 
        {
            total_prices,
            items: goods,
            total: length
        }
    }
})

export default order