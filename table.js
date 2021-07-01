const Koa = require('koa')
const app = new Koa()

const tokens = {
    admin: {
      token: 'admin-token'
    },
    worker: {
      token: 'worker-token'
    },
    buyer: {
      token: 'buyer-token'
    },
    seller: {
      token: 'seller-token'
    }
  }
  
  const users = {
    'admin-token': {
      roles: ['admin'],
      introduction: 'I am an Administrator',
      avatar: 'https://wpimg.wallstcn.com/f778738c-e4f8-4870-b634-56703b4acafe.gif',
      name: 'admin'
    },
    'worker-token': {
      roles: ['worker'],
      introduction: 'I am a worker',
      avatar: 'https://wpimg.wallstcn.com/f778738c-e4f8-4870-b634-56703b4acafe.gif',
      name: 'worker'
    },
    'buyer-token': {
      roles: ['buyer'],
      introduction: 'I am a buyer',
      avatar: 'https://wpimg.wallstcn.com/f778738c-e4f8-4870-b634-56703b4acafe.gif',
      name: 'buyer'
    },
    'seller-token': {
      roles: ['seller'],
      introduction: 'I am a seller',
      avatar: 'https://wpimg.wallstcn.com/f778738c-e4f8-4870-b634-56703b4acafe.gif',
      name: 'seller'
    }
  }

app.use( async ( ctx ) => {

    //console.log(ctx)
    

    let url = ctx.url
    console.log(url)
    console.log(ctx.request)
    if (ctx.method == 'GET')
    {
        if (url === '/dev-api/vue-element-admin/user/info')
        {
            ctx.body = 
            {
                data: users['admin-token'],
                code: 20000
            }
            console.log(ctx.body)
        }
    }
    if (ctx.method == 'POST')
    {
        if (url === '/dev-api/vue-element-admin/user/login')
        {
            let postData = await parsePostData(ctx)
            let data = JSON.parse(postData)
            console.log(data)
            if (data['username'] === 'buyer')
            {
                ctx.body = 
                {
                    data: ['buyer'],
                    code: 20000
                }
                console.log(ctx.body)
            }
            //console.log(postData)
        }
    }
  })
  
  app.listen(9527, () => {
    console.log('[demo] request get is starting at port 9527')
  })

  function parsePostData( ctx ) {
    return new Promise((resolve, reject) => {
      try {
        let postdata = "";
        ctx.req.addListener('data', (data) => {
          postdata += data
        })
        ctx.req.addListener("end",function(){
          //let parseData = parseQueryStr( postdata )
          resolve( postdata )
        })
      } catch ( err ) {
        reject(err)
      }
    })
  }

  function parseQueryStr( queryStr ) {
    let queryData = {}
    let queryStrList = queryStr.split('&')
    console.log( queryStrList )
    for (  let [ index, queryStr ] of queryStrList.entries()  ) {
      let itemList = queryStr.split('=')
      queryData[ itemList[0] ] = decodeURIComponent(itemList[1])
    }
    return queryData
  }