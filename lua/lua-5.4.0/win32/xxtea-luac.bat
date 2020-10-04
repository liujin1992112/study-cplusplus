::-s 表示加密或者解密lua脚本的源文件夹路径
::-d 表示加密或者解密lua脚本输出文件夹路径
::-e true,表示采用lua脚本加密(默认不填为加密) false,表示解密
::-key  秘钥
::-sign 签名
xxtea-luac.exe -s hot/assets/src -d lib/hot/assets/src -e false -key lua16 -sign oxzjh