<?php
	/*
	错误码定义
	1 没有日期参数或日期参数不正确
	2 未获取到数据
	3 连接数据库失败
	*/
	//http://mhchina.a24.cc/api/v1/getstuff/
	header("Content-type: text/html; charset=utf-8");
	require_once("chinese_conversion/convert.php");
	if(array_key_exists("date",$_GET)==false)
	{
		echo '{"error":1}';
		return;
	}
	$date = DateTime::createFromFormat("Y-m-j",$_GET["date"]);
	if(date('Y-m-d',strtotime($_GET["date"]))!=$_GET["date"])
	{
		echo '{"error":1}';
		return;
	}

	//返回的数据
	$stuff_mass = "";		//弥撒
	$stuff_med = "";		//日祷
	$stuff_comp = "";		//夜祷
	$stuff_let = "";		//诵读
	$stuff_lod = "";		//晨祷
	$stuff_thought = "";	//反省
	$stuff_ordo = "";		//礼仪
	$stuff_ves = "";		//晚祷
	$stuff_saint = "";		//圣人传记
	
	$isupdate = false;
	{
		//先从数据库中获取
		$conn = mysql_pconnect("localhost","liyake","me_lyk");
		if(!$conn)
		{
			echo '{"error":3}';
			return;
		}
		mysql_select_db("liyake",$conn);
		$result = mysql_query("select * from stuff where time='".$date->format('Y-m-j')."';");
		if(mysql_num_rows($result)>0)
		{
			$isupdate = true;
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
				
				goto END;
			}
			else if($row['lastupdate']==date('Y-m-d'))
			{
				echo '{"error":2}';
				return;
			}
		}
		else
		{
			goto GETSTUFF;
		}
	}

GETSTUFF:
	$mcurl = curl_init();
	curl_setopt($mcurl,CURLOPT_URL,"http://mhchina.a24.cc/api/v1/getstuff/");
	curl_setopt($mcurl, CURLOPT_RETURNTRANSFER, 1);//设置是否返回信息
//	curl_setopt($mcurl, CURLOPT_HTTPHEADER, $header);//设置HTTP头
	curl_setopt($mcurl, CURLOPT_POST, 1);//设置为POST方式
	curl_setopt($mcurl, CURLOPT_POSTFIELDS, '{"sdb":true,"to":"'.$date->format('Y-m-j').'","from":"'.$date->format('Y-m-j').'"}');//POST数据
	$response = curl_exec($mcurl);//接收返回信息
	$json = json_decode($response,true);
	if($json==null)
	{
		goto GETSTUFFERROR;
	}
	
	{
		//获取返回的数据
		$json_date = $json[$date->format('Y-m-j')];
		if($json_date == null)
		{
			goto GETSTUFFERROR;
		}
		
		$json_mass = $json_date['mass'];
		if($json_mass)
			$stuff_mass = zhconversion_hans($json_mass['content']);
		
		$json_med = $json_date['med'];
		if($json_med)
			$stuff_med = zhconversion_hans($json_med['content']);
		
		$json_comp = $json_date['comp'];
		if($json_comp)
			$stuff_comp = zhconversion_hans($json_comp['content']);
		
		$json_let = $json_date['let'];
		if($json_let)
			$stuff_let = zhconversion_hans($json_let['content']);

		$json_lod = $json_date['lod'];
		if($json_lod)
			$stuff_lod = zhconversion_hans($json_lod['content']);

		$json_thought = $json_date['thought'];
		if($json_thought)
			$stuff_thought = zhconversion_hans($json_thought['content']);
		
		$json_ordo = $json_date['ordo'];
		if($json_ordo)
			$stuff_ordo = zhconversion_hans($json_ordo['content']);
		
		$json_ves = $json_date['ves'];
		if($json_ves)
			$stuff_ves = zhconversion_hans($json_ves['content']);
		
		$json_saint = $json_date['saint'];
		if($json_saint)
			$stuff_saint = zhconversion_hans($json_saint['content']);
		goto INSERTSTUFF;
	}
	
GETSTUFFERROR:
	if($isupdate)
	{
		mysql_query("update stuff set lastupdate=curdate() where time='".$date->format('Y-m-j')."';");
	}
	else
	{
		mysql_query("insert into stuff (time,valid,lastupdate) values ('".$date->format('Y-m-j')."',0,curdate());");
	}
	echo '{"error":2}';
	return;
	
INSERTSTUFF:
	//插入到数据库
	if($isupdate)
	{
		mysql_query("update stuff set mass='".$stuff_mass."',med='".$stuff_med."',comp='".$stuff_comp."',let='".$stuff_let."',lod='".$stuff_lod
		."',thought='".$stuff_thought."',ordo='".$stuff_ordo."',ves='".$stuff_ves."',saint='".$stuff_saint."',valid=1,lastupdate=curdate() "
		."where time='".$date->format('Y-m-j')."';");
	}
	else
	{
		$result = mysql_query('insert into stuff (time,mass,med,comp,let,lod,thought,ordo,ves,saint,valid,lastupdate) values '.
		'("'.$date->format('Y-m-j').'","'.$stuff_mass.'","'.$stuff_med.'","'.$stuff_comp.'","'.$stuff_let.'","'.$stuff_lod
		.'","'.$stuff_thought.'","'.$stuff_ordo.'","'.$stuff_ves.'","'.$stuff_saint.'",1,curdate());');
	}
	goto END;

END:
	$retArray = array();
	$retArray['mass'] = $stuff_mass;		//弥撒
	$retArray['med'] = $stuff_med;		//日祷
	$retArray['comp'] = $stuff_comp;		//夜祷
	$retArray['let'] = $stuff_let;		//诵读
	$retArray['lod'] = $stuff_lod;		//晨祷
	$retArray['thought'] = $stuff_thought;	//反省
	$retArray['ordo'] = $stuff_ordo;		//礼仪
	$retArray['ves'] = $stuff_ves;		//晚祷
	$retArray['saint'] = $stuff_saint;		//圣人传记
	$ret = '{"mass":"'.$stuff_mass.'","med":"'.$stuff_med.'","comp":"'.$stuff_comp.'","let":"'.$stuff_let
	.'","lod":"'.$stuff_lod.'","thought":"'.$stuff_thought.'","ordo":"'.$stuff_ordo.'","ves":"'.$stuff_ves.'","saint":"'.$stuff_saint.'"}';
	echo $ret;
//	echo json_encode($retArray,JSON_UNESCAPED_UNICODE|JSON_UNESCAPED_SLASHES);
?>