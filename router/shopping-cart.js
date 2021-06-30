import Router from 'koa-router'
import pool from '../sql/pool.js'
import {parsePostData, parseQueryStr} from '../utils/parsePostData.js'

let cart = new Router({
    prefix: '/cart'
})

cart.get('/list', async (ctx) => {
    //let goodsList = []
    console.log(ctx.query)
    let params = ctx.query
    let page = params.page
    let limit = params.limit
    let [cart, _] = await pool.query("SELECT id, number from shopping_cart")
    console.log('shopping_cart get list')
    console.log(cart)  
    let waitList = []
    for (let i = (page - 1) * limit; i < page * limit; i++)
    {
        let [goods, _] = await pool.query("SELECT id, name, date, price FROM goods WHERE id=?", cart[i].id)
        waitList.push(goods)
    }
    let goodsList = await Promise.all(waitList)
    console.log(goodsList)
    let List = []
    for (let i = 0; i < goodsList.length; i++)
    {
        let [result] = Object.values(JSON.parse(JSON.stringify(goodsList[i])))
        result.number = cart[i].number
        List.push(result)
        console.log(result)
    }
    //console.log(goodsList)
    ctx.body = {
        code: 20000,
        data: 
        {
            items: List,
            total: List.length
        }
    }

}).post('/update', async (ctx) => {
    let postData = await parsePostData(ctx)
    if (!postData)
        console.log('error!')
    let parsedData = JSON.parse(postData)
    console.log(parsedData)
    let id = parsedData.id
    let new_num = parsedData.num
    console.log(id, new_num)
    pool.query("UPDATE shopping_cart SET number=? WHERE id=?", [new_num, id], (err, result, field)=>{
        console.log("The number of ID: {0} is updated to {1}".format(id, new_num))
    })
    ctx.body = 
    {
        code: 20000
    }
}).post('/delete', async (ctx) => {
    let postData = await parsePostData(ctx)
    let parsedData = JSON.parse(postData)
    console.log(parsedData)
    let id = parsedData
    let result = await pool.query("DELETE FROM shopping_cart WHERE id=?", id)
    console.log(result)
    ctx.body = 
    {
        code: 20000
    }

}).post('/add', async (ctx) => {
    let postData = await parsePostData(ctx)
    let parsedData = JSON.parse(postData)
    console.log(parsedData)
    let id = parsedData['id']
    pool.query("INSERT INTO shopping_cart (id, number) VALUES(?, ?)", [id, 1], (err, result, field)=>{
        console.log(result)
    })
    ctx.body = 
    {
        code: 20000
    }
})

export default cart