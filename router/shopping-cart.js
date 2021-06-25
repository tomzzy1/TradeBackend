import Router from 'koa-router'
import pool from '../sql/pool.js'
import {parsePostData, parseQueryStr} from '../utils/parsePostData.js'

let cart = new Router({
    prefix: '/cart'
})

/*cart.get('/list', async (ctx) => {
    let goodsList = []
    pool.query("SELECT id, number from shopping_cart", (err, result, field)=>{
        console.log('shopping_cart get list')
        console.log(result)  
        for (let i = 0; i < result.length; i++)
            {
                pool.query("SELECT id, name, date, price FROM goods WHERE id=?", result[i].id, (err, goods, field) => {
                if (err)
                    console.log(err)
                    console.log(goods)
                    goods.number = result[i].number
                    goodsList.push(goods)
            })
            
            ctx.body = {
                code: 20000,
                data: 
                {
                    list: goodsList,
                    total: goodsList.length
                }
            }
        }*/
cart.get('/list', async (ctx) => {
    //let goodsList = []
    let [cart, _] = await pool.query("SELECT id, number from shopping_cart")
    console.log('shopping_cart get list')
    console.log(cart)  
    let waitList = []
    for (let i = 0; i < cart.length; i++)
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
    let parsedData = JSON.parse(postData)
    console.log(parsedData)
    let id = parsedData['id']
    let new_num = parsedData['number']
    pool.query("UPDATE shopping_cart SET number=? WHERE id=?", new_num, id, (err, result, field)=>{
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
    let id = parsedData['id']
    pool.query("DELETE FROM shopping_cart WHERE id=?", id, (err, result, field)=>{
        console.log(result)
    })
    ctx.body = 
    {
        code: 20000
    }

}).post('/add', async (ctx) => {
    let postData = await parsePostData(ctx)
    let parsedData = JSON.parse(postData)
    console.log(parsedData)
    let id = parsedData['id']
    pool.query("INSERT INTO shopping_cart (id, number) VALUES(?, ?)", id, 1, (err, result, field)=>{
        console.log(result)
    })
    ctx.body = 
    {
        code: 20000
    }
})

export default cart