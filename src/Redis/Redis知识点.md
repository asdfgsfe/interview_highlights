# Redis知识点
Redis可以使用（主从）复制特性来扩展读性能，使用客户端分片来扩展写性能。

## 1. Redis数据结构及使用场景？

### 1.1 简单动态字符串SDS

### 1.2 链表

### 1.3 字典

### 1.4 跳跃表


Redis内存回收使用了引用计数法。

## 2. 持久化
两种持久化方式：RDB和AOF.
### 2.1 AOF
AOF(Append Only File)持久化是通过保存Redis服务器所执行的*写命令*来记录数据库状
态的。写入的命令以Redis命令请求协议格式保存。

配置项：
```
appendonly yes ## 开启AOF
appendfsync always   # 每个事件循环都将aof_buf缓冲区内容写入AOF，并进行同步
appendfsync everysec # 默认，每个事件循环都将aof_buf缓冲区内容写入AOF文件，每隔1s同步一次
appendfsync no # 每个事件循环都将aof_buf缓冲区中的所有内容写入AOF，而同步则由操作系统控制
```

AOF重写，目的是减小AOF文件体积膨胀的问题。
执行BGREWRITEAOF命令或者配置conf文件来指定触发重写的条件，可自动进行AOF重写。

**数据不一致问题：**

AOF重写期间，服务器还要继续处理命令请求，新的命令可能会使当前数据库状态与重写后的AOF文件锁保存
的数据库状态不一致。

为解决这个问题，Redis设置了一个AOF重写缓冲区。该缓冲区在创建子进程之后使用：Redis执行完写命令
后，同时将这个写命令发送给AOF缓冲区和AOF重写缓冲区。子进程完成AOF的重写工作后，向父进程发送信号
，父进程接到该信号后，会调用信号处理函数：（1）将AOF重写缓冲区中的内容写入新的AOF文件中，从而使数据
一致。（2）对新AOF文件改名，原子覆盖现有的AOF文件。

### 2.2 RDB
SAVE命令，执行同步保存操作。生产环境很少使用。它会阻塞所有的客户端。
BGSAVE命令执行异步保存操作。fork新的进程来完成持久化，原来的进程继续处理客户端请求。

通过conf文件，可配置触发BGSAVE的条件。

优点：RDB文件格式紧凑，适用于备份数据。
缺点：数据集很大的时候，执行fork耗费的时间较长，可能会导致redis在几毫秒甚至一秒内停止处理客户
请求。

生产环境中，要同时使用这两种持久化方式。

## 3. 过期键的删除策略
Redis采用惰性删除 + 定期删除。

## 4. MAXMEMORY POLICY
Redis使用的内存超过设置的最大内存时，采取的策略。
- volatile-lru：对设置过期时间的键使用LRU.
- allkeys-lru：对所有的键使用LRU.
- volatile-random
- allkeys-random
- volatile-ttl
- noeviction：到达最大内存后，会报错。

Note: Redis中使用的LRU

## 5. Pipeline机制（非事务流水线）
减少RTT时间，以提升性能。

```python
pipe = conn.pipeline(False) // 设置流水线
pipe.hset('login:', token, user)
pipe.zadd(...)
pipe.execute() // 执行被流水线包裹的命令
```

## 6. 主从复制

相关命令：
```redis
slaveof host port #将当前服务器设置为host:port服务器的slave. 丢弃旧数据。

slaveof no one #关闭从服务器器的复制功能，并从从服务器变为主服务器。会保留同步所得数据

role # 返回服务器担任的角色

```
### 6.1 复制的流程


### 6.2 陷阱
**注意**:当配置Redis主从复制功能后，强烈建议打开主服务器的持久化功能,否则，应当
禁止reboot后，服务自动重启。

为了帮助理解主服务器关闭持久化时自动拉起的危险性，参考一下以下会导致主从服务器数
据全部丢失的例子：

1) 假设节点A为主服务器，并且关闭了持久化。 节点B和节点C从节点A复制数据;

2) 节点A崩溃，然后由自动拉起服务重启了节点A. 由于节点A的持久化被关闭了，所以重启
之后没有任何数据.

3) 节点B和节点C将从节点A复制数据，但是A的数据是空的， 于是就把自身保存的数据副本
删除。

在关闭主服务器上的持久化，并同时开启自动拉起进程的情况下，即便使用Sentinel来实现
Redis的高可用性，也是非常危险的。 因为主服务器可能拉起得非常快，以至于Sentinel在
配置的心跳时间间隔内没有检测到主服务器已被重启，然后还是会执行上面的数据丢失的流程。

*Tips:* 应该禁止主服务器关闭持久化的同时自动重启服务。

## 7. 哨兵Sentinel

## 8. 集群Cluster

## 9. 事务操作

五个命令：
```
WATCH   # 开始监控keys
MULTI   # 开启事务
EXEC    # 执行
UNWATCH # 取消监控keys
DISCARD # 放弃事务（也就是删除队列中的指令）
```
事务不具有可扩展性，因为程序在尝试完成一个事务的时候，可能会因为事务执行失败而
反复地进行重试。

## 10. 分布式锁
构建分布式锁需要考虑：
(1) 机器崩溃或进程崩溃，无法释放锁；
(2) 锁过期而被释放。

### 10.1 普通实现
分布式锁的python实现：
```python
// 加锁
def acquire_lock(conn, lockname, acquire_timeout = 10):
    identifier = str(uuid.uuid4()) // 随机生成128位UUID
    end = time.time() + acquire_timeout
    while time.time() < end:
        if conn.setnx('lock:' + lockname, identifier):
            return identifier
        time.sleep(.001)
    return False

// 解锁，无超时限制
def release_lock(conn, lockname, identifier):
    pipe = conn.pipeline(True)
    lockname = 'lock:' + lockname

    while True
        try:
            pipe.watch(lockname)
            if pipe.get(lockname) == identifier: // 检查标识符是否与加锁时一致
                pipe.multi()
                pipe.delete(lockname)
                pipe.execute()
                return True
            
            pipe.unwatch()
            break
        except redis.exceptions.WatchError:
            pass
    return False
```
上面实现的锁在持有者崩溃的时候，不会自动释放。为了解决这个问题，为锁加上超时功能。

```python
// 带超时时间的锁
def acquire_timedlock(conn, lockname, acquire_timeout=10, lock_timeout=10):
    identifier = str(uuid.uuid4())
    lockname = 'lock:' + lockname
    lock_timeout = int(math.ceil(lock_timeout))
    end = time.time() + acquire_timeout

    while time.time() < end:
        if conn.setnx(lockname, identifier):
            conn.expire(lockname, lock_timeout) // 保证锁的释放
            return identifier
        elif not conn.ttl(lockname):
            conn.expire(lockname, lock_timeout)
        
        time.sleep(.001)
    return False
```

可以使用Lua来重写上面的锁，来提高性能。

另外，在Redis中，WATCH被称乐观锁：只会在数据被其它客户端抢先修改了的情况下
才通知这个命令的客户端，而不阻止其他客户端对数据进行修改。

### 10.2 Redlock
TODO