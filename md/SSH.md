## 1. SSH key 删除以及生成

```shell
cd ~/.ssh
//查看是否存在ssh文件
```

\#如果.ssh文件夹不存在，可以执行指令自行创建

```shell
mkdir ~/.ssh
```

//删除本地电脑上的已经存在的ssh key方法

进入本地电脑的ssh文件目录，删除存在的ssh  key,只需要将目录下的id_rsa、id_rsa.pub删除，其中的know_hosts文件，有时需要删除，有时不需要，这个具体操作时可以尝试就可以了

 

//生成 RSA 密钥对

```shell
 ssh-keygen -t rsa -C "你的注册邮箱@xxx.com"
```

\# 为了方便，全程回车即可（这个过程中会出现诸如key pair，passphrase等的请求，一律回车即可）



//查看公钥内容（即用来做ssh key用的）

```shell
cat ~/.ssh/id_rsa.pub
```

如果id_rsa.pub文件找不到，可以在终端中使用open ~/.ssh，打开ssh文件，可以看到id_rsa,id_rsa.pub文件；也可以在前往中输入"~/.ssh"来查看是否存在id_rsa.pub文件

## 2. GitLab or GitHub 配置SSH key

1. 成好ssh key之后， 打开./ssh/id_rsa.pub文件， copy其中内容。
2. 打开GitLab 或者GitHub， 进入设置的SSH Key的界面， 粘贴进去就好。