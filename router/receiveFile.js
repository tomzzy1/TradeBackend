import Router from 'koa-router'
import koaBody from 'koa-body'
import path from 'path'
import fs from 'fs'


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
    let re = /^[0-9]+/
    for (let key in files)
    {
      if (re.test(key))
      {
        let file = files[key]
        const reader = fs.createReadStream(file.path)
        const writer = fs.createWriteStream(path.join('./uploads/', file.name))
        reader.pipe(writer)
      }
    }
    ctx.body = 
    {
        code: 20000
    }
})


export default recvFile