import user from './user.js'
import cart from './shopping-cart.js'
import order from './order.js'
import Router from 'koa-router'
import gallery from './gallery.js'
import crowdsourcing from './crowdsourcing.js'
import recvFile from './receiveFile.js'
import detail from './detail.js'
import post from './post_question.js'
import setting from './setting.js'

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
router.use(detail.routes(), detail.allowedMethods())
router.use(post.routes(), post.allowedMethods())
router.use(setting.routes(), setting.allowedMethods())

export default router