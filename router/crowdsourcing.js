import Router from 'koa-router'
import pool from '../sql/pool.js'
import {parsePostData, parseQueryStr} from '../utils/parsePostData.js'

let crowdsourcing = new Router({
    prefix: '/crowdsourcing'
})
/* TODO: fetch the target dataset from goods and automatically determine the loss field */
crowdsourcing.get('/list', async(ctx) => {
    let params = ctx.query
    let [q_list, _] = await pool.query("SELECT * FROM crowdsourcing")
    console.log("=====CrowdSourcing CHecking=====")
    console.log(q_list)
    ctx.body = {
        code: 20000,
        data: 
        {
            total: q_list.length, // # of questions
            items: q_list  
        }
    }
})

export default crowdsourcing