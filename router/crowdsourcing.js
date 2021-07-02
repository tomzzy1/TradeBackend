import Router from 'koa-router'
import pool from '../sql/pool.js'
import {parsePostData, parseQueryStr} from '../utils/parsePostData.js'

let crowdsourcing = new Router({
    prefix: '/crowdsourcing'
})
/* TODO: fetch the target dataset from goods and automatically determine the loss field */
crowdsourcing.get('/list', async(ctx) => {
    let params = ctx.query
    let [ds, _] = await pool.query("SELECT * FROM namelist where Country=''")
    console.log("=====CrowdSourcing CHecking=====")
    console.log(ds)
    ctx.body = {
        code: 20000,
        data: 
        {
            // total: length,
            items: ds
        }
    }
})

export default crowdsourcing