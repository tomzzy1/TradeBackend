import Router from 'koa-router'
import pool from '../sql/pool.js'
import {performance} from 'perf_hooks'
import {parsePostData, parseQueryStr} from '../utils/parsePostData.js'
import {cppModule} from '../cppModule.cjs'

let setting = new Router({
    prefix: '/setting'
})

setting.get('/list', async (ctx) => {
    console.log(ctx.query)
    let params = ctx.query
    let page = params.page
    let limit = params.limit
    let [goods, _] = await pool.query("SELECT g.id, g.name, d.description, d.size, d.source, g.date FROM goods g JOIN \
    goods_detail d on g.id = d.id")
    //console.log(goods)
    if (params.searching_content !== undefined)
    {
        goods = goods.filter((value) => value[params.search] == params.searching_content)
    }
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
}).get('/post_query', async (ctx) => {
    let query = ctx.query.query
    let id = ctx.query.id
    let methods = []
    let name = ["QUCA price", "UCA price", "UP price"]
    for (let i = 0; i < 3; ++i)
    {
        let [ds_name, _0] = await pool.query('SELECT name, price FROM goods WHERE id = ?', id)
        let [tb_names, _1] = await pool.query('SELECT name FROM ' + ds_name[0].name)
        tb_names = tb_names.map((value) => value.name)
        let [params, _3] = await pool.query("SELECT price_coefficient, sensitivity_degree FROM goods_detail \
        WHERE id = ?", id)
        let begin = performance.now()
        let price = //params[0].price_coefficient * 
            cppModule.queryPrice(ds_name[0].name, tb_names, query, [Number(params[0].price_coefficient), Number(params[0].sensitivity_degree), 
            Number(ds_name[0].price)], i)
        let end = performance.now()
        methods.push({name: name[i], time: end - begin, price, param1: params[0].price_coefficient, param2: params[0].sensitivity_degree})
        console.log(methods)
    }
  
    ctx.body = {
        code: 20000,
        data:
        {
            items: methods
        }
    }

}).post('/post_params', async (ctx) => {
    let postData = await parsePostData(ctx)
    let parsedData = JSON.parse(postData)
    console.log(parsedData)
    await pool.query("UPDATE goods_detail SET price_coefficient = ?, sensitivity_degree = ? WHERE id = ?", 
    [parsedData.price_coefficient, parsedData.sensitivity_degree, parsedData.id])
    ctx.body = 
    {
        code: 20000
    }
}).post('/post_data', async (ctx) => {
    let postData = await parsePostData(ctx)
    let parsedData = JSON.parse(postData)
    console.log(parsedData)
    await pool.query("UPDATE goods_detail SET strategy = ? WHERE id = ?", [parsedData.strategy, parsedData.id])
    ctx.body = 
    {
        code: 20000
    }
})

export default setting