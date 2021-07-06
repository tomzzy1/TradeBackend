import user from './user.js'
import cart from './shopping-cart.js'
import order from './order.js'
import Router from 'koa-router'
import gallery from './gallery.js'
import crowdsourcing from './crowdsourcing.js'
import recvFile from './receiveFile.js'

let router = new Router(
{
  prefix: '/dev-api/vue-element-admin'
})

router.use(user.routes(), user.allowedMethods())
router.use(cart.routes(), cart.allowedMethods())
router.use(order.routes(), order.allowedMethods())
router.use(gallery.routes(), gallery.allowedMethods())
router.use(crowdsourcing.routes(), crowdsourcing.allowedMethods())
router.use(recvFile.routes(), recvFile.allowedMethods())

export default router