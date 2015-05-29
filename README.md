># lproxy-childish
> ###lproxy-childish: 
>> ####**[ 此版本已被丢弃，所以被命名为-childish，推荐更为健壮功能更为完善的 [lproxy](https://github.com/DD-L/lproxy.git), 请移步 [https://github.com/DD-L/lproxy.git](https://github.com/DD-L/lproxy.git) ]**

>> 基于HTTP的轻量级网络代理服务器。

> ### Platform
>>* unix/linux
>>* cygwin

> ### 依赖库
>>* iconv
>>* pthread


> ### lproxy-childish 使用方法
>> #### 1. 源码编译:
>>> <pre><code>
$ cd lproxy-childish
$ make clean
$ make
</code></pre>

>> #### 2. 运行 lproxy 示例:
>>> <pre><code>
$ cd bin
$ ./lproxy port # or './proxy port' 
</code></pre>

>> #### 3. options:
>>> <pre><code>
proxy port
proxy -p port [-l clientIpList] [-c code]

>>>
-p[必选]        port        : 服务器端(本机)使用的端口.
-l[可选]        clientIpList: 允许客户端的ip列表,多个ip用','隔开;
                不指定列表表示允许所有ip.
-c[可选]        code        : proxy显示的编码,默认为 UTF-8.
</code></pre>
推荐下面使用方式:
>>> <pre><code>
lproxy [port]
lproxy -p port [-l clientIpList] [-c code]

>>>
 port                       : 端口默认2222 
-p[必选]        port        : 服务器端(本机)使用的端口.
-l[可选]        clientIpList: 允许客户端的ip列表,多个ip用','隔开;
                不指定列表表示允许所有ip.
-c[可选]        code        : proxy显示的编码,默认为 UTF-8.
</code></pre>


> ### lproxy-childish 特性
>>
* 轻量级HTTP代理服务器
* 支持客户端IP白名单设置
* 为适应windows远程查看，提供编码设置
* 中小型级负载能力
* 多线程支持
* socket 异步读写
* **...**

---------------------------------------------------------

> ####[LICENSE](./LICENSE)
&copy; Deel@d-l.top | [d-l.top](http://d-l.top)
