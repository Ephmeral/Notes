package main

import (
	"fmt"
	"io"
	"net"
	"sync"
	"time"
)

type Server struct {
	IP   string
	Port int

	// 在线用户的列表
	OnlineMap map[string]*User
	mapLock   sync.RWMutex

	// 消息广播的channel
	Message chan string
}

// 创建一个新的server接口
func NewServer(ip string, port int) *Server {
	return &Server{
		IP:        ip,
		Port:      port,
		OnlineMap: make(map[string]*User),
		Message:   make(chan string),
	}
}

// 监听Message广播消息Channel的goroutine，一旦有消息就发送给全部在线的User
func (this *Server) ListenMessager() {
	for {
		msg := <-this.Message

		// 将msg发送给全部在线的User
		this.mapLock.Lock()
		for _, cli := range this.OnlineMap {
			cli.C <- msg
		}
		this.mapLock.Unlock()
	}
}

// 广播消息的方法
func (this *Server) BroadCast(user *User, msg string) {
	sendMsg := "[" + user.Addr + "]" + user.Name + ":" + msg
	this.Message <- sendMsg
}

func (this *Server) Handler(conn net.Conn) {
	// 处理hanlder

	user := NewUser(conn, this)
	onlinemsg := "[" + user.Addr + "]" + user.Name + ":连接建立成功"
	fmt.Println(onlinemsg)

	// 用户已经上线
	user.Online()

	// 监听用户是否活跃的channel
	isLive := make(chan bool)

	// 接受客户端发送的消息
	go func() {
		buf := make([]byte, 4096)
		for {
			n, err := conn.Read(buf)
			if n == 0 {
				user.Offline()
				return
			}
			if err != nil && err != io.EOF {
				fmt.Println("Conn Read err:", err)
				return
			}

			// 提取用户的消息(取出\n)
			msg := string(buf[:n-1])
			// 用户针对msg进行消息处理
			user.DoMessage(msg)

			// 用户的任意消息，表示当前用户是活跃的
			isLive <- true
		}
	}()

	// 当前hanlder阻塞
	for {
		select {
		case <-isLive:
			// 当前用户是活跃的，应该重置定时器
			// 不做任何事情，为了激活select，更新下面的定时器
		case <-time.After(time.Second * 300):
			// 已经超时，将当前的User强制关闭
			user.SendMsg("你长时间没活动，已经被强制下线:" + user.Name)
			close(user.C) // 销毁使用的资源
			conn.Close()  // 关闭连接
			return        //runtime.Goexit(),退出当前的Handler
		}
	}

}

// 启动服务器接口
func (this *Server) Start() {
	// listen
	listener, err := net.Listen("tcp", fmt.Sprintf("%s:%d", this.IP, this.Port))
	if err != nil {
		fmt.Println("net.Listen err:", err)
		return
	}
	defer listener.Close()

	// 启动监听message的goroutine
	go this.ListenMessager()
	for {
		// accept
		conn, err := listener.Accept()
		if err != nil {
			fmt.Println("listener accept err:", err)
			continue
		}
		// do handler
		go this.Handler(conn)
	}
}
