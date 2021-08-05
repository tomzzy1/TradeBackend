import Router from 'koa-router'
import koaBody from 'koa-body'
import path from 'path'
import fs from 'fs'
import pool from '../sql/pool.js'
import {cppModule} from '../cppModule.cjs'
import loadCSVIntoDataset from '../utils/loadCSV.js'

let recvFile = new Router({
    prefix: '/upload_item'
})

recvFile.use(koaBody(
{   
    multipart: true, 
    formidable:
    {
        uploadDir: './temp',
        keepExtensions: true
    }
}))

recvFile.post('/upload_data',async (ctx) => {
    console.log('processing upload')
    console.log(ctx.request.files)
    //console.log(ctx.request.body)
    const files = ctx.request.files
    let params = JSON.parse(ctx.request.body.params)
    await pool.query("CREATE TABLE IF NOT EXISTS " + params.name + " LIKE NBA")
    let [ids, _] = await pool.query("SELECT id FROM goods")
    let id = ids.length
    await pool.query("INSERT IGNORE INTO goods (id, name, date, price) VALUES(?, ?, NOW(), ?)", 
    [id, params.name, params.base_price])
    await pool.query("INSERT IGNORE INTO goods_detail (id, description, source, size) VALUES(?, ?, ?, ?)",
    [params.id, params.description, params.source, params.size])
    let re = /^[0-9]+/
    let savePath = './query/archive'
    let names = []
    ctx.body = 
    {
        code: 20000
    }
    for (let key in files)
    {
      if (re.test(key))
      {
        let file = files[key]
        const reader = fs.createReadStream(file.path)
        let fpath = path.join(savePath, file.name)
        const writer = fs.createWriteStream(fpath)
        reader.pipe(writer)
        loadCSVIntoDataset(fpath)
        let fname = file.name.split('.')[0]
        names.push(fname)
        await pool.query("INSERT IGNORE INTO " + params.name + " (id, name) VALUES (?, ?)", 
            [Number(key), fname])
      }
      //console.log(key)
    }
    cppModule.generate(params.name, names, 0.2)
    cppModule.getQuestionInfo(params.name, names)
})


export default recvFile