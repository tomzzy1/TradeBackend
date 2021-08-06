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
      avatar: 'https://img2.baidu.com/it/u=694300726,2684226088&fm=26&fmt=auto&gp=0.jpg',
      name: 'admin'
    },
    'worker-token': {
      roles: ['worker'],
      introduction: 'I am a worker',
      avatar: 'https://img2.baidu.com/it/u=2778260321,2626442338&fm=26&fmt=auto&gp=0.jpg',
      name: 'worker'
    },
    'buyer-token': {
      roles: ['buyer'],
      introduction: 'I am a buyer',
      avatar: 'https://gimg2.baidu.com/image_search/src=http%3A%2F%2Fimages.669pic.com%2Felement_pic%2F67%2F93%2F67%2F70%2F852ce063a77202fe2954747d1b29905a.jpg%21w700wb&refer=http%3A%2F%2Fimages.669pic.com&app=2002&size=f9999,10000&q=a80&n=0&g=0n&fmt=jpeg?sec=1629664778&t=0c210374f2fab7ef9731b6fa8a3b3d19',
      name: 'buyer'
    },
    'seller-token': {
      roles: ['seller'],
      introduction: 'I am a seller',
      avatar: 'http://bpic.588ku.com/element_origin_min_pic/17/12/14/406525ae9fc82311116781f6e3d52ac0.jpg',
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
