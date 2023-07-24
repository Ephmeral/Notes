package main

import (
	"net"
	"strings"
)

type User struct {
	Name string
	Addr string
	C    chan string
	conn net.Conn

	server *Server
}

// 创建一个用户的API
func NewUser(conn net.Conn, server *Server) *User {
	userAddr := conn.RemoteAddr().String()

	user := &User{
		Name:   userAddr,
		Addr:   userAddr,
		C:      make(chan string),
		conn:   conn,
		server: server,
	}
	// 启动监听当前user channel消息的goroutine
	go user.ListenMessage()

	return user
}

// 用户上线的业务
func (this *User) Online() {
	// 用户上线，将用户加入到onlineMap总
	this.server.mapLock.Lock()
	this.server.OnlineMap[this.Name] = this
	this.server.mapLock.Unlock()

	// 广播当前用户上线消息
	this.server.BroadCast(this, "已上线")
}

// 用户下线的业务
func (this *User) Offline() {
	// 用户下线，将用户从onlineMap中删除
	this.server.mapLock.Lock()
	delete(this.server.OnlineMap, this.Name)
	this.server.mapLock.Unlock()

	// 广播当前用户下线消息
	this.server.BroadCast(this, "下线")
}

// 给当前User对应的客户端发送消息
func (this *User) SendMsg(msg string) {
	this.conn.Write([]byte(msg + "\n"))
}

// 用户处理消息的业务
func (this *User) DoMessage(msg string) {
	if msg == "who" {
		// 查询当前在线的用户有哪些
		this.server.mapLock.Lock()
		for _, user := range this.server.OnlineMap {
			onlineMsg := "[" + user.Addr + "]" + user.Name + ":" + "在线...\n"
			this.SendMsg(onlineMsg)
		}
		this.server.mapLock.Unlock()
	} else if len(msg) > 7 && msg[:7] == "rename|" {
		// 消息格式：rename|张三
		newName := strings.Split(msg, "|")[1]

		// 判断name是否已经存在了
		_, ok := this.server.OnlineMap[newName]
		if ok {
			this.SendMsg("当前用户名被使用\n")
		} else {
			this.server.mapLock.Lock()
			delete(this.server.OnlineMap, this.Name)
			this.server.OnlineMap[newName] = this
			this.server.mapLock.Unlock()

			this.Name = newName
			this.SendMsg("已经更新用户名：" + newName)
		}
	} else if len(msg) > 4 && msg[:3] == "to|" {
		// 消息格式：to|张三|消息内容
		// 1 获取对方用户名
		remoteName := strings.Split(msg, "|")[1]
		if remoteName == "" {
			this.SendMsg("消息格式不正确，请使用：\"to|张三|你好\"格式\n")
			return
		}
		// 2 根据用户名 得到对方User对象
		remoteUser, ok := this.server.OnlineMap[remoteName]
		if !ok {
			this.SendMsg("该用户不存在\n")
			return
		}
		// 3 获取消息内容，根据对方User对象将消息内容发送过去
		content := strings.Split(msg, "|")[2]
		if content == "" {
			this.SendMsg("无消息内容，请重新发送\n")
			return
		}
		remoteUser.SendMsg(this.Name + "对您说:" + content)
	} else {
		this.server.BroadCast(this, msg)
	}
}

// 监听当前User channel的方法，一旦有消息，就直接发送给对端客户端
func (this *User) ListenMessage() {
	for {
		msg := <-this.C
		this.conn.Write([]byte(msg + "\n"))
	}
}
