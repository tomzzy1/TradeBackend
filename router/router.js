import user from './user.js'

import Router from 'koa-router'

let router = new Router(
{
  prefix: '/dev-api/vue-element-admin'
})

router.use(user.routes(), user.allowedMethods())

export default router