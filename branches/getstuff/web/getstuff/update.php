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
	
	if(!isset($_SESSION['isadmin']))
	{
		die("请先登录！");	
	}
	if($_SESSION['isadmin']!='1')
	{
		echo $_SESSION['isadmin'];
		die("非管理员帐户！");
	}
	if(!isset($_POST['date']))
	{
		die("未获取到日期！");
	}
	
	$date = DateTime::createFromFormat("Y-m-j",$_POST["date"]);
	if(date('Y-m-d',strtotime($_POST["date"]))!=$_POST["date"])
	{
		die("日期格式不正确！");
	}
	
	//获取数据库中的数据
	$stuff_mass = "";		//弥撒
	$stuff_med = "";		//日祷
	$stuff_comp = "";		//夜祷
	$stuff_let = "";		//诵读
	$stuff_lod = "";		//晨祷
	$stuff_thought = "";	//反省
	$stuff_ordo = "";		//礼仪
	$stuff_ves = "";		//晚祷
	$stuff_saint = "";		//圣人传记
	
	//先从数据库中获取
	$conn = mysql_pconnect("localhost","liyake","me_lyk");
	if(!$conn)
	{
		die("connect db error!");
	}
	mysql_select_db("liyake",$conn);
	$result = mysql_query("select * from stuff where time='".$date->format('Y-m-j')."';");
	if(mysql_num_rows($result)<1)
	{
		die("未在数据库中找到相应日期的数据");
	}
	else
	{
		$row = mysql_fetch_array($result);
		if($row['valid']>0)
		{
			//已经拥有数据可以直接获取
			$stuff_mass = $row["mass"];		//弥撒
			$stuff_med = $row["med"];		//日祷
			$stuff_comp = $row["comp"];		//夜祷
			$stuff_let = $row["let"];		//诵读
			$stuff_lod = $row["lod"];		//晨祷
			$stuff_thought = $row["thought"];	//反省
			$stuff_ordo = $row["ordo"];		//礼仪
			$stuff_ves = $row["ves"];		//晚祷
			$stuff_saint = $row["saint"];		//圣人传记
		}
	}
	
	{
		//获取post中的数据
		if(isset($_POST['mass']))
			$stuff_mass = $_POST['mass'];
		if(isset($_POST['med']))
			$stuff_med = $_POST['med'];
		if(isset($_POST['comp']))
			$stuff_comp = $_POST['comp'];
		if(isset($_POST['let']))
			$stuff_let = $_POST['let'];
		if(isset($_POST['lod']))
			$stuff_lod = $_POST['lod'];
		if(isset($_POST['thought']))
			$stuff_thought = $_POST['thought'];
		if(isset($_POST['ordo']))
			$stuff_ordo = $_POST['ordo'];
		if(isset($_POST['ves']))
			$stuff_ves = $_POST['ves'];
		if(isset($_POST['saint']))
			$stuff_saint = $_POST['saint'];
	}
	
	//插入到数据库
	mysql_query("update stuff set mass='".$stuff_mass."',med='".$stuff_med."',comp='".$stuff_comp."',let='".$stuff_let."',lod='".$stuff_lod
	."',thought='".$stuff_thought."',ordo='".$stuff_ordo."',ves='".$stuff_ves."',saint='".$stuff_saint."',valid=2,lastupdate=curdate() "
	."where time='".$date->format('Y-m-j')."';");
	
	exit("更新成功！");
//	echo json_encode($retArray,JSON_UNESCAPED_UNICODE|JSON_UNESCAPED_SLASHES);
?>