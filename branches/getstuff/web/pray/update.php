<?php
	session_start();
	/*
	错误码定义
	1 没有日期参数或日期参数不正确
	2 未获取到数据
	3 连接数据库失败
	*/
	//http://mhchina.a24.cc/api/v1/getstuff/
	header("Content-type: text/html; charset=utf-8");
?>
<head>
	<meta name="viewport" content="user-scalable=no, width=device-width" />  
	<title>更新结果</title>
</head>
<?php
	$text = "";
	$name = "";
	$error = "";
	if(isset($_POST["text"]))
		$text = trim($_POST["text"]);
	if($text=="")
	{
		$error= "请输入祈祷意向!";
		goto END;
	}
	if(isset($_POST['name']))
	{
		$name=trim($_POST['name']);
		$_SESSION['name'] = $name;
	}
	
	
	//先从数据库中获取
	$conn = mysql_pconnect("localhost","liyake","me_lyk");
	if(!$conn)
	{
		$error= "连接数据库失败!";
		goto END;
	}
	
	mysql_select_db("liyake",$conn);
	$result = mysql_query("insert into pray (name,text,createtime) values ('".$name."','".$text."',utc_timestamp());");
	if(mysql_query("select row_count();")<1)
	{
		$error = "添加祈祷意向失败，请稍后重试...";
		goto END;
	}
	
	$error = "更新成功！";
	
END:
	echo $error;
	echo '<form action="index.php" method="post">
	<input style="float:right;" type="submit" value=" 返  回 ">
</form>'
?>