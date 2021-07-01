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
    let [dss, _] = await pool.query("SELECT * FROM dataset")
    let length = dss.length
    dss = dss.filter((value, index) => index < limit * pagenum && index >= limit * (pagenum - 1))
    // console.log("=====processing query!=====")
    // console.log(dss)

    ctx.body = {
        code: 20000,
        data: 
        {
            total: length,
            items: dss
        }
    }

})

export default gallery