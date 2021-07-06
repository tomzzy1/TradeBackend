import Router from 'koa-router'
import koaBody from 'koa-body'
import path from 'path'


let recvFile = new Router({
    prefix: '/upload_item'
})

recvFile.use(koaBody(
{   
    multipart: true, 
    formidable:
    {
        uploadDir: './uploads',
        keepExtensions: true
    }
}))

recvFile.post('/upload',async (ctx) => {
    console.log('processing upload')
    const file = ctx.request.body.files.file
    
    console.log(file.path)
    ctx.body = 
    {
        code: 20000
    }
})

export default recvFile