//
// Created by zhouyang on 16-10-9.
//

#include "TcpServer.h"

#include "base/Logging.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "Sockets.h"
#include "SocketsOps.h"

#include <boost/bind.hpp>

#include <stdio.h>

using namespace muduo;

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr) :
        loop_(CHECK_NOTNULL(loop)),
        name_(listenAddr.toHostPort()),
        acceptor_(new Acceptor(loop, listenAddr)),
        started_(false),
        nextConnId_(1) {
    //set TcpServer::newConnection callback function to accptor's new Connection callback function
    acceptor_->setNewConnectionCallback(
            boost::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer() {

}

void TcpServer::start() {
    if (!started_)
    {
        started_ = true;
    }
    //acceptor begin to listen
    if (!acceptor_->listening())
    {
        loop_->runInLoop(boost::bind(&Acceptor::listen, get_pointer(acceptor_)));
    }
}

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {
    loop_->assertInLoopThread();
    char buf[32];
    //nextConnId is an integer begin with 1 and increase by 1
    snprintf(buf, sizeof buf, "#%d", nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;
    LOG_INFO << "TcpServer::newConnection [" << name_ << "] -new connection [" << connName << "] from" <<
             peerAddr.toHostPort();
    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    TcpConnectionPtr conn(new TcpConnection(loop_, connName, sockfd, localAddr, peerAddr));
    connections_[connName] = conn;
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    //set close callback function to new TcpConnection
    conn->setCloseCallback(boost::bind(&TcpServer::removeConnection, this, _1));
    conn->connectEstablished();
}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
    loop_->assertInLoopThread();
    LOG_INFO << "TcpServer::removeConnection [" << name_ << "] -connection " << conn->name();
    //erase TcpConnectionPtr from ConnectionMap in TcpServer
    ssize_t n = connections_.erase(conn->name());
    assert(n == 1);
    //使用boost bind延长TcpConnectionPtr的生命期直至运行connectDestroyed
    //connectDestroyed运行结束，conn就析构了
    loop_->queueInLoop(boost::bind(&TcpConnection::connectDestroyed, conn));
}

