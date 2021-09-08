

## 1.简介

````txt
macaron是高生产和模块的Go web框架,grafana使用此web框架
````

macaron官网有详细文档 https://go-macaron.com/

## 2.使用

#### 安装

````txt
go get gopkg.in/macaron.v1
````

#### 简单例子

`````go
package main

import "gopkg.in/macaron.v1"

func main() {
    m := macaron.Classic()
    m.Get("/", func() string {
        return "Hello world!"
    })
    m.Run()
}
`````



`````txt
函数macaron.Classic创建并返回 Classic Macaron.
`````

`````txt
方法m.Get注册路由用于HTTP GET方法,此例允许GET请求路径为根目录以及对应Handler函数.此handler函数返回一个字符串最为响应
`````

`````txt
调用m.Run()方法启动服务器，默认，macaron实例监听0.0.0.0:4000
`````

`````txt
运行go run main.go，有以下log message:
`````

`````log
[Macaron] listening on 0.0.0.0:4000 (development)
`````

打开浏览器，访问 [localhost:4000](http://localhost:4000/)。



#### 例子拓展

`````go
package main

import (
    "log"
    "net/http"

    "gopkg.in/macaron.v1"
)

func main() {
    m := macaron.Classic()
    m.Get("/", myHandler)

    log.Println("Server is running...")
    log.Println(http.ListenAndServe("0.0.0.0:4000", m))
}

func myHandler(ctx *macaron.Context) string {
    return "the request path is: " + ctx.Req.RequestURI
}
`````

`````txt
再次运行go run main.go，控制台信息：
`````

`````txt
the request path is: /
`````

`````txt
这次没有使用匿名函数，而是使用函数myHandler
`````

````txt
函数myHandler接收一个 *macaron.Context参数 返回一个string
````

````txt
注意m.Get注册路由时，没有向myHandler传递参数，因为macaron将所有的handler函数作为interface{}，这里有个服务注册的概念，*macaron.Context是一个默认服务注册，可以直接使用它。
````

`````txt
和前面的例子一样，启动服务监听，这次调用go标准库函数http.ListenAndServe，这说明了macaron实例与GO标准库完全兼容
`````

----

## 3.核心概念

#### Classic Macaron

`````txt
为了快速有效的运行，macaron.Classic 为大多数web 应用程序提供了有用的默认功能
`````

``````go
m := macaron.Classic()
// ... middleware and routing goes here
m.Run()
``````

`````txt
请求/响应日志 macaron.Logger
静态文件服务 macaron.Static
Panic recovery macaron.Recovery
`````



#### Instances

````txt
任何 macaron.Macaron 类型的对象都被看做  Macaron实例，可以在一段代码中设置多个实例

````

#### Handlers

`````txt
Handlers是 Macaron的核心和灵魂，handler可以是任何可以调用的函数
`````

````go
m.Get("/", func() string {
    return "hello world"
})
````



`````txt
普通函数可以被多个路由使用
`````

````go
m.Get("/", myHandler)
m.Get("/hello", myHandler)

func myHandler() string {
    return "hello world"
}
````



````txt
此外，一个路由可以处理多个handlers
````

`````txt
m.Get("/", myHandler1, myHandler2)

func myHandler1() {
    // ... do something
}

func myHandler2() string {
    return "hello world"
}
`````



#### Return Values

`````txt
如果handler 有返回值， Macaron 会将其作为string写入当前的标准库http.ResponseWriter
`````

````go
m.Get("/", func() string {
    return "hello world" // HTTP 200 : "hello world"
})

m.Get("/", func() *string {
    str := "hello world"
    return &str // HTTP 200 : "hello world"
})

m.Get("/", func() []byte {
    return []byte("hello world") // HTTP 200 : "hello world"
})

m.Get("/", func() error {
    // Nothing happens if returns nil
    return nil 
}, func() error {
    // ... get some error
    return err // HTTP 500 : <error message>
})
````



````txt
也可以操作返回状态代码，只能应用于string和[]byte类型
````

`````go
m.Get("/", func() (int, string) {
    return 418, "i'm a teapot" // HTTP 418 : "i'm a teapot"
})

m.Get("/", func() (int, *string) {
    str := "i'm a teapot"
    return 418, &str // HTTP 418 : "i'm a teapot"
})

m.Get("/", func() (int, []byte) {
    return 418, []byte("i'm a teapot") // HTTP 418 : "i'm a teapot"
})
`````



#### Service Injection

`````txt
Handlers 通过反射，Macaron使用依赖注入，解决Handler参数列表中的依赖。这使得Macaron 完全兼容GO标准库http.HandlerFunc interface.
`````

、cc

````txt
当你向handlers添加参数，Macaron 会搜索它的服务列表，通过类型断言解析依赖
````

````go
m.Get("/", func(resp http.ResponseWriter, req *http.Request) {
    // resp and req are injected by Macaron
    resp.WriteHeader(200) // HTTP 200
})
````



`````txt
最应常用的服务是:*macaron.Context
`````

`````go
m.Get("/", func(ctx *macaron.Context) {
    ctx.Resp.WriteHeader(200) // HTTP 200
})
`````



````txt
下列服务都包含在macaron.Classic
*macaron.Context - HTTP request context
*log.Logger - Global logger for Macaron instances
http.ResponseWriter - HTTP Response writer interface
*http.Request - HTTP Request
````



#### Middleware Handlers

`````txt
中间件处理程序设置在将要来到的 HTTP request和路由之间，本质上它与其他Handlers没有不同
`````

`````go
m.Use(func() {
  // do some middleware stuff
})
`````

````txt
可以使用Handlers function控制中间件，这回替代之前设置的handlers函数
````

`````go
m.Handlers(
    Middleware1,
    Middleware2,
    Middleware3,
)
`````



````txt
中间件程序用于处理 logging, authorization, authentication, sessions, gzipping, error pages，之类发生在HTTP request之前或之后的操作
````

`````go
// validate an api key
m.Use(func(ctx *macaron.Context) {
    if ctx.Req.Header.Get("X-API-KEY") != "secret123" {
        ctx.Resp.WriteHeader(http.StatusUnauthorized)
    }
})
`````

#### Handler Workflow







## 4.核心服务

``````txt
默认，Macaron注册一些服务驱动你的程序，可以直接使用它们作为参数在你的handler程序，无需其他的操作
``````

#### Context

`````txt
*macaron.Context 这是Macaron的最核心的服务，它包含request, response, templating, data store,以及注册和检索其他服务所需要的信息
`````

`````go
package main

import "gopkg.in/macaron.v1"

func Home(ctx *macaron.Context) {
    // ...
}
`````

#### Next()

`````txt
方法Context.Next可以让中间件handler直到其他handler处理完后才执行，这可以用于任何需要在 HTTP request后发生的操作
`````

`````go
// log before and after a request
m.Use(func(ctx *macaron.Context, log *log.Logger){
    log.Println("before a request")

    ctx.Next()

    log.Println("after a request")
})
`````



#### Cookie()

``````txt
cookie的基本用法
``````

````txt
*macaron.Context.SetCookie
*macaron.Context.GetCookie, *macaron.Context.GetCookieInt, *macaron.Context.GetCookieInt64, *macaron.Context.GetCookieFloat64
````



`````go
// ...
m.Get("/set", func(ctx *macaron.Context) {
    ctx.SetCookie("user", "Unknwon", 1)
})

m.Get("/get", func(ctx *macaron.Context) string {
    return ctx.GetCookie("user")
})
// ...
`````

#### 路由日志

该服务可以通过函数 [`macaron.Logger`](https://gowalker.org/gopkg.in/macaron.v1#Logger) 来注入。该服务主要负责应用的路由日志。

`````go
package main

import "gopkg.in/macaron.v1"

func main() {
    m := macaron.New()
    m.Use(macaron.Logger())
    // ...
}
`````



#### 容错恢复

该服务可以通过函数 [`macaron.Recovery`](https://gowalker.org/gopkg.in/macaron.v1#Recovery) 来注入。该服务主要负责在应用发生恐慌（panic）时进行恢复。

`````go
package main

import "gopkg.in/macaron.v1"

func main() {
    m := macaron.New()
    m.Use(macaron.Recovery())
    // ...
}
`````



#### 静态文件

该服务可以通过函数 [`macaron.Static`](https://gowalker.org/gopkg.in/macaron.v1#Static) 来注入。该服务主要负责应用静态资源的服务，当您的应用拥有多个静态目录时，可以对其进行多次注入。

`````go
package main

import "gopkg.in/macaron.v1"

func main() {
    m := macaron.New()
    m.Use(macaron.Static("public"))
    m.Use(macaron.Static("assets"))
    // ...
}
`````

默认情况下，当您请求一个目录时，该服务不会列出目录下的文件，而是去寻找 `index.html` 文件。

#### 使用示例

`````txt
public/
    |__ html
            |__ index.html
    |__ css/
            |__ main.css
`````

