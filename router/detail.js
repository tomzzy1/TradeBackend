import Router from 'koa-router'
import pool from '../sql/pool.js'
import {parsePostData, parseQueryStr} from '../utils/parsePostData.js'
import {queryPricer} from '../queryPricer.cjs'

//let queryPricer = require('bindings')('queryPricer.node')
//import queryPricer from '../build/Release/queryPricer'

let detail = new Router({
    prefix: '/detail'
})

detail.get('/list', async (ctx) => {
    console.log(ctx.query)
    let id = ctx.query.id
    console.log(id)
    let table_info = []
    let [ds_name, _0] = await pool.query('SELECT name FROM goods WHERE id = ?', id)
    console.log(ds_name[0].name)
    let [tb_names, _1] = await pool.query('SELECT name FROM ' + ds_name[0].name)
    console.log(tb_names)
    for (let i = 0; i < tb_names.length; ++i)
    {
        let [columns, _2] = await pool.query('SHOW COLUMNS FROM ' + tb_names[i].name)
        columns = columns.map((value) => value.Field)
        console.log(columns)
        let types = columns.map(() => 'string')
        table_info.push({columns, table: tb_names[i].name, types})
    }
    let [desc, _3] = await pool.query('SELECT description FROM goods_detail WHERE id = ?', id)
    //console.log(description)
    console.log(table_info)
    ctx.body = {
        code: 20000,
        data:
        {
            table_info,
            description: desc.description
        }
    }
}).get('/check', async (ctx) => {
    console.log(ctx.query)
    ctx.body = {
        code: 20000,
        data: {
            status: queryPricer.checkSQL(ctx.query.query)
        }
    }
}).post('/add_to_cart', async (ctx) => {
    let postData = await parsePostData(ctx)
    let parsedData = JSON.parse(postData)
    let [ds_name, _0] = await pool.query('SELECT name, price FROM goods WHERE id = ?', parsedData.id)
    console.log(ds_name[0])
    let [tb_names, _1] = await pool.query('SELECT name FROM ' + ds_name[0].name)
    tb_names = tb_names.map((value) => value.name)
    console.log(parsedData)
    let price = queryPricer.queryPrice(tb_names, parsedData.query, [0.5, 0.5, ds_name[0].price])
    if (parsedData.complement)
        price += 100
    let [dummy, _2] = await pool.query("INSERT IGNORE INTO shopping_cart (number, query, date, dataset_id,\
        price) VALUES(1, ?, NOW(), ?, ?)", [parsedData.query, parsedData.id, price])
    ctx.body = {
        code: 20000,
        dummy
    }

})

function UP(query) {
    
}

function UCP() {

}

export default detail