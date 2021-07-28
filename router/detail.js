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
})

export default detail