import Router from 'koa-router'
import pool from '../sql/pool.js'
import {parsePostData, parseQueryStr} from '../utils/parsePostData.js'


let gallery = new Router({
    prefix: '/gallery'
})

gallery.get('/list', async (ctx) => {
    let params = ctx.query
    let pagenum = params.page
    let limit = params.limit
    // console.log("=====Checking Params=====")
    // console.log(pagenum)
    // console.log(limit)
    
    // console.log("=====Connect Successfully!=====")
    let [dss, _] = await pool.query("SELECT g.id, d.description, g.date, g.name, d.source, d.size FROM goods g JOIN goods_detail d ON g.id=d.id")
    let length = dss.length 
    dss = dss.filter((value, index) => index < limit * pagenum && index >= limit * (pagenum - 1))
    console.log("=====processing query!=====")
    console.log(dss)

    ctx.body = {
        code: 20000,
        data: 
        {
            total: length,
            items: dss
        }
    }

}).post('/addtoCart', async (ctx) => {
    console.log('processing login')
    let postData = await parsePostData(ctx)
    let parsedData = JSON.parse(postData)
    console.log(parsedData)
    let [old_num] = await pool.query("SELECT number FROM shopping_cart s WHERE s.id=?", parsedData.ID)
    if (old_num != 0)
    {
        await pool.query("UPDATE shopping_cart s SET number=? WHERE s.id=?", [old_num + 1, parsedData.ID])
    }
    else
    {
        await pool.query("INSERT INTO shopping_cart VALUES(?, ?, ?, ?)", [parsedData.ID, 1, parsedData.Query, parsedData.Time])
    }
    
})

export default gallery