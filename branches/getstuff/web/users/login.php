<?php
session_start();

header("Content-type: text/html; charset=utf-8");
//登录
if(!isset($_POST['submit'])){
    exit('非法访问!');
}

$username = htmlspecialchars($_POST['username']);
$password = $_POST['password'];

//包含数据库连接文件
$conn = mysql_pconnect("localhost","liyake","me_lyk");
if(!$conn)
{
	$errorcode = 3;
	die("connect db error!");
}
mysql_select_db("liyake",$conn);

//检测用户名及密码是否正确
$result = mysql_query("select * from users where username='".$username."' and password='".$password."' limit 1;");
if($result = mysql_fetch_array($result)){
	//登录成功
	$_SESSION['username'] = $username;
	$_SESSION['userid'] = $result['id'];
	$_SESSION['name'] = $result['name'];
	$_SESSION['isadmin'] = $result['isadmin'];
	exit('登录成功!');
}
exit('登录失败!');
?>