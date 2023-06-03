# 前言
在网易从事游戏服务器开发近四年，前后用过bigworld server/mobile server/messiah server三款网易内部的服务器引擎。

bigworld server是真正的无缝大世界服务端引擎，由澳大利亚公司研发，网易后来买入，是我在网易使用时间最长的一款服务器引擎，也是我觉得最牛逼的服务器引擎，只是上手难度较大。

mobile server和messiah server是网易内部自研的，很多设计均参考bigworld server，但上手难度较bigworld更低，经过多年发展，已经渐渐取代bigworld，现在网易互娱内部新项目的游戏服务器引擎大多以这两款为主。

# yjengine
yjengine是我自研的游戏服务器引擎，架构上取bigworld server/mobile server/messiah server三者之长，下面介绍一下yjengine的使用方法以及设计思想。

<span id="catalogue">整个介绍分为以下几个部分</span>
* [分布式架构](#distributed)
* [网络架构](#net)
* [多线程框架](#thread)
* [rpc框架](#rpc)
* [entity系统](#entity)
* [Mailbox](#mailbox)
* [属性系统](#property)
* [component系统](#component)
* [entity的创建/迁移/灾备/重连](#flowchart)
* AOI - TODO
* 无缝大世界 - TODO

## <span id="distributed">分布式架构[（目录）](#catalogue)</span>
yjengine是一款分布式服务器引擎，进程分为game和gate两类，若干个game和若干个gate组成分布式集群，game/client分别和gate直连，gate负责game和client以及game之间的通信转发，game承载所有游戏实例（包括玩家，怪物，场景等等）。

分布式的优势是将单点压力分散到多点上，yjengine首先将网络连接的压力从game上剥离到gate上，gate支持横向拓展，单服同时的客户端网络连接理论上是没有上限的。
game也支持横向拓展，服务端的游戏实例分布在不同game上，且部分游戏实例可以在game之间进行迁移，以mmo游戏为例，游戏中的场景实例分布在不同game上，玩家实例离开上一个场景进入新场景就可能切换到新的进程上。

后续分布式架构还会支持服务发现，即支持线上game和gate进程的动态扩缩容

## <span id="net">网络框架[（目录）](#catalogue)</span>
分布式集群中，client作为客户端向gate发起TCP连接请求，gate作为客户端向game发起TCP连接请求。
* client -> gate -> game

yjengine的底层网络框架用的是boost::asio库，boost::asio基于windows下的iocp和linux下的epoll封装了一套统一的接口，实现了跨平台兼容，利用boost::asio的异步接口可以很容易实现高并发的网络服务器，boost::asio还支持多线程并行，关于boost::asio的详细介绍可以上网搜一下。

yjengine基于boost:asio封装了session manager(boost_asio.h)和remote manager(remote_manager.h)。

session manager用来管理net client（game上每个gate连接是一个session，gate上的每个client连接也是一个session）。

remote manager管理net server（gate上每个game连接是一个remote，client上和gate的连接也是remote）。

## <span id="thread">多线程框架[（目录）](#catalogue)</span>
yjengine的线程分三类：
* 主线程
* 网络线程
* 辅助线程

### 主线程
用来跑游戏逻辑

### 网络线程
boost::asio的异步任务是支持多线程的，yjengine中为了降低主线程压力，网络包的接收和发送，序列化和反序列化都是在子线程进行的，以发包为例，假设有packet_1和packet_2两个网络包，多线程处理步骤如下：

1. 主线程调用一次发包接口发送packet_1，先将packet_1缓存起来，并往异步队列中插入一个发包通知，并标记session为发包状态

2. 主线程再次调用发包接口发送packet_2，同理packet_2被缓存，此时session已经处于发包状态，不做任何处理
  
3. 子线程A从异步队列中拿到发包通知，从缓存中取出packet_1进行发送，发送结束后会向异步队列中插入回调通知
  
4. 子线程B从异步队列中拿到回调通知，发现缓存中还有packet_2待发送，线程B再将packet_2发送出去，发送结束后向队列中插入回调通知
  
5. 子线程C从异步队列中拿到回调通知，发现没有待发送的网络包了，将session标记为待发送状态，等待下次主线程的发送请求

### 辅助线程
辅助线程用于写日志和进程间的心跳检测

* 写日志 - 其他线程调用写日志接口时，会将日志信息打包丢进日志队列中，然后由辅助线程从日志队列中取出输出到日志文件或终端

* 心跳检测 - game和gate之间的心跳检测也是放在辅助线程处理的，好处有两个：一是降低主线程压力，二是心跳检测不受游戏逻辑影响

## <span id="rpc">rpc框架[（目录）](#catalogue)</span>
yjengine基于C++模板实现了一套rpc框架，用于game和gate之间的消息序列化和反序列化，比如gate要调用game上的方法，只需要两步：
1. 在game上实现并注册rpc方法
```
// 实现
void regist_from_gate(const GString& gate_listen_addr) {
    ...
}
// 注册
RPC_REGISTER(regist_from_gate);
```
2. 在gate上调用rpc方法
```
// remote代表game地址，后面接rpc名称和参数
REMOTE_RPC_CALL(remote, "regist_from_gate", get_listen_addr());
```
rpc的参数个数是可以扩展的
rpc的参数类型除了支持C++内置的布尔/整型/浮点型，还支持stl的字符串/数组/字典，以及yjengine自定义的二进制类型，具体可以参考代码中的GType枚举
```
enum GType : uint8_t {BOOL_T, INT8_T, INT16_T, INT32_T, INT64_T, UINT8_T, UINT16_T, UINT32_T, UINT64_T, FLOAT_T, DOUBLE_T, STRING_T, ARRAY_T, DICT_T, BIN_T, NONE_T};
```
rpc框架会对rpc的参数个数和类型进行校验，如果调用方传递的参数个数或类型不符合要求则会有明确的报错提示

## <span id="entity">entity系统[（目录）](#catalogue)</span>
游戏中的所有实例都称之为entity

### base && cell
前面说过游戏实例会在game之间迁移，但并不是所有数据都需要迁移，比如mmo中玩家身上需要迁移的一般都是和战斗或场景直接相关的（战斗属性（生命/防御），场景进入的次数等），其他无关的则不需要迁移（比如背包，养成之类）。所以将entity分为base和cell两部分，base承载无需迁移的数据固定于某个game上，而cell则承载需要迁移的数据，base和cell之间通过gate转发rpc进行异步通信。

对于entity来说，base是必要的，但cell是可选的。如果entity没有迁移需求，就可以不要cell。

另外部分entity是有客户端的（比如玩家），entity的客户端部分称之为client

### entity类型
根据entity是否有cell，以及是否有client，将entity分为如下几种类型：
```
enum EntityType : int8_t {
    EntityType_Base, 
    EntityType_BaseWithCell, 
    EntityType_BaseWithClient, 
    EntityType_BaseWithCellAndClient, 
    EntityType_Cell, 
    EntityType_CellWithClient, 
    EntityType_Client, 

    EntityType_None
};
```
* EntityType_Base是只有纯base的entity，比如游戏中的邮件系统/公会系统等单点服务（不过单点服务我更推荐用微服务来做，关于微服务框架的实现可以看我的另外一个[git仓库](https://github.com/610789338/microservice)）

* EntityType_BaseWithCell顾名思义是带cell的entity，需要配合EntityType_Cell使用，EntityType_BaseWithCell是entity的base部分，EntityType_Cell是entity的cell部分，比如游戏中的怪物

* EntityType_BaseWithCellAndClient是带cell和client的entity，需要配合EntityType_CellWithClient以及EntityType_Client使用，EntityType_BaseWithCellAndClient是entity的base部分，EntityType_CellWithClient是entity的cell部分，EntityType_Client是entity的client部分，这种类型的entity一般就指的就是玩家了

* EntityType_BaseWithClient是带client的entity，需要配合EntityType_Client使用，EntityType_BaseWithClient是entity的base部分，EntityType_Client是entity的client部分，比如登陆账号后进入游戏前会先创建一个Account实例，用于控制接下来的创建/选角，Account就是EntityType_BaseWithClient类型的

### entity的定义
yjengine中每种EntityType都有一个基类实现，entity定义只需继承相应基类即可，比如玩家entity的三部分定义分别为：
```
// base部分
class BaseAvatar : public BaseEntityWithCellAndClient {
    GENERATE_ENTITY_INNER(BaseAvatar);

    static void regist_components() { ... }
    static void rpc_method_define() { ... }
    static void property_define() { ... }
    static void migrate_timer_define() { ... }
...
    void on_ready() { ... }
};
GENERATE_ENTITY_OUT(BaseAvatar)

// cell部分
class CellAvatar : public CellEntityWithClient {
    GENERATE_ENTITY_INNER(CellAvatar);

    static void regist_components() { ... }
    static void rpc_method_define() { ... }
    static void property_define() { ... }
    static void migrate_timer_define() { ... }
...
    void on_ready() { ... }
};
GENERATE_ENTITY_OUT(CellAvatar)

// client部分
class ClientAvatar : public ClientEntity {
    GENERATE_ENTITY_INNER(ClientAvatar);

    static void regist_components() { ... }
    static void rpc_method_define() { ... }
    static void property_define() { ... }
    static void migrate_timer_define() { ... }
...
    void on_ready() { ... }
};
GENERATE_ENTITY_OUT(ClientAvatar)
```

GENERATE_ENTITY_INNER/GENERATE_ENTITY_OUT两个宏是必不可少的，用于entity的通用定义

四个static方法也是必不可少的，分别用于：
1. regist_components - 注册component
2. rpc_method_define - 注册rpc
3. property_define - 定义属性
4. migrate_timer_define - 使定时器支持迁移

on_ready方法一般用于注册定时器和事件

## <span id="mailbox">MailBox[（目录）](#catalogue)</span>
entity的base/cell/client之间通过mailbox发送rpc来通信，mailbox内部记录了entity uuid以及对端的网络地址(ip:port)，其基类实现如下：
```
class MailBox {
public:
    MailBox();
    ~MailBox();
...
protected:
    GString     m_entity_uuid = "";
    GString     m_addr = "";
    GString     m_side = "server";
    Entity*     m_owner;
};
```

以EntityType_BaseWithCellAndClient为例，其base部分在初始化的时候会生成cell和client的mailbox
```
class BaseEntityWithCellAndClient {
...
private:
    CellMailBox cell;
    ClientMailBox client;
};
```

entity rpc复用了rpc框架，base要调用cell的rpc同样分两步：
1. 在cell上实现并注册rpc
```
// 实现rpc
void CellAvatar::msg_from_base(const GString& msg) {
    INFO_LOG("[cell] msg.%s from base\n", msg.c_str());
}

// 注册rpc
static void rpc_method_define() {
    RPC_METHOD(RpcType::SERVER_ONLY, msg_from_base);
}
```

2. 在base上调用cell的rpc
```
// cell是Mailbox，call方法传入rpc名字和参数
cell.call("msg_from_base", "hello, i am base");
```

entity rpc分为以下几种类型
```
enum RpcType {
    SERVER_ONLY,
    EXPOSED,
    CLIENT
};
```
1. SERVER_ONLY是只允许在base/cell之间调用的rpc
2. EXPOSED是指允许从client发往base/cell的rpc
3. CLIENT是指从base/cell发往client的rpc

## <span id="property">属性系统[（目录）](#catalogue)</span>
yjengine实现了一套属性系统来管理entity身上数据的增删改查以及同步

### 属性类型
entity属性分为以下几种类型：
```
enum PropType : int8_t {
    NONE = 0x00,

    BASE_PRIVATE = 0x01,
    BASE_AND_CLIENT = 0x02,
    CELL_PRIVATE = 0x03,
    CELL_AND_CLIENT = 0x05,
    ALL = 0x08,
};
```
1. BASE_PRIVATE是base的属性
2. BASE_AND_CLIENT是base和client的共有属性，且会从base同步到client
3. CELL_PRIVATE是cell的属性
4. CELL_AND_CLIENT是cell和client的共有属性，且会从cell同步到client
5. ALL是base/cell/client共有的属性

### 属性定义
属性定义在property_define方法中，一般建议将base/cell/client的属性定义在一个文件中，这样可以方便的阅览entity身上的所有属性，比如：
```
template<class TEntity>
void avatar_property_test() {
    // avatar属性列表
    PROPERTY_SIMPLE(PropType::ALL, avatar_id, int32_t, 1001);
    PROPERTY_SIMPLE(PropType::BASE_AND_CLIENT, avatar_id_base, int32_t, 1002);
    PROPERTY_SIMPLE(PropType::CELL_AND_CLIENT, avatar_id_cell, int32_t, 1003);
    PROPERTY_SIMPLE(PropType::BASE_PRIVATE, avatar_id_base_private, int32_t, 1004);
    PROPERTY_SIMPLE(PropType::CELL_PRIVATE, avatar_id_cell_private, int32_t, 1005);
    PROPERTY_MAP(PropType::BASE_AND_CLIENT, avatar_datas, AvatarData);

    PROPERTY_SIMPLE(PropType::CELL_AND_CLIENT, migrate_int, int32_t, 0);
    PROPERTY_SIMPLE(PropType::CELL_PRIVATE, test_timer, int32_t, 0);
}

template<class TEntity>
void avatar_property_define() {
    avatar_property_test<TEntity>();
}

// avatar属性定义
static void property_define() {
    avatar_property_define<BaseAvatar>();
}
```
属性定义分四个宏
```
#define PROPERTY_SIMPLE(prop_type, property_name, prop_class, default_value) \
    TEntity::property_manager.template regist_simple_property<prop_class>(prop_type, #property_name, default_value)
#define PROPERTY_COMPLEX(prop_type, property_name, prop_class) \
    TEntity::property_manager.template regist_complex_property<prop_class>(prop_type, #property_name)
#define PROPERTY_ARRAY(prop_type, property_name, prop_class) \
    TEntity::property_manager.template regist_array_property<prop_class>(prop_type, #property_name)
#define PROPERTY_MAP(prop_type, property_name, prop_class) \
    TEntity::property_manager.template regist_map_property<prop_class>(prop_type, #property_name)
```

1. PROPERTY_SIMPLE - C++内置类型
2. PROPERTY_COMPLEX - 复合类型，将不同类型的属性组合在一起，类似于struct
3. PROPERTY_ARRAY - 数组，数组元素可以是任意类型
4. PROPERTY_MAP - 字典，字典的key是字符串，value可以是任意类型

以PROPERTY_SIMPLE为例，宏的参数分别代表：属性类型，属性名，属性类别，默认值

四个宏在代码中分别为以下四个类（均继承自EntityPropertyBase）
```
struct EntityPropertySimple : public EntityPropertyBase { ... }
struct EntityPropertyComplex : public EntityPropertyBase { ... }
struct EntityPropertyArray : public EntityPropertyBase { ... }
struct EntityPropertyMap : public EntityPropertyBase { ... }
```

### 属性嵌套
由于PROPERTY_COMPLEX类型的存在，使得属性系统支持属性嵌套，PROPERTY_COMPLEX配合C++的struct使用，比如：
```
// AvatarData是complex属性
PROPERTY_COMPLEX(PropType::BASE_AND_CLIENT, avatar_data, AvatarData);

// AvatarData属性的定义
struct AvatarData : public EntityPropertyComplex {
    MEM_PROP_BEGIN_Eight_MEM(AvatarData,
        avatar_id,
        avatar_name,
        avatar_level,
        avatar_extra_data,
        avatar_title_ids,
        avatar_equips,
        avatar_horses,
        avatar_fashion_shows);

    MEM_PROPERTY_SIMPLE(avatar_id, int32_t, 0);
    MEM_PROPERTY_SIMPLE(avatar_name, GString, "");
    MEM_PROPERTY_SIMPLE(avatar_level, int32_t, 0);
    MEM_PROPERTY_COMPLEX(avatar_extra_data, AvatarExtraData);
    MEM_PROPERTY_ARRAY(avatar_title_ids, int32_t);
    MEM_PROPERTY_ARRAY(avatar_equips, AvatarEquip);
    MEM_PROPERTY_MAP(avatar_fashion_shows, int32_t);
    MEM_PROPERTY_MAP(avatar_horses, AvatarHorse);
};
```
AvatarData共有8个成员，成员用MEM_PROPERTY_XXX宏来定义，成员可以是任意类型。

### 属性树
每个entity实例都会有自己的属性树实例，属性树本质是一个多叉树，根节点即entity自身，PROPERTY_XXX定义的属性是树的第一层子节点，ARRAY和MAP的元素以及COMPLEX的成员为树的第N层子节点（N > 1）

complex开头的MEM_PROP_BEGIN_Eight_MEM宏是用于生成属性树中complex节点的子节点，目前一共有九个宏（从MEM_PROP_BEGIN_One_MEM到MEM_PROP_BEGIN_Nine_MEM，可拓展），complex有几个成员就用哪个宏，否则会导致属性树生成错误。

利用属性树可以遍历entity身上的所有属性，基于属性树可以实现属性的差量同步和全量同步。


### 属性的增删改查
* 增

ARRAY用push_back(T value)方法，T支持所有类型

MAP用insert(const GString& key, T value)方法，T支持所有类型

* 删

ARRAY用pop_back方法

MAP用erase(const GString& key)方法

* 改

SIMPLE属性和COMPLEX属性用update(T value)方法

ARRAY用update(int32_t idx, T value)方法

MAP用update(const GString& key, T value)方法

* 查

get_prop方法用于访问属性树的第一层子节点，get用于访问COMPLEX属性的成员

ARRAY通过get(int32_t idx)方法访问指定下标元素

MAP通过get(const GString& key)方法访问指定key的元素

ARRAY的遍历要先用size方法拿到大小，然后for循环配合get(int32_t idx)访问每个元素

MAP的遍历要先用keys方法拿到所有key，然后for循环配合get(const GString& key)访问每个元素

* 注意

属性系统的增删改查一定要用以上接口，否则会导致脏标记漏更新，从而导致属性同步错误

详细的增删改查使用方法可以参考代码中的property_sync_test函数示例


### 属性的同步
对于有client的entity来说，base/cell在创建/断线重连/灾备时会全遍历一次属性树将属性全同步到client

当属性发生增删改时，会给对应的属性节点打上脏标记，且向上递归遍历其父属性节点打上脏标记

差量同步时从根节点开始遍历属性树，将所有打了脏标记的属性同步到客户端，差量同步每帧执行

## <span id="component">component系统[（目录）](#catalogue)</span>
yjengine支持以组合的方式来扩充entity的功能，在regist_components方法中为entity注册组件
```
static void regist_components() {
    REGIST_COMPONENT(BaseAccount, CreateAvatarComponent);
}
```
component支持定义自己的rpc，定时器，以及属性（TODO），使component和entity解耦，做到component可插拔

### component的定义
```
class CreateAvatarComponent : public EntityComponentBase {
    GENERATE_COMPONENT_INNER(CreateAvatarComponent);

    COMP_RPC_DEFINE() {
        COMP_RPC_METHOD(RpcType::EXPOSED, component_rpc_test);
    }

    COMP_MIGRATE_TIMER_DEFINE() {
        COMP_MIGRATE_TIMER_DEF(component_timer_test);
    }
...
    virtual void on_ready() { ... }
};
```
component继承自EntityComponentBase类，和entity类似，component的定义也类不开以下宏和方法：
* GENERATE_COMPONENT_INNER - component的一些通用定义
* COMP_RPC_DEFINE - component的rpc定义
* COMP_MIGRATE_TIMER_DEFINE - 使component定时器支持迁移
* on_ready - 一般用于注册component定时器和事件

### 事件系统
component和entity以及component之间通过事件系统来通信，目的也是为了解耦，
```
// 注册事件
REGIST_EVENT("event_test", component_event_test);

// 发送事件
COMP_SEND_EVENT("event_test", "lalalala");
```

事件系统是典型的订阅发布模式，entity和component可以即是订阅者也是发布者

## <span id="flowchart">entity的创建/迁移/灾备/重连[（目录）](#catalogue)</span>
接下来，以EntityType_BaseWithCellAndClient为例，介绍一下entity的创建/迁移/灾备/断线重连流程：

### entity的创建
创建流程：

![](https://github.com/610789338/yjengine/blob/main/doc/%E6%B5%81%E7%A8%8B%E5%9B%BE/entity%E5%88%9B%E5%BB%BA%E6%B5%81%E7%A8%8B.drawio.png)

创建流程分创建和ready通知两个阶段，时序为：
1. 创建base
2. 创建cell
3. 创建client
4. base收到ready通知
5. cell收到ready通知
6. client收到ready通知

上层在收到ready之后才可以进行逻辑操作

### entity的迁移
迁移流程由entity的cell部分发起，迁移的数据包括cell身上的属性，mailbox以及定时器，其中定时器的迁移比较特殊，除了定时器本身的定义外，还需要将定时器定义为可迁移的，否则定时器迁移会失败：
```
static void migrate_timer_define() {
    MIGRATE_TIMER_DEF(avatar_migrate_timer);
    MIGRATE_TIMER_DEF(base_rpc_timer);
}
```

另外，迁移期周期牵扯到base/cell/client之间的多个异步，所以在于开始迁移时base和client要cache发往old cell的rpc，待new cell创建完成后再将cache的rpc发往new cell

详细的迁移流程图如下：

![](https://github.com/610789338/yjengine/blob/main/doc/%E6%B5%81%E7%A8%8B%E5%9B%BE/entity%E8%BF%81%E7%A7%BB%E6%B5%81%E7%A8%8B.drawio.png)

### entity的灾备
yjengine支持base和cell将自身的数据定时备份到对方身上，若base或cell所在的进程异常退出，则可以利用对方身上的备份数据来恢复自身，流程如下：

![](https://github.com/610789338/yjengine/blob/main/doc/%E6%B5%81%E7%A8%8B%E5%9B%BE/entity%E7%81%BE%E5%A4%87%E6%B5%81%E7%A8%8B.drawio.png)

灾备的重点在于：
1. 灾备本质上是数据回退，要求base和cell的备份数据要同时更新
2. base或者cell一方发生灾备回退时，另一方也要将自身的数据回退到对应的备份

另外由于base和cell的数据备份不是严格同步的，所有要求上层在编写业务逻辑时要遵循先扣资源再给资源的原则

### entity的断线重连
断线重连的重点在于：新客户端连上来后，要给旧客户端发送下线通知，通知base和cell更新自身的client mailbox

![](https://github.com/610789338/yjengine/blob/main/doc/%E6%B5%81%E7%A8%8B%E5%9B%BE/entity%E6%96%AD%E7%BA%BF%E9%87%8D%E8%BF%9E%E6%B5%81%E7%A8%8B.drawio.png)
