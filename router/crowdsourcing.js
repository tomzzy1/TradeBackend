import Router from 'koa-router'
import pool from '../sql/pool.js'
import {parsePostData, parseQueryStr} from '../utils/parsePostData.js'

let crowdsourcing = new Router({
    prefix: '/crowdsourcing'
})
/* TODO: fetch the target dataset from goods and automatically determine the loss field */
/* TODO finised */
crowdsourcing.get('/list', async (ctx) => {
    console.log(ctx.query)
    let params = ctx.query
    let page = params.page
    let limit = params.limit
    let [goods, _] = await pool.query("SELECT g.id, g.name, d.description FROM \
    goods g JOIN goods_detail d ON d.id = g.id")
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

}).get('/answer_list', async (ctx) => {
    console.log(ctx.query)
    let [goods, _] = await pool.query("SELECT id, question, description, answers, correctAnswer, point FROM question \
    WHERE dataset_id = ?", ctx.query.id)
    //console.log(goods)
    ctx.body = {
        code: 20000,
        data: 
        {
            items: goods
        }
    }
}).post('/submit_answer', async (ctx) => {
    let postData = await parsePostData(ctx)
    let parsedData = JSON.parse(postData)
    for (let answer of parsedData)
    {
        await pool.query("INSERT IGNORE INTO answers (question_id, answer) VALUES(?, ?)", [answer.id, answer.answer])
    }
    ctx.body = {
        code: 20000
    }
})

export default crowdsourcing