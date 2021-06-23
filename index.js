import Koa from 'koa'
import router from './router/router.js'

const app = new Koa()

app.use(router.routes())

/*app.use( async ( ctx ) => {

    //console.log(ctx)
    let url = ctx.url
    console.log(url)
    console.log(ctx.request)
    ctx.body = 
    { 
      code: 20000
    }
   
  })*/
  
  app.listen(9527, () => {
    console.log('[demo] request get is starting at port 9527')
  })

  

