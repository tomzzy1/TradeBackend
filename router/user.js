import Router from 'koa-router'
import {parsePostData, parseQueryStr} from '../utils/parsePostData.js'

let user = new Router({
    prefix: '/user'
})

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

user.get('/info', async (ctx) =>
{
  console.log('processing get user info')
  //let postData = await parsePostData(ctx)
  //let parsedData = JSON.parse(postData)
  //console.log(parsedData)
  console.log(ctx.query)
  let parsedData = JSON.parse(ctx.query.token)
  ctx.body = 
  {
    data: users[parsedData.token],
    code: 20000
  }
  console.log(ctx.body)
})
.post('/login', async (ctx) => 
{
  console.log('processing login')
  let postData = await parsePostData(ctx)
  let parsedData = JSON.parse(postData)
  console.log(parsedData)
  ctx.body = 
  {
    data: 
    {
      token: tokens[parsedData['username']]
    },
    code: 20000
  }
  console.log(ctx.body)

})
.post('/logout', async (ctx) => 
{
  console.log('processing logout')
  ctx.body = 
  {
    data: 'success',
    code: 20000
  }
}).redirect('/login:1', '/login')

export default user
