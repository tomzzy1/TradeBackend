import Router from 'koa-router'
import pool from '../sql/pool.js'
import {parsePostData} from '../utils/parsePostData.js'

let post = new Router({
    prefix: '/post_question'
})

post.get('/list', async (ctx) => {
    console.log(ctx.query)
    let params = ctx.query
    let page = params.page
    let limit = params.limit
    let [goods, _] = await pool.query("SELECT g.id, g.name, d.description FROM goods g JOIN \
    goods_detail d on g.id = d.id WHERE g.need_complement = 2")
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
}).get('/info', async (ctx) => {
    let [info, _] = await pool.query("SELECT * FROM question_info WHERE id <= 2 AND dataset_id = " + ctx.query.id)
    ctx.body = {
        code: 20000,
        data:
        {
            items: info
        }
    }

}).post('/post', async (ctx) => {
    let postData = await parsePostData(ctx)
    let parsedData = JSON.parse(postData)
    console.log(parsedData)
    for (let p of parsedData)
    {
        await pool.query("INSERT INTO question (id, question, description, answerNumber, \
            answers, correctAnswer, point, dataset_id) VALUES(?, ?, ?, ?, ?, ?, ?, ?)", [p.id, p.question, p.description, 
            p.answerNumber, p.answers, p.correctAnswer, p.point, p.dataset_id])
    }
    ctx.body = 
    {
        code: 20000
    }
})

export default post