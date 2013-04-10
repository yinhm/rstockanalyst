<?php
	//http://mhchina.a24.cc/api/v1/getstuff/
	$date = DateTime::createFromFormat("Y-m-j",$_GET["date"]);
	if($date==null)
	{
		echo "Invalid date!";
		return;
	}
	
	$mcurl = curl_init();
	curl_setopt($mcurl,CURLOPT_URL,"http://mhchina.a24.cc/api/v1/getstuff/");
	curl_setopt($mcurl, CURLOPT_RETURNTRANSFER, 1);//设置是否返回信息
//	curl_setopt($mcurl, CURLOPT_HTTPHEADER, $header);//设置HTTP头
	curl_setopt($mcurl, CURLOPT_POST, 1);//设置为POST方式
	curl_setopt($mcurl, CURLOPT_POSTFIELDS, '{"sdb":true,"to":"'.$date->format('Y-m-j').'","from":"'.$date->format('Y-m-j').'"}');//POST数据
	$response = curl_exec($mcurl);//接收返回信息
//	echo $response;
	$json = json_decode($response);
	var_dump($json[$date->format('Y-m-j')]['ordo']['content']);
?>